//
//  ThemeParser.m
//  TMHighlighter
//
//  Created by david on 1/8/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "ThemeParser.h"

NSString* trim(NSString* str) {
  return [str stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
}

@implementation ThemeParser
+ (NSDictionary*)parseThemeData: (NSString*)aString {
  NSMutableDictionary* theme = [[NSMutableDictionary alloc] init];
  NSRegularExpression* regex = [[NSRegularExpression alloc]initWithPattern:@"[\\{\\}]" options:0 error:NULL];
  NSArray* matches = [regex matchesInString:aString options:0 range:NSMakeRange(0, [aString length])];
  
  if ([matches count] % 2 != 0) @throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"The theme definition files is not valid" userInfo:NULL];
  
  unsigned long start = 0;
  for (int i = 0; i < [matches count]; i+=2) {
    unsigned long pos1 = ((NSTextCheckingResult*)matches[i]).range.location;
    unsigned long pos2 = ((NSTextCheckingResult*)matches[i+1]).range.location;
    NSArray* selectors = [self parseSelectors:[aString substringWithRange:NSMakeRange(start, pos1-start)]];
    NSDictionary* styles = [self parseStyles:[aString substringWithRange:NSMakeRange(pos1+1, pos2-pos1-1)]];
    [self addToTheme:theme fromSelectors:selectors andStyles:styles];
    start = pos2 + 1;
  }
  
  return theme;
}

+ (void)addToTheme: (NSMutableDictionary*)theme fromSelectors: (NSArray*)selectors andStyles: (NSDictionary*)styles {
  for (NSString* selector in selectors) {
    if (theme[selector] != nil) NSLog(@"Overwritting styles for selector %@", selector);
    theme[selector] = [[NSDictionary alloc] initWithDictionary:styles];
  }
}

+ (NSArray*)parseSelectors: (NSString*)selectorStr {
  NSMutableArray* selectorArray = [[NSMutableArray alloc]init];
  
  NSArray*selectors = [selectorStr componentsSeparatedByString:@","];
  for (NSString* selector in selectors) {
    [selectorArray addObject:trim(selector)];
  }

  return selectorArray;
}

+ (NSDictionary*)parseStyles: (NSString*)styleStr {
  NSMutableDictionary* styleDictionary = [[NSMutableDictionary alloc] init];
  
  NSArray* styles = [styleStr componentsSeparatedByString:@";"];
  for (NSString* styleDef in styles ) {
    if (![trim(styleDef) isEqualToString:@""]) {
      NSArray* definition = [styleDef componentsSeparatedByString:@":"];
      if ([definition count] != 2) @throw [NSException exceptionWithName:NSInvalidArgumentException reason:[NSString stringWithFormat:@"Cannot parse style definition %@", definition ] userInfo:NULL];
      styleDictionary[trim(definition[0])] = trim(definition[1]);
    }
  }
  
  return styleDictionary;
}
@end
