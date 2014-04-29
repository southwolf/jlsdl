//
//  jlplist.c
//  jlsdl
//
//  Created by dp on 4/11/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#include "jlplist.h"
static struct list_head openfiles = LIST_HEAD_INIT(openfiles);

int jlplistfile_destroy(jlplistfile *p)
{
    if(!p)
        return -1;
    list_del(&p->filelist);
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, &p->childhead){
        jlplist* tmp = list_entry(pos, jlplist, list);
        jlplist_destroy(tmp);
    }
    if(p->surface){
        SDL_FreeSurface(p->surface);
    }
    if(p->filename)
        free(p->filename);
    if(p->texturefile)
        free(p->texturefile);
    return 0;
}
int jlplist_destroy(jlplist *p)
{
    if(!p)
        return -1;
    list_del(&p->list);
    struct list_head *pos, *n;
    list_for_each_safe(pos, n, &p->childhead){
        jlplist* tmp = list_entry(pos, jlplist, list);
        jlplist_destroy(tmp);
    }
    if(p->texture){
        SDL_DestroyTexture(p->texture);
    }
    if(p->key)
        free(p->key);
    if(p->value)
        free(p->value);
    free(p);
    return 0;
}
jlplist* jlplist_create()
{
    jlplist *ret = malloc(sizeof(jlplist));
    if(ret){
        memset(ret, 0, sizeof(jlplist));
        INIT_LIST_HEAD(&ret->childhead);
        INIT_LIST_HEAD(&ret->list);
    }
    return ret;
}
int jlplist_add_child(jlplist* parent, jlplist *child)
{
    if(!parent || !child){
        return -1;
    }
    child->parent = parent;
    list_add_tail(&child->list, &parent->childhead);
    return 0;
}
jlplistfile* jlplistfile_create()
{
    jlplistfile *ret = malloc(sizeof(jlplistfile));
    if(ret){
        memset(ret, 0, sizeof(jlplistfile));
        INIT_LIST_HEAD(&ret->childhead);
        INIT_LIST_HEAD(&ret->filelist);
    }
    return ret;
}

jlplistfile* jlplistfile_parse(char *buf)
{
    jlplistfile *ret = jlplistfile_create();//create plistfile
    jlplist *root = jlplist_create();//create root plist
    root->file = ret;
    list_add(&root->list, &ret->childhead);
    jlplist *cur = root;
    char *p = strchr(buf, '<');
    while(p){
        if(*(p+1) != '/'){
            //new node
            jlplist *child = jlplist_create();
            child->file = ret;
            jlplist_add_child(cur, child);
            
            //key
            char *pd = strchr(p, '>');
            if(!pd){
                printf("parse error\n");
                goto CLEAN;
            }
            char *v = malloc(pd-p);
            if(v == NULL){
                goto CLEAN;
            }
            memset(v, 0, pd-p);
            strncpy(v, p+1, pd-p-1);
            child->key = v;
            if(*(pd-1) == '/'){
                //<XXX/> style
                p = pd+1;
            }else{
                //value
                p = pd+1;
                
                pd = strchr(p, '<');
                while(p < pd){
                    if(*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n'){
                        p++;
                    }else{
                        break;
                    }
                }
                if(p == pd){
                    if(*(pd+1) == '/'){
                        p = pd + 1;
                    }else{
                        cur = child;
                    }
                }else{
                    v = malloc(pd-p+1);
                    if(v == NULL){
                        goto CLEAN;
                    }
                    memset(v, 0, pd-p+1);
                    strncpy(v, p, pd-p);
                    child->value = v;
                    cur = child;
                }
            }
        }else{
            char *pd = strchr(p, '>');
            if(!pd){
                printf("parse error\n");
                goto CLEAN;
            }
            p = pd+1;
            cur = cur->parent;
        }
        p = strchr(p, '<');
    }
    return ret;
CLEAN:
    jlplistfile_destroy(ret);
    return NULL;
}
jlplistfile* jlplistfile_open(char *plistfile, char *texturefile)
{
    if(!plistfile || !texturefile)
        return NULL;
    struct list_head *pos;
    list_for_each(pos, &openfiles){
        jlplistfile *f = list_entry(pos, jlplistfile, filelist);
        if(strcmp(f->filename, plistfile) == 0){
            return f;
        }
    }
    int fd = open(plistfile, O_RDONLY);
    if(fd < 0){
        printf("open file error:%s\n",plistfile);
        return NULL;
    }
    //read all file into memory
    struct stat st;
    fstat(fd, &st);
    int size = (int)st.st_size;
    char *buf = malloc(size + 1);
    if(!buf){
        close(fd);
        return NULL;
    }
    memset(buf, 0, size+1);
    int rdsize = 0;
    while(rdsize < size){
        int r = read(fd, buf+rdsize, size-rdsize);
        if(r < 0){
            close(fd);
            return NULL;
        }
        rdsize += r;
    }
    
    //parse
    jlplistfile *ret = jlplistfile_parse(buf);
    free(buf);
    close(fd);
    if(ret){
        ret->filename = strdup(plistfile);
        ret->texturefile = strdup(texturefile);
        list_add(&ret->filelist, &openfiles);
        ret->surface = IMG_Load(texturefile);
    }
    return ret;
}
jlplist* jlplist_get_value(jlplist*p, char *key1, char *key2)
{
    jlplist *ret = jlplist_item_for_key(p, key1);
    return jlplist_item_for_key(ret, key2);
}
jlplist* jlplist_item_for_key(jlplist *p, char *key)
{
    if(!p || !key)
        return NULL;
    struct list_head *pos;
    list_for_each(pos, &p->childhead){
        jlplist *tmp = list_entry(pos, jlplist, list);
        if(tmp){
            if(strcmp(tmp->key, "key") == 0 && tmp->value && strcmp(tmp->value, key) == 0){
                return list_entry(pos->next, jlplist, list);
            }
            jlplist *ret = jlplist_item_for_key(tmp, key);
            if(ret){
                return ret;
            }
        }
    }
    return NULL;
}

