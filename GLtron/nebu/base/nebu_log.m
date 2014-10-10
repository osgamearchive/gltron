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

void nebu_Log(const char* format, ...)
{
    char buf[4096];
    va_list ap;
    va_start(ap, format);
    vsprintf(buf, format, ap);
    va_end(ap);
    NSLog(@"NSLog: %s", buf);
}


