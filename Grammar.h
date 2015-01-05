//
//  Grammar.h
//  TMHighlighter
//
//  Created by david on 1/3/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Grammar : NSObject
- (instancetype)initWithJsonFile: (NSString*) aPath;
- (instancetype)initWithJsonString: (NSString*) aJsonString;

- (NSMutableDictionary*)getTopLevelMode;
@end
