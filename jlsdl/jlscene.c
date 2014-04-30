//
//  jlscene.c
//  jlsdl
//
//  Created by dp on 4/9/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#include "jlsdl.h"

int jlscene_render(jlscene *s, unsigned int ticks)
{
    if(!s)
        return 0;
    struct list_head *pos;
    list_for_each(pos, &s->layerhead){
        jllayer *l = list_entry(pos, jllayer, layerlist);
        jllayer_render(l, ticks);
    }
    return 0;
}
jlscene* jlscene_create()
{
    jlscene *ret = NULL;
    ret = malloc(sizeof(jlscene));
    if(ret){
        memset(ret, 0, sizeof(jlscene));
        INIT_LIST_HEAD(&ret->list);
        INIT_LIST_HEAD(&ret->layerhead);
    }
    return ret;
}
int jlscene_destroy(jlscene *s)
{
    if(!s)
        return -1;
    if(s->list.next != LIST_POISON1)
        list_del(&s->list);//before destroy, please pop it first
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, &s->layerhead){
        jllayer *l = list_entry(pos, jllayer, layerlist);
        if(l){
            jllayer_destroy(l);
        }
    }
    free(s);
    return 0;
}

int jlscene_add_layer(jlscene *scene, jllayer *layer)
{
    if(!scene || !layer)
        return -1;
    layer->scene = scene;
    list_add_tail(&layer->layerlist, &scene->layerhead);
    return 0;
}
int jlscene_add_layer_at_index(jlscene *scene, jllayer *layer, int index)
{
    if(!scene || !layer || index < 0)
        return -1;
    struct list_head *pos;
    if(index == 0){
        list_add(&layer->layerlist, &scene->layerhead);
        return 0;
    }
    int i = 1;
    list_for_each(pos, &scene->layerhead){
        if(i == index){
            list_add(&layer->layerlist, pos);
        }
        i++;
    }
    return 0;
}

int jlscene_del_layer(jlscene *scene, jllayer *layer)
{
    if(!scene || !layer)
        return -1;
    struct list_head *pos, *n;
    list_for_each_safe( pos, n, &scene->layerhead){
        jllayer *l = list_entry(pos, jllayer , layerlist);
        if(l == layer){
            list_del(&layer->layerlist);
            return 0;
        }
    }
    return 0;
}
int jlscene_del_layer_at_index(jlscene *scene, jllayer *layer, int index)
{
    return 0;
}
int jlscene_del_top_layer(jlscene *scene)
{
    list_del(scene->layerhead.prev);
    return 0;
}

jllayer* jlscene_get_layer_at_index(jlscene *scene, int index)
{
    jllayer *ret = NULL;
    return ret;
}
jllayer* jlscene_get_top_layer(jlscene *scene)
{
    jllayer *ret = list_entry(scene->layerhead.prev, jllayer, layerlist);
    return ret;
}
