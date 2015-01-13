//
//  ShlException.h
//  TMHighlighter
//
//  Created by david on 1/13/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ShlException : NSException
- (instancetype)initWithReason:(NSString *)aReason userInfo:(NSDictionary *)aUserInfo;
@end
