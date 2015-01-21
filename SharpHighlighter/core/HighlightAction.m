//
//  HighlightAction.m
//  TMHighlighter
//
//  Created by david on 1/13/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "HighlightAction.h"

@implementation HighlightAction
- (instancetype)initWithRange: (NSRange)range andclassName: (NSString*)className{
  if (self = [super init]) {
    _className = className;
    _range = range;
  }
  return self;
}
@end