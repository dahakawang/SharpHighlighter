//
//  SHLFantomObject.h
//  SharpHighlighter
//
//  Created by david on 3/14/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SHLFantomObject : NSObject
@property (readonly, nonatomic, getter=getObject) id object;
- (instancetype)initWithObject: (id)object;
- (BOOL)isNil;
@end
