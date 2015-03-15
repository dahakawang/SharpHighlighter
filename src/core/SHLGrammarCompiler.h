//
//  GrammarCompiler.h
//  SharpHighlighter
//
//  Created by david on 3/14/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SHLGrammar.h"

#define SHLGRAMMARCOMPILER_DOMAIN @"net.davidvoid.SharpHighlighter.SHLGrammarCompiler"
#define SHLGRAMMARCOMPILER_INVALID_JSON 1

@interface SHLGrammarCompiler : NSObject
- (SHLGrammar*)compile: (NSString*)jsonString error: (NSError**)error;
@end
