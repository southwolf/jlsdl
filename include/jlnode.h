//
//  jlnode.h
//  jlsdl
//
//  Created by dp on 4/11/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#ifndef jlsdl_jlnode_h
#define jlsdl_jlnode_h
#include "jlsdl.h"

//jlanim
//动画

//动画类型
enum {
    JLANIM_MOVE = 0,//移动
    JLANIM_ROTATE,//旋转
    JLANIM_FLIP,//翻转，暂不支持
    JLANIM_MOVIE,//播放多帧动画
    JLANIM_SCROLL,//无限滚动
};
//创建动画帧
jlmovie* jlmovie_create(char *file);
jlmovie* jlmovie_create_with_texture(SDL_Texture* text);

//设置动画的名字，可以通过名字查找动画
int jlanim_set_name(jlanim* a, char *name);
//向动画anim里添加texture
int jlmovie_add_texture(jlanim *anim, SDL_Texture *text);
//向动画anim里添加文件
int jlmovie_add_file(jlanim *anim, char *newfile);
//销毁帧
int jlmovie_destroy(jlmovie *movie);

//创建动画，类型，耗时，是否重复(当type为无限滚动时,ms为每帧移动的像素数),data根据类型不同而不同
jlanim* jlanim_create(int type, int ms, int rep, char* data);

//设置移动动画的起点，终点
int jlanim_set_move_point(jlanim* anim,int fx, int fy, int tx, int ty);

//销毁动画
int jlanim_destroy(jlanim *anim);

//获得当前帧动画
SDL_Texture* jlanim_get_movie_curr_texture(jlanim *a);
//根据index获得第n个帧的texture
SDL_Texture *jlanim_get_movie_texture_at_index(jlanim *a, int index);
//获得下一帧的动画
SDL_Texture *jlanim_get_movie_next_texture(jlanim *a);
// jlanimation end

/*jlnode.c*/

//绘制到renderer
int jlnode_render(jlnode *node, unsigned int ticks);
//从node的父节点删除该node
int jlnode_remove_from_super(jlnode *node);

//创建label类型的node
jlnode* jlnode_create_label(char* title, TTF_Font* font, SDL_Color color);
//创建jlnode并初始化
jlnode* jlnode_create();

//销毁jlnode
int jlnode_destroy(jlnode* node);

//添加子node
int jlnode_add_child(jlnode *parent, jlnode *child);

//根据index添加子节点
int jlnode_add_child_at_index(jlnode *parent, jlnode *child, int index);

//通过tag查找子节点
jlnode* jlnode_get_child_by_tag(jlnode *parent, int tag);

//获得第index个子节点
jlnode* jlnode_get_child_at_index(jlnode *parent, int index);

//设置frame
int jlnode_set_frame(jlnode *node, SDL_Rect r);

//设置位置, 以x, y 为中心点(不是起始点)
int jlnode_set_pos(jlnode *node, int x, int y);
//设置长宽
int jlnode_set_wh(jlnode *node, int w, int h);
//创建texture
SDL_Texture * jlnode_create_texture(jlnode *node);
//通过背景色创建texture
int jlnode_set_background_color(jlnode *node, SDL_Color color);

//设置texture
int jlnode_set_texture(jlnode*node, SDL_Texture *texture);
//通过图片文件创建
int jlnode_create_texture_from_file(jlnode *node, char *path);
//通过surface
int jlnode_create_texture_from_surface(jlnode* node, SDL_Surface* sur);

//通过动画名字查找动画
jlanim* jlnode_get_anim_by_name(jlnode* node, char *name);
//运行动画
int jlnode_run_animation(jlnode *node, jlanim *anim);
//删除正在运行的动画
int jlnode_unrun_animation(jlnode *node, jlanim *anim);

//添加动画
int jlnode_add_animation(jlnode *node, jlanim *anim);
//删除动画
int jlnode_del_animation(jlnode *node, jlanim *anim);
/*jlnode end*/

#endif
