//
//  nebu_log.m
//  GLtron
//
//  Created by Andreas Umbach on 28.07.2014.
//  Copyright (c) 2014 Andreas Umbach. All rights reserved.
//

#include "base/nebu_debug_memory.h"

#include "Cocoa/Cocoa.h"

void nebu_Log(const char* message)
{
    NSLog(@"NSLog: %s", message);
}


