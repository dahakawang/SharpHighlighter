//
//  HighlightAction.h
//  TMHighlighter
//
//  Created by david on 1/13/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface HighlightAction : NSObject
@property (readonly, nonatomic) NSRange range;
@property (strong, readonly, nonatomic) NSString* className;

- (instancetype)initWithRange: (NSRange)range andclassName: (NSString*)className;
@end

HighlightAction* MakeAction(NSRange range, NSArray* modeStack, NSString* name);