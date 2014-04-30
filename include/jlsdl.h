//
//  jlsdl.h
//  jlsdl
//
//  Created by dp on 4/9/14.
//  Copyright (c) 2014 dp. All rights reserved.
//
#ifndef  JLSDL
#define JLSDL
#include <stdio.h>
#include "SDL.h"
#include <time.h>
#include "list.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef IOS
#include "ios.h"
#endif
#define JLDELAY 16

//基本概念
//director管理scene
//scene为场景，有多个layer组成,render的时候依次将这多个layer绘制
//layer，处理点击事件，包含一个node，所有添加到该层的node都是此node的子节点
//node，节点，用于显示到屏幕上的结构， 支持动画，图片，文字，色块等，

//回调函数类型
typedef int (*event_handler)(SDL_Event *e);

//注册事件
typedef struct jl_event{
    SDL_Event e;//事件类型:type
    event_handler handler;//回调函数
    struct list_head list;//列表
} jl_event;

//调度函数类型
typedef int (*jlschd_func)();

typedef struct jlschd{
    jlschd_func func;//调度函数
    int interval;//调度间隔, 毫秒ms
    unsigned int lastCalledTicket;//上次调用回调函数时间
    struct list_head list;//链接所有调度函数的列表
}jlschd;

//节点结构
typedef struct jlnode{
    SDL_Rect frame;//位置，大小
    double angle;//旋转角度
    struct list_head childhead;//子节点链表头
    struct list_head nlist;//链表项
    struct list_head animhead;//正在运行的动画链表头
    struct list_head sanimhead;//该node支持的各种动画(比如打击，爆炸，死亡，发射，行走等)
    struct jlnode *supernode;//父节点
    SDL_Color backgroundColor;//背景色
    SDL_Texture *texture;//texture
    int shouldDestroyTexture;//0, 不销毁texture,否则销毁; 一般来说，从jlplistfile还有手工设置texture的，不需要销毁，从文件创建或色块，则需要销毁
    int tag;//tag
    int ishidden; //是否隐藏，隐藏就不render
    
    struct list_head list;//自定义链表项
} jlnode;
//回调函数类型
typedef int (*jlcallback)(jlnode* n);
typedef struct jlplistfile{
    struct list_head filelist;//将所有打开的plist文件链表缓存起来。将来打开先从链表里找。找不到再打开新的。
    struct list_head childhead;
    char *filename;
    char *texturefile;
    SDL_Surface *surface;//整张图片的surface
}jlplistfile;

typedef struct jlplist{
    struct list_head list;//链表项
    struct list_head childhead;//子项链表头
    struct jlplist *parent;//指向parent的指针
    jlplistfile *file;
    char *key;//<XXX>DDD</XXX>, key=XXX
    char *value;//<XXX>DDD</XXX>, value=DDD
    SDL_Rect rect;
    SDL_Rect dstrect;
    SDL_Point size;
    SDL_Texture *texture;//plist中某一张图片的texture
} jlplist;
//多帧动画，为一个链表，每个链表项里有一个texture
typedef struct jlmovie{
    struct list_head list;
    SDL_Texture *texture;
}jlmovie;

//动画主结构，添加到jlnode
typedef struct jlanim{
    struct list_head list;//正在运行的动画，添加到node的链表里
    struct list_head slist;//支持的动画列表，可以通过index或名字，让node运行该动画（将此动画的list添加到node的正在运行的列表中)
    int duration;//ms 耗时，毫秒
    int type;//enum JLANIM_XXX 类型
    int repeate;//是否重复
    int rest;//剩余时间
    unsigned int lastTick;
    char *name;//命名，可以通过名字查到
    
    //move animation
    double cx, cy;//current point
    SDL_Point srcpoint, dstpoint;//source，dst point;
    double dx, dy;//每次移动多少像素
    
    //rotate
    float angle;//旋转的角度，根据耗时 递增
    float anglePerFrame;
    //movie
    struct list_head moviehead;//多帧链表头
    int movienum;//帧数
    struct list_head *moviecur;
    
    //scroll
    SDL_Rect curRect;//滚动到当前rect
    int v;//x pixels every frame
    int direction;//direction, 0 for v, 1 for h
    int totalp;//total pixels to scroll
    
    jlcallback callback;
}jlanim;

//场景
typedef struct jlscene{
    struct list_head layerhead;//layer 链表头
    struct list_head list;//scene列表
} jlscene;

/*jlsdl.c*/


extern SDL_Window *window;//主窗口
extern SDL_Renderer *renderer;//

extern int screen_width, screen_height;//屏幕高，宽

//注册事件及其回调函数
int jl_register_event(SDL_Event *e, event_handler h);

//删除注册的事件及回调函数
int jl_unregister_event(SDL_Event *e, event_handler h);

//初始化
int jl_init();
//主循环
int jl_loop();

//创建一个jlschd结构
jlschd* jl_schedule_create();
//添加调度函数
int jl_schedule_add(jlschd *schd);
//删除调度函数
int jl_schedule_del(jlschd *schd);

//生成sdl_rect
SDL_Rect SDL_RectMake(int a, int b, int c, int d);
//生成sdl_color
SDL_Color SDL_ColorMake(int r, int g, int b, int a);
//从sdl_color变到32位int
Uint32 jl_map_color(SDL_Color *color);

//sdl_rect里是否包含点（x，y）
int jl_rect_contain_point(SDL_Rect rect, int x, int y);

/*jlsdl end*/
#include "jlplist.h"
#include "jlnode.h"
#include "jllayer.h"
#include "jlscene.h"
#include "jldirector.h"


#endif