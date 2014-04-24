//
//  jlscene.h
//  jlsdl
//
//  Created by dp on 4/11/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#ifndef jlsdl_jlscene_h
#define jlsdl_jlscene_h
#include "jlsdl.h"

//jlscene

//绘制场景
int jlscene_render(jlscene *s, unsigned int ticks);
//创建场景
jlscene* jlscene_create();
//销毁场景
int jlscene_destroy(jlscene *s);

//添加层
int jlscene_add_layer(jlscene *scene, jllayer *layer);
//根据index添加层
int jlscene_add_layer_at_index(jlscene *scene, jllayer *layer, int index);

//删除层
int jlscene_del_layer(jlscene *scene, jllayer *layer);
//根据index删除层
int jlscene_del_layer_at_index(jlscene *scene, jllayer *layer, int index);
//删除最上面的层
int jlscene_del_top_layer(jlscene *scene);

//根据index获得层
jllayer* jlscene_get_layer_at_index(jlscene *scene, int index);
//获得最上面的层
jllayer* jlscene_get_top_layer(jlscene *scene);

//end


#endif
