//
//  jlanim.c
//  jlsdl
//
//  Created by dp on 4/10/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#include "jlsdl.h"

jlmovie* jlmovie_create_with_texture(SDL_Texture* text)
{
    jlmovie *ret = malloc(sizeof(jlmovie));
    if(ret){
        memset(ret,0, sizeof(jlmovie));
        INIT_LIST_HEAD(&ret->list);
        ret->texture = text;
    }
    return ret;
}

jlmovie* jlmovie_create(char *file)
{
    jlmovie *ret = malloc(sizeof(jlmovie));
    if(ret){
        memset(ret,0, sizeof(jlmovie));
        INIT_LIST_HEAD(&ret->list);
        SDL_Surface *sur = IMG_Load(file);
        if(sur){
            ret->texture = SDL_CreateTextureFromSurface(renderer, sur);
            SDL_FreeSurface(sur);
        }
    }
    return ret;
}
int jlmovie_add_texture(jlanim *anim, SDL_Texture *text)
{
    if(!anim || !text){
        return -1;
    }
    if(anim->type != JLANIM_MOVIE)
        return -1;
    jlmovie *m = jlmovie_create_with_texture(text);
    list_add_tail(&m->list, &anim->moviehead);
    anim->movienum ++;
    return 0;
}
int jlmovie_add_file(jlanim *anim, char *newfile)
{
    if(!anim || !newfile)
        return -1;
    if(anim->type != JLANIM_MOVIE)
        return -1;
    jlmovie *m = jlmovie_create(newfile);
    list_add_tail(&m->list, &anim->moviehead);
    anim->movienum ++;
    return 0;
}
int jlmovie_destroy(jlmovie *movie)
{
    list_del(&movie->list);
    SDL_DestroyTexture(movie->texture);
    free(movie);
    return 0;
}

SDL_Texture *jlanim_get_movie_next_texture(jlanim *a)
{
    if(!a)
        return NULL;
    if(a->moviecur == NULL){
        a->moviecur = &a->moviehead;
    }
    a->moviecur = a->moviecur->next;
    if(a->moviecur != &a->moviehead){
        jlmovie *m = list_entry(a->moviecur, jlmovie, list);
        if(m){
            return m->texture;
        }
    }else{
        //repeat?
        if(a->repeate == 0)
            return NULL;
        a->moviecur = a->moviehead.next;
        jlmovie *m = list_entry(a->moviecur, jlmovie, list);
        if(m){
            return m->texture;
        }
    }
    return NULL;
}
SDL_Texture *jlanim_get_movie_texture_at_index(jlanim *a, int index)
{
    if(!a || a->type != JLANIM_MOVIE)
        return NULL;
    struct list_head *pos;
    int i = 0;
    list_for_each(pos, &a->moviehead){
        if(i == index){
            jlmovie *m = list_entry(pos, jlmovie, list);
            return m->texture;
        }
        i++;
    }
    return NULL;
}
int jlanim_set_move_point(jlanim* anim,int fx, int fy, int tx, int ty)
{
    anim->dstpoint.x = tx;
    anim->dstpoint.y = ty;
    anim->cx = anim->srcpoint.x = fx;
    anim->cy = anim->srcpoint.y = fy;
    int freq = anim->duration / JLDELAY;
    anim->dx = (double)(tx-fx) / (double)freq;
    anim->dy = (double)(ty-fy) / (double)freq;
    return 0;
}
int jlanim_destroy(jlanim *anim)
{
    list_del(&anim->list);
    list_del(&anim->slist);
    if(anim->type == JLANIM_MOVIE){
        struct list_head *pos, *n;
        list_for_each_safe(pos, n, &anim->moviehead){
            jlmovie *m = list_entry(pos, jlmovie, list);
            jlmovie_destroy(m);
        }
    }
    if(anim->name){
        free(anim->name);
    }
    return 0;
}
int jlanim_set_name(jlanim* a, char *name)
{
    if(!a || !name)
        return -1;
    a->name = strdup(name);
    return 0;
}
jlanim* jlanim_create(int type, int ms, int rep)
{
    jlanim *anim = NULL;
    anim = malloc(sizeof(jlanim));
    if(anim){
        memset(anim, 0, sizeof(jlanim));
        INIT_LIST_HEAD(&anim->list);
        anim->type = type;
        anim->duration = ms;
        if(type == JLANIM_MOVIE)
            anim->rest = 0;
        else
            anim->rest = ms;
        anim->repeate = rep;
        if(type == JLANIM_MOVIE){
            INIT_LIST_HEAD(&anim->moviehead);
        }
    }
    return  anim;
}