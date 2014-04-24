//
//  jldirector.c
//  jlsdl
//
//  Created by dp on 4/9/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#include "jlsdl.h"
static jldirector *director = NULL;
jldirector* jldirector_default()
{
    if(director == NULL){
        director = malloc(sizeof(jldirector));
        if(director){
            memset(director, 0 , sizeof(jldirector));
            INIT_LIST_HEAD(&director->scenehead);
        }
    }
    return director;
}
int jldirector_destroy()
{
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, &director->scenehead){
        jlscene *s = list_entry(pos, jlscene, list);
        list_del(&s->list);
        jlscene_destroy(s);
    }
    free(director);
    director = NULL;
    return 0;
}
int jldirector_replace_scene(jlscene *s, int animatino_type)
{
    jldirector_pop_scene(animatino_type);
    jldirector_push_scene(s, animatino_type);
    return 0;
}
int jldirector_push_scene(jlscene *s, int animation_type)
{
    list_add_tail(&s->list, &director->scenehead);
    return 0;
}

jlscene* jldirector_pop_scene(int animation_type)
{
    if(director->scenehead.prev == &director->scenehead)
        return 0;
    struct list_head *pos = director->scenehead.prev;
    jlscene *s = list_entry(pos, jlscene, list);
    list_del(&s->list);
    jlscene_destroy(s);
    return s;
}

int jldirector_pop_to_scene(jlscene *s, int animation_type)
{
    do{
        if(director->scenehead.next != &director->scenehead){
            jlscene *tmp = list_entry(director->scenehead.prev, jlscene, list);
            if(tmp != s){
                jldirector_pop_scene(0);
            }
        }else{
            break;
        }
    }while(1);
    return 0;
}

jlscene* jldirector_current_scene()
{
    if(director->scenehead.next == &director->scenehead){
        return NULL;
    }
    jlscene *ret = list_entry(director->scenehead.prev, jlscene, list);
    return ret;
}
