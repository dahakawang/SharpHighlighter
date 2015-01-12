//
//  RegularExpressionWrapper.m
//  TMHighlighter
//
//  Created by david on 1/5/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "RegularExpressionWrapper.h"

@implementation RegularExpressionWrapper {
  BOOL _isGlobal;
  BOOL _isCaseInsensitive;
  NSString* _pattern;
  NSRegularExpression* _regex;
}

- (NSString*)fixJSRegex: (NSString*) aPattern {
  NSMutableString* ret = [NSMutableString stringWithString:aPattern];
  [ret replaceOccurrencesOfString:@"{" withString:@"\\{" options:0 range:NSMakeRange(0, [ret length])];
  
  return ret;
}

- (instancetype)initWithPattern: (NSString*) aPattern caseInsensitive: (BOOL) caseInsensitive isGlobal: (BOOL)isGlobal {
  if (self = [super init]) {
    _isGlobal = isGlobal;
    _isCaseInsensitive = caseInsensitive;
    _pattern = aPattern;
    
    NSRegularExpressionOptions options = 0;
    if (caseInsensitive) options |= NSRegularExpressionCaseInsensitive;
    aPattern = [self fixJSRegex:aPattern];
    
    _regex = [[NSRegularExpression alloc] initWithPattern:aPattern options:options error:0];
    if (!_regex) @throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"Cannot compile pattern" userInfo:NULL];
  }
  
  return self;
}

- (NSArray*)matchText: (NSString*) aText inRange: (NSRange)range{
  return [_regex matchesInString:aText options:0 range:range];
}

- (NSString*)toString {
  NSString* format = [NSString stringWithFormat:@"/%@/m%@%@", _pattern, _isCaseInsensitive? @"i" : @"", _isGlobal? @"g" : @""];
  return format;
}

- (NSString*)description {
  return [self toString];
}

- (NSString*)debugDescription {
  return [self toString];
}
@end
