//
//  jlplist.h
//  jlsdl
//
//  Created by dp on 4/11/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#ifndef jlsdl_jlplist_h
#define jlsdl_jlplist_h
#include "jlsdl.h"

#define JLP_FRAMES "frames"
#define JLP_SOURCESIZE "sourceSize"
#define JLP_OFFSET "offset"
#define JLP_FRAME "frame"
#define JLP_RORATE "rotated"
#define JLP_SOURCECOLORRECT "sourceColorRect"

//销毁plist文件
int jlplistfile_destroy(jlplistfile *p);
//销毁jlplist
int jlplist_destroy(jlplist *p);
//打开并解析文件,plistfile为plist文件，cocos2d格式（texturepacker,zwoptex等合并图片生成的）,texturefile为合并的图片文件(png格式)
jlplistfile* jlplistfile_open(char *plistfile, char *texturefile);

//查找为key的项
jlplist* jlplist_item_for_key(jlplist *p, char *key);

//查找key1项，并在key1中查找key2的值
jlplist* jlplist_get_value(jlplist*p, char *key1, char *key2);

//返回plist的某个文件的texture
int jlplist_set_node_file_texture(jlplistfile *file, char *filename, jlnode *node);
//创建plist的动画
jlanim* jlplist_create_animation(char *plist, char *imagefile, int duration);

//获得jlplistfile 的根节点
jlplist* jlplistfile_get_root(jlplistfile* p);
#endif
