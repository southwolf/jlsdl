//
//  ios.m
//  KidsDrive
//
//  Created by dp on 4/9/14.
//  Copyright (c) 2014 dp. All rights reserved.
//

#include "jlsdl.h"
#include "SDL_syswm.h"
#import <UIKit/UIKit.h>

int platform_ios_init()
{
    //InitGameCenter();
    return 0;
}

UIViewController * get_ios_viewcontroller()
{
    SDL_SysWMinfo systemWindowInfo;
    SDL_VERSION(&systemWindowInfo.version);
    if ( ! SDL_GetWindowWMInfo(window, &systemWindowInfo)) {
        // consider doing some kind of error handling here
        return nil;
    }
    UIWindow * appWindow = systemWindowInfo.info.uikit.window;
    UIViewController * rootViewController = appWindow.rootViewController;
    return rootViewController;
}