//
//  RegularExpressionWrapper.h
//  TMHighlighter
//
//  Created by david on 1/5/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface RegularExpressionWrapper : NSObject
- (instancetype)initWithPattern: (NSString*) aPattern caseInsensitive: (BOOL) caseInsensitive isGlobal: (BOOL)isGlobal;
- (NSArray*)matchText: (NSString*)aText inRange: (NSRange)range;
- (BOOL)isMatchingText: (NSString*)aText;
@end
