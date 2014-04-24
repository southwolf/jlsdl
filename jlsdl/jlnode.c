//
//  jlnode.c
//  jlsdl
//
//  Created by dp on 4/9/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#include "jlsdl.h"

int jlnode_move(jlnode *node, jlanim *a)
{
    a->rest -= JLDELAY;
    a->cx += a->dx;
    a->cy += a->dy;
    jlnode_set_pos(node, a->cx, a->cy);
    if(a->rest <= 0){
        if(a->repeate){
            a->cx  = a->srcpoint.x;
            a->cy = a->srcpoint.y;
            a->rest = a->duration;
        }else{
            jlnode_unrun_animation(node, a);
            if(a->callback){
                a->callback(node);
            }
            return -1;
        }
    }
    return 0;
}
int jlnode_render(jlnode *node, unsigned int ticks)
{
    if(!node)
        return -1;
    if(node->ishidden){
        return 0;
    }
    struct list_head *pos, *n;
    double angle = 0;
    list_for_each_safe(pos, n, &node->animhead){
        jlanim *a = list_entry(pos, jlanim, list);
        if(a->type == JLANIM_MOVE){
            if(jlnode_move(node, a) < 0){
                return -1;
            }
        }else if(a->type == JLANIM_ROTATE){
            a->angle += 360/(a->duration/JLDELAY);
            angle = a->angle;
        }else if(a->type == JLANIM_FLIP){
            
        }else if(a->type == JLANIM_MOVIE){
            a->rest -= (ticks - a->lastTick);
            a->lastTick = ticks;
            if(a->rest <= 0){
                SDL_Texture *tmp = jlanim_get_movie_next_texture(a);
                
                if(tmp == NULL){
                    jlnode_unrun_animation(node, a);
                    continue;
                }
                node->texture = tmp;
                a->rest = a->duration / a->movienum;
            }
        }
    }
    SDL_RenderCopyEx(renderer, node->texture, NULL, &node->frame, angle, NULL, SDL_FLIP_NONE);
    list_for_each_safe (pos, n, &node->childhead){
        jlnode *n = list_entry(pos, jlnode, nlist);
        jlnode_render(n, ticks);
    }
    return 0;
}
jlnode* jlnode_create()
{
    jlnode *ret = NULL;
    ret = malloc(sizeof(jlnode));
    if(ret == NULL)
        return NULL;
    memset(ret, 0, sizeof(jlnode));
    INIT_LIST_HEAD(&ret->childhead);
    INIT_LIST_HEAD(&ret->list);
    INIT_LIST_HEAD(&ret->nlist);
    INIT_LIST_HEAD(&ret->animhead);
    INIT_LIST_HEAD(&ret->sanimhead);
    return ret;
}
jlanim* jlnode_get_anim_by_name(jlnode* node, char *name)
{
    struct list_head *pos;
    list_for_each(pos, &node->sanimhead){
        jlanim *a = list_entry(pos, jlanim, slist);
        if(a && a->name && strcmp(a->name, name) == 0){
            return a;
        }
    }
    return NULL;
}
int jlnode_remove_from_super(jlnode *node)
{
    if(!node)
        return -1;
    list_del(&node->nlist);
    return 0;
}

int jlnode_destroy(jlnode* node)
{
    if(!node)
        return -1;
    //remove from super
    if(node->nlist.next != LIST_POISON1)
        list_del(&node->nlist);
    if(node->list.next != LIST_POISON1)
        list_del(&node->list);
    //remove all children
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, &node->childhead){
        jlnode *jn = list_entry(pos, jlnode, nlist);
        if(jn){
            jlnode_destroy(jn);
        }
    }
    if(node->texture && node->shouldDestroyTexture){
        SDL_DestroyTexture(node->texture);
    }
    free(node);
    return 0;
}

int jlnode_add_child(jlnode *parent, jlnode *child)
{
    if(!parent || !child)
        return -1;
    list_add_tail(&child->nlist, &parent->childhead);
    child->supernode = parent;
    return 0;
}
int jlnode_add_child_at_index(jlnode *parent, jlnode *child, int index)
{
    if(!parent || !child || index < 0)
        return -1;
    struct list_head *pos;
    int i = 0;
    list_for_each(pos, &parent->childhead){
        if(i == index){
            list_add(pos, &child->nlist);
            return 0;
        }
        i++;
    }
    list_add_tail(&child->nlist, &parent->childhead);
    return 0;
}