static SDL_Rect _stringToRect(char *v)
{
    SDL_Rect ret;
    memset(&ret, 0, sizeof(ret));
    if(!v)
        return ret;
    sscanf(v,"{{%d,%d},{%d,%d}}", &ret.x, &ret.y, &ret.w, &ret.h);
    return ret;
}
static SDL_Point _stringToPoint(char* v)
{
    SDL_Point p;
    memset(&p, 0, sizeof(p));
    if(!v)
        return p;
    sscanf(v,"{%d,%d}", &p.x, &p.y);
    return p;
}
jlplist* jlplistfile_get_root(jlplistfile* p)
{
    if(!p)
        return NULL;
    if(p->childhead.next != &p->childhead){
        return list_entry(p->childhead.next, jlplist, list);
    }
    return NULL;
}
jlplist* jlplist_load_texture_for_frame(jlplistfile* file, char*filename)
{
    jlplist *plist = jlplistfile_get_root(file);
    jlplist *frame = jlplist_item_for_key(plist, filename);
    if(frame == NULL){
        printf("cant find filename:%s\n",filename);
        return NULL;
    }
    if(!frame->texture){
        jlplist *p = jlplist_get_value(plist, filename, JLP_FRAME);
        jlplist *s = jlplist_get_value(plist, filename, JLP_SOURCESIZE);
        if(!p || !s){
            return NULL;
        }
        frame->rect = _stringToRect(p->value);
        frame->size = _stringToPoint(s->value);
        frame->dstrect.x = (frame->size.x - frame->rect.w) / 2;
        frame->dstrect.y = (frame->size.y - frame->rect.h) / 2;
        frame->dstrect.w = frame->size.x;
        frame->dstrect.h = frame->size.y;
        SDL_Surface *sur = SDL_CreateRGBSurface(0, frame->size.x, frame->size.y, 32, 0, 0, 0, 255);
        SDL_UpperBlit(file->surface, &frame->rect, sur, &frame->dstrect);
        Uint32 colorkey = SDL_MapRGB( sur->format, 0, 0, 0);
        SDL_SetColorKey(sur, 1, colorkey);
        frame->texture =SDL_CreateTextureFromSurface(renderer, sur);
    }
    return frame;
}
int jlplist_set_node_file_texture(jlplistfile *file, char *filename, jlnode *node)
{
    if(!file || !filename || !node)
        return -1;
    jlplist *frame = jlplist_load_texture_for_frame(file, filename);
    if(frame){
        node->frame.w = frame->size.x;
        node->frame.h = frame->size.y;
        jlnode_set_texture(node, frame->texture);
    }
    return 0;
}

jlanim* jlplist_create_animation(char *plistfile, char *imagefile, int duration)
{
    jlanim *ret = jlanim_create(JLANIM_MOVIE, duration, 0, NULL);
    jlplistfile *plist = jlplistfile_open(plistfile, imagefile);
    if(!plist){
        printf("open file:%s error\n", plistfile);
        return NULL;
    }
    jlplist *arr = jlplist_item_for_key(jlplistfile_get_root(plist), JLP_FRAMES);
    struct list_head *pos;
    if(arr){
        list_for_each(pos, &arr->childhead){
            jlplist *tmp = list_entry(pos, jlplist, list);
            if(tmp->value){
                jlplist *tf = jlplist_load_texture_for_frame(plist, tmp->value);
                if(tf->texture == NULL){
                    printf("texture load error\n");
                }
                jlmovie_add_texture(ret, tf->texture);
            }
        }
    }
    ret->rest = duration / ret->movienum;//set rest
    return ret;
}