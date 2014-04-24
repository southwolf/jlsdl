//
//  jldirector.h
//  jlsdl
//
//  Created by dp on 4/11/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#ifndef jlsdl_jldirector_h
#define jlsdl_jldirector_h

#include "jlsdl.h"

//jldirector
//导演？ 单例模式
typedef struct jldirector{
    struct list_head scenehead;//scene链表头
}jldirector;
//创建和销毁director
jldirector* jldirector_default();
int jldirector_destroy();

//push场景
int jldirector_push_scene(jlscene *s, int animation_type);
//弹出场景
jlscene*  jldirector_pop_scene(int animation_type);
//弹出到某个场景
int jldirector_pop_to_scene(jlscene *s, int animation_type);
//返回当前场景
jlscene* jldirector_current_scene();
//覆盖场景
int  jldirector_replace_scene(jlscene *s, int animatino_type);
//end


#endif