jlnode* jlnode_get_child_by_tag(jlnode *parent, int tag)
{
    if(!parent)
        return NULL;
    struct list_head *pos;
    list_for_each(pos, &parent->childhead){
        jlnode *jn = list_entry(pos, jlnode, nlist);
        if(jn->tag == tag){
            return jn;
        }
    }
    return NULL;
}
jlnode* jlnode_get_child_at_index(jlnode *parent, int index)
{
    if(!parent)
        return NULL;
    int i = 0;
    struct list_head *pos;
    list_for_each(pos, &parent->childhead){
        if(i == index){
            return list_entry(pos, jlnode, nlist);
        }
        i++;
    }
    return list_entry(parent->childhead.prev, jlnode, nlist);
}
SDL_Texture * jlnode_create_texture(jlnode *node)
{
    if(node == NULL)
        return NULL;
    if(node->texture){
        return node->texture;
    }
    SDL_Surface *sur = SDL_CreateRGBSurface(SDL_SWSURFACE, node->frame.w, node->frame.h, 32, 0, 0, 0, 0);
    node->texture = SDL_CreateTextureFromSurface(renderer, sur);
    SDL_FreeSurface(sur);
    node->shouldDestroyTexture = 1;
    return node->texture;
}
int jlnode_set_frame(jlnode *node, SDL_Rect r)
{
    if(!node){
        return -1;
    }
    jlnode_set_wh(node, r.w, r.h);
    jlnode_set_pos(node, r.x, r.y);
    return 0;
}
int jlnode_set_background_color(jlnode *node, SDL_Color color)
{
    if(!node)
        return -1;
    node->backgroundColor = color;
    if(node->shouldDestroyTexture && node->texture){
        SDL_DestroyTexture(node->texture);
    }
    SDL_Surface *sur = SDL_CreateRGBSurface(0, node->frame.w, node->frame.h, 32, 0, 0, 0, 0);
    SDL_FillRect(sur, NULL, jl_map_color(&node->backgroundColor));
    node->texture = SDL_CreateTextureFromSurface(renderer, sur);
    node->shouldDestroyTexture = 1;
    SDL_FreeSurface(sur);
    return 0;
}
int jlnode_create_texture_from_surface(jlnode* node, SDL_Surface* sur)
{
    if(!node || !sur)
        return -1;
    if(node->shouldDestroyTexture && node->texture){
        SDL_DestroyTexture(node->texture);
    }
    node->frame.w = sur->clip_rect.w;
    node->frame.h = sur->clip_rect.h;
    node->texture = SDL_CreateTextureFromSurface(renderer, sur);
    node->shouldDestroyTexture = 1;
    return 0;
}
int jlnode_create_texture_from_file(jlnode *node, char *path)
{
    if(!node || !path)
        return -1;
    if(node->shouldDestroyTexture && node->texture){
        SDL_DestroyTexture(node->texture);
    }
    SDL_Surface *sur = IMG_Load(path);
    if(sur){
        node->texture = SDL_CreateTextureFromSurface(renderer, sur);
        if(node->texture == NULL){
            return -1;
        }
        node->frame.w = sur->clip_rect.w;
        node->frame.h = sur->clip_rect.h;
        SDL_FreeSurface(sur);
        node->shouldDestroyTexture = 1;
    }
    return 0;
}
int jlnode_set_texture(jlnode*node, SDL_Texture *texture)
{
    if(node->shouldDestroyTexture && node->texture){
        SDL_DestroyTexture(node->texture);
    }
    node->texture = texture;
    node->shouldDestroyTexture = 0;
    return 0;
}
int jlnode_is_anim_running(jlnode *node, jlanim *anim)
{
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, &node->animhead){
        jlanim *a = list_entry(pos, jlanim, list);
        if(a == anim){
            return 1;
        }
        if(a->type == JLANIM_MOVE && anim->type == JLANIM_MOVE){
            jlnode_unrun_animation(node, a);
        }
            
    }
    return 0;
}
int jlnode_run_animation(jlnode *node, jlanim *anim)
{
    if(jlnode_is_anim_running(node, anim) == 0){
        anim->lastTick = SDL_GetTicks();
        list_add_tail(&anim->list, &node->animhead);
    }
    return 0;
}
int jlnode_unrun_animation(jlnode *node, jlanim *anim)
{
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, &node->animhead){
        jlanim *a = list_entry(pos, jlanim, list);
        if(a == anim){
            //JUST delete from anim list! dot not destroy
            list_del(&anim->list);
            return 0;
        }
    }
    return 0;

}
int jlnode_add_animation(jlnode *node, jlanim *anim)
{
    list_add_tail(&anim->slist, &node->sanimhead);
    return 0;
}
int jlnode_del_animation(jlnode *node, jlanim *anim)
{
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, &node->sanimhead){
        jlanim *a = list_entry(pos, jlanim, slist);
        if(a == anim){
            jlanim_destroy(a);
            return 0;
        }
    }
    return 0;
}
int jlnode_set_wh(jlnode *node, int w, int h)
{
    if(!node)
        return -1;
    node->frame.w = w;
    node->frame.h = h;
    return 0;
}
int jlnode_set_pos(jlnode *node, int x, int y)
{
    if(!node)
        return -1;
    node->frame.x = x - node->frame.w/2;
    node->frame.y = y - node->frame.h/2;
    return 0;
}
jlnode* jlnode_create_label(char* title, TTF_Font* font, SDL_Color color)
{
    jlnode *n = jlnode_create();
    if(n){
        SDL_Surface *sur;
        sur = TTF_RenderText_Solid(font, title, color);
        jlnode_create_texture_from_surface(n, sur);
        SDL_FreeSurface(sur);
    }
    return n;
}