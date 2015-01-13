//
//  ShlException.m
//  TMHighlighter
//
//  Created by david on 1/13/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "ShlException.h"

@implementation ShlException
- (instancetype)initWithReason:(NSString *)aReason userInfo:(NSDictionary *)aUserInfo {
  return [super initWithName:@"SHLException" reason:aReason userInfo:aUserInfo];
}

+ (instancetype)exeptionWithReason:(NSString *)aReason userInfo:(NSDictionary *)aUserInfo {
  return [[self alloc] initWithReason:aReason userInfo:aUserInfo];
}
@end
