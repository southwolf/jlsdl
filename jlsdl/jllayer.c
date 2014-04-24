//
//  jllayer.c
//  jlsdl
//
//  Created by dp on 4/9/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#include "jlsdl.h"

int jllayer_set_pos(jllayer *l, int x, int y)
{
    return jlnode_set_pos(l->node, x, y);
}
int jllayer_set_wh(jllayer *l, int w, int h)
{
    return jlnode_set_wh(l->node, w, h);
}
int jllayer_set_texture_from_file(jllayer* l, char *file)
{
    return jlnode_create_texture_from_file(l->node, file);
}
int jllayer_set_texture_from_surface(jllayer *l,  SDL_Surface *sur)
{
    jlnode_create_texture_from_surface(l->node, sur);
    return 0;
}
int jllayer_set_textures_from_surface(jllayer *l,  SDL_Surface *normal, SDL_Surface *down)
{
    if(!l)
        return -1;
    jlnode_create_texture_from_surface(l->node, normal);
    l->node->shouldDestroyTexture = 0;
    l->shouldDestroyTextures = 1;
    l->normalTexture = l->node->texture;
    l->downTexture = SDL_CreateTextureFromSurface(renderer, down);
    return 0;
}
int jllayer_contain_point(jllayer *l, int x, int y)
{
    return jl_rect_contain_point(l->node->frame, x, y);
}
int jllayer_render(jllayer* l, unsigned int ticks)
{
    jlnode_render(l->node, ticks);
    return 0;
}
int jllayer_add_node(jllayer *layer, jlnode* node)
{
    jlnode_add_child(layer->node, node);
    return 0;
}
int jllayer_add_node_at_index(jllayer *layer, jlnode *node, int index)
{
    jlnode_add_child_at_index(layer->node, node, index);
    return 0;
}

jlnode* jllayer_get_node_by_tag(jllayer *layer, int tag)
{
    struct list_head *pos;
    list_for_each(pos, &layer->node->childhead){
        jlnode *n = list_entry(pos, jlnode, nlist);
        if(n->tag == tag){
            return n;
        }
    }
    return NULL;
}

int jllayer_destroy(jllayer *l)
{
    if(l->layerlist.next && l->layerlist.next != LIST_POISON1 )
        list_del(&l->layerlist);
    if(l->list.next && l->list.next != LIST_POISON1 )
        list_del(&l->list);
    jlnode_destroy(l->node);
    if(l->shouldDestroyTextures){
        if(l->normalTexture){
            SDL_DestroyTexture(l->normalTexture);
        }
        if(l->downTexture){
            SDL_DestroyTexture(l->downTexture);
        }
    }
    free(l);
    return 0;
}

jllayer* jllayer_create()
{
    jllayer *ret = NULL;
    ret = malloc(sizeof(jllayer));
    if(ret){
        memset(ret, 0, sizeof(jllayer));
        INIT_LIST_HEAD(&ret->layerlist);
        INIT_LIST_HEAD(&ret->list);
        ret->node = jlnode_create();
        if(!ret->node){
            free(ret);
            return NULL;
        }
    }
    return ret;
}

jllayer* jllayer_create_button(char *title, SDL_Color color, TTF_Font* font, layer_event_handler handler)
{
    jllayer *l = jllayer_create();
    if(l){
        SDL_Surface *sur, *surtouch;
        sur = TTF_RenderText_Solid(font, title, color);
        color.r = 233;
        color.g = 233;
        color.b = 233;
        surtouch = TTF_RenderText_Solid(font, title, color);
        jllayer_set_textures_from_surface(l, sur, surtouch);
        SDL_FreeSurface(sur);
        SDL_FreeSurface(surtouch);
        l->touchUpHandler = handler;
    }
    return l;
}