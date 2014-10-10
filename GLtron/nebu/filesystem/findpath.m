//
//  findpath.m
//  GLtron
//
//  Created by Andreas Umbach on 28.07.2014.
//  Copyright (c) 2014 Andreas Umbach. All rights reserved.
//

#ifndef __IPHONEOS__
#include "Cocoa/Cocoa.h"
#else
#import "UIKit/UIKit.h"
#endif

void goto_installpath(const char *argv0)
{
    NSString *resourceDir = [NSString stringWithFormat:@"%@/Resources", [[NSBundle mainBundle] resourcePath]];
    NSLog(@"resource dir: %@", resourceDir);
    chdir([resourceDir cStringUsingEncoding:NSUTF8StringEncoding]);
}