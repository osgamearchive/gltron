//
//  png_texture.m
//  GLtron
//
//  Created by Andreas Umbach on 23.08.2014.
//  Copyright (c) 2014 Andreas Umbach. All rights reserved.
//

#include "video/nebu_png_texture.h"

#include <stdio.h>
#include <stdlib.h>

#define ERR_PREFIX "[load_png_texture] "

#ifdef __IPHONEOS__
#import <UIKit/UIKit.h>
#else
// #import <CoreImage/CoreImage.h>
#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>
#endif

png_texture* load_png_texture(const char *filename) {

#ifdef __IPHONEOS__
    UIImage* image = [UIImage imageWithContentsOfFile:[NSString stringWithUTF8String:filename]];
    CGImageRef imageRef = [image CGImage];
    size_t width = CGImageGetWidth(imageRef);
    size_t height = CGImageGetHeight(imageRef);
    // NSLog(@"[image] %s has size (%d,%d)\n", filename, width, height);
#else

    CIImage* image = [CIImage imageWithData:[NSData dataWithContentsOfFile:[NSString stringWithUTF8String:filename]]];
    
    size_t width = (int) image.extent.size.width;
    size_t height = (int) image.extent.size.height;
    
    // NSLog(@"[image] %s has size (%d,%d)\n", filename, width, height);
#endif
    
    
    GLubyte* textureData = (GLubyte *)malloc(width * height * 4); // if 4 components per pixel (RGBA)
    memset(textureData, 0, width * height * 4);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    NSUInteger bytesPerPixel = 4;
    NSUInteger bytesPerRow = bytesPerPixel * width;
    NSUInteger bitsPerComponent = 8;
    CGContextRef context = CGBitmapContextCreate(textureData, width, height,
                                                 bitsPerComponent, bytesPerRow, colorSpace,
                                                 kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);
    
    CGContextTranslateCTM(context, 0, height);
    CGContextScaleCTM(context, 1.0f, -1.0f);
    
#ifndef __IPHONEOS__
    CIContext *ciContext = [CIContext contextWithCGContext:context options:nil];
    CGImageRef imageRef = [ciContext createCGImage:image fromRect:image.extent];
#endif
    
    // [image drawInRect:CGRectMake(0, 0, width, height)];
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), imageRef);
    CGContextRelease(context);

    
    png_texture* tex = (png_texture*) malloc(sizeof(png_texture));
    tex->data = textureData;
    tex->width = (int)width;
    tex->height = (int)height;
    tex->channels = 4;
    
    return tex;
}

void unload_png_texture(png_texture *tex) {
    free(tex->data);
    free(tex);
}

#define TARGET(X, Y, I) (mip->data + I + mip->channels * (X + Y * mip->width))
#define SOURCE(X, Y, I) (source->data + I + \
source->channels * (X + Y * source->width))

png_texture* mipmap_png_texture(png_texture *source, int level,
                                int clamp_u, int clamp_v) {
    png_texture *mip;
    int i;
    int x, y;
    int fx, fy;
    
    if(level != 1) return NULL;
    mip = (png_texture*) malloc(sizeof(png_texture));
    mip->channels = source->channels;
    fx = (source->width > 1) ? 2 : 1;
    fy = (source->height > 1) ? 2 : 1;
    
    mip->width = source->width / fx;
    mip->height = source->height / fy;
    mip->data = (unsigned char*) malloc(mip->width * mip->height *
                                        mip->channels);
    
    /* simple linear filtering */
    for(i = 0; i < mip->channels; i++) {
        for(y = 0; y < mip->height; y++) {
            for(x = 0; x < mip->width; x++) {
                /*
                 *( TARGET(x, y, i) ) = ( *( SOURCE(x * fx, y * fy, i) )  +
				 *( SOURCE((x + 1) * fx, (y + 1) * fy, i) ) +
				 *( SOURCE((x + 1) * fx, y * fy, i) ) +
				 *( SOURCE(x * fx, (y + 1) * fy, i) ) ) / 4;
				 */
                int sx, sy, xoff, yoff;
                sx = fx * x;
                sy = fy * y;
                xoff = fx - 1;
                yoff = fy - 1;
                
                *( mip->data + i + mip->channels * ( x + y * mip->width ) ) =
                ( *( source->data + i + source->channels * ( sx + sy * source->width) ) +
                 *( source->data + i + source->channels * ( sx + xoff + sy * source->width) ) +
                 *( source->data + i + source->channels * ( sx + xoff + (sy + yoff) * source->width) ) +
                 *( source->data + i + source->channels * ( sx + (sy + yoff) * source->width) ) ) / 4;
                
            }
        }
    }
    return mip;
}
