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

- (BOOL)shouldEscapeToken: (NSString*)token inString: (NSString*)str range: (NSRange)range {
  int offset = 0;
  unichar end = 0;
  if ([token isEqualToString:@"{"]) {
    offset = 1;
    end = '}';
  } else if ([token isEqualToString:@"}"]) {
    offset = -1;
    end = '{';
  }
  
  long pos = range.location + range.length - 1 + offset;
  while (pos >= 0 && pos < str.length) {
    pos += offset;
    unichar ch = [str characterAtIndex:pos];
    if (ch == end) {
      return NO;
    }
    
    if (!(ch == ',' || (ch >= '0' && ch <= '9'))) {
      return YES;
    }
  }
  
  return YES;
}

- (void)EscapeToken:(NSString *)token str:(NSMutableString *)str {
  NSAssert([token length] == 1, @"Length of token should be 1");
  
  NSString* escapedToken = [NSString stringWithFormat:@"\\%@", token];
  NSString* pattern = [NSString stringWithFormat:@"\\\\*%@", escapedToken];
  NSRegularExpression* regex = [[NSRegularExpression alloc] initWithPattern:pattern options:NSRegularExpressionAnchorsMatchLines error:NULL];
  
  NSUInteger lastIndex = 0;
  NSRange found = NSMakeRange(0, 1);
  
  while (found.length != 0) {
    found = [regex firstMatchInString:str options:0 range:NSMakeRange(lastIndex, str.length - lastIndex)].range;
    lastIndex = found.location + found.length;
    
    if (found.length % 2 == 1) { //even number of \ before {, so this token is not escaped
      if ( [self shouldEscapeToken:token inString:str range:found]) {
        [str replaceOccurrencesOfString:token withString:escapedToken options:0 range:found];
        lastIndex = found.location + found.length + 1;
      }
    }
  }
}

- (NSString*)fixJSRegex: (NSString*) aPattern {
  NSMutableString* str = [NSMutableString stringWithString:aPattern];
  
  [self EscapeToken:@"{" str:str];
  [self EscapeToken:@"}" str:str];
  
  return str;
}

- (instancetype)initWithPattern: (NSString*) aPattern caseInsensitive: (BOOL) caseInsensitive isGlobal: (BOOL)isGlobal {
  if (self = [super init]) {
    _isGlobal = isGlobal;
    _isCaseInsensitive = caseInsensitive;
    _pattern = aPattern;
    
    NSRegularExpressionOptions options = NSRegularExpressionAnchorsMatchLines;
    if (caseInsensitive) options |= NSRegularExpressionCaseInsensitive;
    aPattern = [self fixJSRegex:aPattern];
    
    _regex = [[NSRegularExpression alloc] initWithPattern:aPattern options:options error:NULL];
    if (!_regex) @throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"Cannot compile pattern" userInfo:NULL];
  }
  
  return self;
}

- (NSArray*)matchText: (NSString*) aText inRange: (NSRange)range{
  return [_regex matchesInString:aText options:0 range:range];
}

- (BOOL)isMatchingText: (NSString*)aText {
  NSRange textRange = NSMakeRange(0, [aText length]);
  NSTextCheckingResult* result = [_regex firstMatchInString:aText options:0 range:textRange];
  return (result.range.location == textRange.location) && (result.range.length == textRange.length);
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
