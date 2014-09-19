//
//  nebu_log.m
//  GLtron
//
//  Created by Andreas Umbach on 28.07.2014.
//  Copyright (c) 2014 Andreas Umbach. All rights reserved.
//

#include "base/nebu_debug_memory.h"

#ifndef __IPHONEOS__
#include "Cocoa/Cocoa.h"
#else
#include "UIKit/UIKit.h"
#endif

void nebu_Log(const char* message)
{
    NSLog(@"NSLog: %s", message);
}


