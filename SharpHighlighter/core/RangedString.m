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
    [self ensureRange:range];
    _range = range;
  }
  return self;
}



- (RangedString*)newWithRange: (NSRange)range {
  return [[RangedString alloc] initWithString:_backend andRange:range];
}



- (RangedString*)newWithBegin: (NSUInteger)begin andEnd: (NSUInteger)end {
  NSRange newRange = NSMakeRange(begin, end - begin);
  return [[RangedString alloc] initWithString:_backend andRange:newRange];
}



- (RangedString*)newWithBegin:(NSUInteger)start {
  [self ensureWithinCurrentRange:start];
  NSRange newRange = NSMakeRange(start, _range.location+_range.length-start);
  
  return [[RangedString alloc] initWithString:_backend andRange:newRange];
}



- (RangedString*)newWithEnd: (NSUInteger)end {
  [self ensureWithinCurrentRange:end];
  NSRange newRange = NSMakeRange(_range.location, end - _range.location);
  
  return [[RangedString alloc] initWithString:_backend andRange:newRange];
}



- (NSTextCheckingResult*)findFirstMatch: (RegularExpressionWrapper*) pattern {
  return [pattern firstMatchInString:_backend range:_range];
}



- (NSArray*)findMatches: (RegularExpressionWrapper*) pattern {
  return [pattern matchText:_backend inRange:_range];
}



- (NSString*)toNSString {
  return [_backend substringWithRange:_range];
}



- (NSRange)toNSRange {
  return _range;
}



- (NSString*)description {
  return [self toNSString];
}



- (NSString*)debugDescription {
  return [NSString stringWithFormat:@"[%lu,%lu]%@", (unsigned long)_range.location, _range.length, [self toNSString]];
}



- (NSUInteger)location {
  return _range.location;
}



- (NSUInteger)length {
  return _range.length;
}



- (NSUInteger)endLocation {
  return _range.location + _range.length;
}



- (NSRange)range {
  return _range;
}



- (NSString*)rangedString {
  return [_backend substringWithRange:_range];
}



- (NSString*)string {
  return _backend;
}



- (void)ensureRange:(NSRange)range {
  if (range.location + range.length > _backend.length) {
    @throw [NSException exceptionWithName:NSRangeException reason:@"out of range" userInfo:NULL];
  }
}


- (void)ensureWithinCurrentRange:(NSUInteger)point {
  if (point < _range.location || point >= _range.location+_range.length) {
    @throw [NSException exceptionWithName:NSRangeException reason:@"out of range" userInfo:NULL];
  }
}

@end
