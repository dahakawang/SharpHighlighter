//
//  Grammar.h
//  Pods
//
//  Created by david on 3/14/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SHLRegex.h"

@interface SHLScope : NSObject
@property (nonatomic) NSString* name;
@property (nonatomic) NSMutableArray* patterns;
@property (nonatomic) SHLRegex* terminators;
@property (nonatomic) BOOL compiled;

@end

@interface SHLGrammar : SHLScope
@property (nonatomic) NSString* title;
@property (nonatomic) NSMutableArray* extensions;
@property (nonatomic) SHLRegex* foldingStart;
@property (nonatomic) SHLRegex* foldingStop;
@property (nonatomic) NSDictionary* repository;

@end

@interface SHLGrammarRule : SHLScope
@property (nonatomic) NSString* contentName;
@property (nonatomic) SHLRegex* match;
@property (nonatomic) NSMutableArray* captures;
@property (nonatomic) SHLRegex* begin;
@property (nonatomic) NSMutableArray* beginCaptures;
@property (nonatomic) SHLRegex* end;
@property (nonatomic) NSMutableArray* endCaptures;

@end