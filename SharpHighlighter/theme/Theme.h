//
//  Theme.h
//  TMHighlighter
//
//  Created by david on 1/7/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ThemeCompiler.h"

@interface Theme : NSObject
- (instancetype)initWithString: (NSString*) aString;
- (instancetype)initWithFile: (NSString*) filePath;
- (NSDictionary*)attributesForSelector: (NSString*) selector;
- (void)compileWithCompiler: (ThemeCompiler*)compiler;
@end
