//
//  jllayer.h
//  jlsdl
//
//  Created by dp on 4/11/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#ifndef jlsdl_jllayer_h
#define jlsdl_jllayer_h
#include "jlsdl.h"
//jllayer
struct jllayer;

typedef int (*layer_event_handler)(SDL_Event *e, struct jllayer *l);

//层，层处理点击事件
typedef struct jllayer{
    jlnode *node;//层的主node，层的子node都添加到该node下面
    int shouldDestroyTextures;//是否需要清除下面俩texture
    SDL_Texture *normalTexture, *downTexture;
    jlscene *scene;
    //touch handler
    layer_event_handler touchUpHandler;//手指抬起回调函数
    layer_event_handler touchMotionHandler;//手指移动回调函数
    layer_event_handler touchDownHandler;//手指点击回调函数
    
    //keyborad handler
    layer_event_handler keyDownHandler;//键盘按下
    layer_event_handler keyUpHandler;//键盘弹起
    
    //mouse handler
    layer_event_handler mouseButtonDownHandler;//鼠标按下
    layer_event_handler mouseButtonUpHandler;//鼠标弹起
    layer_event_handler mouseMotionHandler;//鼠标移动
    struct list_head layerlist;//层列表项，每个scene有多个层，每个层有多个node，每个node可以有多个子node
    struct list_head list;//自定义链表
}jllayer;

//绘制层
int jllayer_render(jllayer* l, unsigned int ticks);

//向层添加node
int jllayer_add_node(jllayer *layer, jlnode* node);
//根据index，向层添加node
int jllayer_add_node_at_index(jllayer *layer, jlnode *node, int index);

//根据tag查找子节点
jlnode* jllayer_get_node_by_tag(jllayer *layer, int tag);

//销毁层及起所有子节点
int jllayer_destroy(jllayer *l);
//创建层
jllayer* jllayer_create();

//检查该层是否包含 点（x，y）
int jllayer_contain_point(jllayer *l, int x, int y);
//end

//设置正常和按下的texture
int jllayer_set_textures_from_surface(jllayer *l,  SDL_Surface *normal, SDL_Surface *down);

//加载文件 设置layer的texture
int jllayer_set_texture_from_file(jllayer* l, char *file);

//从surface 设置layer的texture
int jllayer_set_texture_from_surface(jllayer *l,  SDL_Surface *sur);
//设置改层的位置
int jllayer_set_pos(jllayer *l, int x, int y);
//设置长宽
int jllayer_set_wh(jllayer *l, int w, int h);
jllayer* jllayer_create_button(char *title, SDL_Color color, TTF_Font* font, layer_event_handler handler);
#endif
