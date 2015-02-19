//
//  RangedString.m
//  SharpHighlighter
//
//  Created by david on 2/19/15.
//
//

#import "RangedString.h"
#import "RegularExpressionWrapper.h"

@implementation RangedString {
  NSString* _backend;
  NSRange _range;
}

- (instancetype)initWithString:(NSString*)str {
  if (self=[super init]) {
    _backend = str;
    _range = NSMakeRange(0, [str length]);
  }
  return self;
}

- (instancetype)initWithString:(NSString*)str andRange:(NSRange)range {
  if (self=[super init]) {
    _backend = str;
    _range = range;
  }
  return self;
}

- (NSRange)convertToInternal:(NSRange)external {
  NSRange internal = NSMakeRange(_range.location + external.location, external.length);
  
  if (internal.location+internal.length > _range.location+_range.length) {
    @throw NSRangeException;
  }
  return internal;
}

- (NSRange)convertToExternal:(NSRange)internal {
  NSRange external = NSMakeRange(internal.location-_range.location, internal.length);
  
  NSAssert(internal.location >= _range.location && internal.location+internal.length<=_range.location+_range.length, @"invalid internal range");
  return external;
}

- (RangedString*)substringWithRange: (NSRange)range {
  NSRange newRange = [self convertToInternal:range];
  
  return [[RangedString alloc] initWithString:_backend andRange:newRange];
}

- (RangedString*)substringFrom:(NSUInteger)start {
  NSRange newRange = [self convertToInternal:NSMakeRange(start, _range.length-start)];
  
  return [[RangedString alloc] initWithString:_backend andRange:newRange];
}

- (RangedString*)substringTo: (NSUInteger)end {
  NSRange newRange = [self convertToInternal:NSMakeRange(0, end+1)];
  
  return [[RangedString alloc] initWithString:_backend andRange:newRange];
}


- (BOOL)findFirstMatch: (RegularExpressionWrapper*) pattern  result: (NSRange*)range {
  NSTextCheckingResult* result = [pattern firstMatchInString:_backend range:_range];
  if (result == nil) {
    return NO;
  } else {
    *range = [self convertToExternal:result.range];
    return YES;
  }
}

- (BOOL)findFirstMatch: (RegularExpressionWrapper*) pattern from: (NSUInteger)start  result: (NSRange*)range {
  NSRange newRange = [self convertToInternal:NSMakeRange(_range.location + start, _range.length - start)];
  
  NSTextCheckingResult* result = [pattern firstMatchInString:_backend range:newRange];
  if (result == nil) {
    return NO;
  } else {
    *range = [self convertToExternal:result.range];
    return YES;
  }
}

- (NSRange)translateToGlobalRange: (NSRange)range {
  return [self convertToInternal:range];
}

- (NSUInteger)length {
  return _range.length;
}

- (NSString*)description {
  return [self toNSString];
}

- (NSString*)debugDescription {
  return [NSString stringWithFormat:@"[%lu,%lu]%@", (unsigned long)_range.location, _range.length, [self toNSString]];
}

- (NSString*)toNSString {
  return [_backend substringWithRange:_range];
}

@end
