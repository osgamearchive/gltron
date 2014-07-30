//
//  findpath.m
//  GLtron
//
//  Created by Andreas Umbach on 28.07.2014.
//  Copyright (c) 2014 Andreas Umbach. All rights reserved.
//

#include "Cocoa/Cocoa.h"

void goto_installpath(const char *argv0)
{
    NSString *resourceDir = [NSString stringWithFormat:@"%@/Resources", [[NSBundle mainBundle] resourcePath]];
    NSLog(@"%@", resourceDir);
    chdir([resourceDir cStringUsingEncoding:NSUTF8StringEncoding]);
}