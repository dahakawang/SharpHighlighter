//
//  SHLFantomObject.m
//  SharpHighlighter
//
//  Created by david on 3/14/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "SHLFantomObject.h"

@implementation SHLFantomObject {
  __weak id _object;
}

- (instancetype)initWithObject:(id)object {
  if ([super init] == nil) return nil;
  
  _object = object;
  return self;
}

- (id)getObject {
  return _object;
}

- (NSMethodSignature*)methodSignatureForSelector:(SEL)aSelector {
  if (_object == nil) {
    return [super methodSignatureForSelector:@selector(dummy:)];
  } else {
    return [_object methodSignatureForSelector:aSelector];
  }
}

- (void)forwardInvocation:(NSInvocation *)anInvocation {
  [anInvocation invokeWithTarget:_object];
}

- (BOOL)isNil {
  return _object == nil;
}

- (id)dummy: (int)i {
  return nil;
}

@end
