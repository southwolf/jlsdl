//
//  jlsdl.m
//  jlsdl
//
//  Created by dp on 4/9/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#include "jlsdl.h"

int screen_height, screen_width;
SDL_Window *window;
SDL_Renderer *renderer;

static struct list_head eventlist;
static struct list_head schdlist;

SDL_Window * jl_window()
{
    return window;
}

SDL_Renderer * jl_renderer()
{
    return renderer;
}
jlschd* jl_schedule_create()
{
    jlschd *ret = malloc(sizeof(jlschd));
    if(ret){
        memset(ret, 0, sizeof(jlschd));
        INIT_LIST_HEAD(&ret->list);
    }
    return  ret;
}
int jl_schedule_add(jlschd *schd)
{
    list_add_tail(&schd->list, &schdlist);
    return 0;
}
int jl_schedule_del(jlschd *schd)
{
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, &schdlist){
        jlschd *tmp = list_entry(pos, jlschd, list);
        if(tmp->func == schd->func){
            list_del(pos);
            return 0;
        }
    }
    return -1;
}
int jl_update_screen(unsigned int ticks)
{
    jlscene *s = jldirector_current_scene();
    if(s){
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        jlscene_render(s, ticks);
        SDL_RenderPresent(renderer);
    }
    return 0;
}
int jl_animation_update()
{
    return 0;
}

int jl_loop()
{
    int done;
    SDL_Event event;
    /* Enter render loop, waiting for user to quit */
    done = 0;
    struct list_head *pos, *n;
    jl_event *je;
    int ret;
    
    while (!done) {
        unsigned int bft = SDL_GetTicks();
        int en = 10;//最多处理10次事件
        do{
            ret = SDL_PollEvent(&event);
            if(ret > 0){
                list_for_each(pos, &eventlist){
                    je = list_entry(pos, jl_event, list);
                    if(je->e.type == event.type && je->handler){
                        je->handler(&event);
                        break;
                    }
                }
            }
            --en;
        }while(ret > 0 && en > 0);
        
        list_for_each_safe(pos, n, &schdlist){
            jlschd *sch = list_entry(pos, jlschd, list);
            if(bft - sch->lastCalledTicket > sch->interval){
                sch->lastCalledTicket = bft;
                sch->func();
            }
        }
        //show all nodes
        jl_update_screen(bft);
        unsigned int aft = SDL_GetTicks();
        unsigned int delt = aft - bft;
        if(delt < JLDELAY){
            delt = JLDELAY - delt;
            SDL_Delay(delt);
        }
    }
    /* shutdown SDL */
    SDL_Quit();
    return 0;
}
int platform_init()
{
#ifdef IOS
    return platform_ios_init();
#endif
    return 0;
}
int jl_quit(SDL_Event* e)
{
    return  0;
}

int jl_touch_motion(SDL_Event *e)
{
    int x = e->tfinger.x * screen_width;
    int y = e->tfinger.y * screen_height;
    struct list_head *pos;
    
    jlscene *s = jldirector_current_scene();
    if(!s)
        return 0;
    list_for_each(pos, &s->layerhead){
        jllayer *l = list_entry(pos, jllayer, layerlist);
        if(l->touchMotionHandler){
            if(jllayer_contain_point(l, x, y)){
                l->touchMotionHandler(e, l);
                break;
            }
        }
    }
    return 0;

}
int jl_touch_up(SDL_Event* e)
{
    int x = e->tfinger.x * screen_width;
    int y = e->tfinger.y * screen_height;
    struct list_head *pos;
    jlscene *s = jldirector_current_scene();
    if(!s)
        return 0;
    list_for_each(pos, &s->layerhead){
        jllayer *l = list_entry(pos, jllayer, layerlist);
        if(jllayer_contain_point(l, x, y)){
            if(l->normalTexture){
                l->node->texture = l->normalTexture;
            }
            if(l->touchUpHandler){
                l->touchUpHandler(e, l);
                break;
            }
        }
    }
    return 0;
}
int jl_touch_down(SDL_Event *e)
{
    int x = e->tfinger.x * screen_width;
    int y = e->tfinger.y * screen_height;
    struct list_head *pos;
    jlscene *s = jldirector_current_scene();
    if(!s)
        return 0;
    list_for_each(pos, &s->layerhead){
        jllayer *l = list_entry(pos, jllayer, layerlist);
        if(jllayer_contain_point(l, x, y)){
            if(l->downTexture){
                l->node->texture = l->downTexture;
            }
            if(l->touchDownHandler){
                l->touchDownHandler(e, l);
                break;
            }
        }
    }
    return 0;
}

int jl_init()
{
    //platform_init();
    INIT_LIST_HEAD(&eventlist);
    INIT_LIST_HEAD(&schdlist);
    /* initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS) < 0) {
        printf("Could not initialize SDL\n");
        return 1;
    }
    
    /* seed random number generator */
    srand((unsigned int)time(NULL));
    
    /* create window and renderer */
    window =
    SDL_CreateWindow(NULL, 0, 0, 0, 0,
                     SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL |SDL_WINDOW_FULLSCREEN);
    if (!window) {
        printf("Could not initialize Window\n");
        return 1;
    }
    SDL_GetWindowSize(window, &screen_width, &screen_height);
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        printf("Could not create renderer\n");
        return 1;
    }
    IMG_Init(IMG_INIT_PNG);
    if(TTF_Init() < 0){
        printf("TTF_Init: %s\n", TTF_GetError());
    }
    jldirector_default();
    //register events
    SDL_Event e;
    e.type = SDL_APP_WILLENTERBACKGROUND;
    jl_register_event(&e, jl_quit);
    e.type = SDL_FINGERUP;
    jl_register_event(&e, jl_touch_up);
    e.type = SDL_FINGERDOWN;
    jl_register_event(&e, jl_touch_down);
    e.type = SDL_FINGERMOTION;
    jl_register_event(&e, jl_touch_motion);
    //register end

    return 0;
}

int jl_register_event(SDL_Event *e, event_handler h)
{
    jl_event *je = malloc(sizeof(jl_event));
    if(je == NULL){
        return -1;
    }
    memset(je, 0, sizeof(jl_event));
    je->e.type  = e->type;
    je->handler = h;
    INIT_LIST_HEAD(&je->list);
    list_add_tail(&je->list, &eventlist);
    return 0;
}

int jl_unregister_event(SDL_Event *e, event_handler h)
{
    struct list_head *pos;
    struct list_head *next;
    list_for_each_safe(pos, next, &eventlist){
        jl_event *je = list_entry(pos, jl_event, list);
        if(je->e.type == e->type && je->handler == h){
            //find
            list_del(pos);
            free(je);
            return 0;
        }
    }
    return -1;
}

SDL_Rect SDL_RectMake(int a, int b, int c, int d)
{
    SDL_Rect r;
    r.x = a;
    r.y=b;
    r.w = c;
    r.h = d;
    return r;
}
SDL_Color SDL_ColorMake(int r, int g, int b, int a)
{
    SDL_Color ret;
    ret.r = r;
    ret.g = g;
    ret.b = b;
    ret.a = a;
    return ret;
}

Uint32 jl_map_color(SDL_Color *color)
{
    Uint32 ret = 0;
    ret = color->a << 24;
    ret |= color->r <<16 ;
    ret |= color->g << 8;
    ret |= color->b;
    return ret;
}

int jl_rect_contain_point(SDL_Rect rect, int x, int y)
{
    if(x > rect.x && x < rect.x+rect.w  && y > rect.y && y < rect.y +rect.h){
        return 1;
    }
    return 0;
}