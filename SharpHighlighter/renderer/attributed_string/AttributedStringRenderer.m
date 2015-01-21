//
//  AttributedStringRenderer.m
//  TMHighlighter
//
//  Created by david on 1/18/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "AttributedStringRenderer.h"
#import "AttributeThemeCompiler.h"

@implementation AttributedStringRenderer
- (id)renderString:(NSString *)sourceCode withTheme:(Theme *)theme action:(NSArray *)actions {
  AttributeThemeCompiler* themeCompiler = [[AttributeThemeCompiler alloc] init];
  [theme compileWithCompiler:themeCompiler];
  
  NSMutableAttributedString* highlightedStr = [[NSMutableAttributedString alloc] initWithString:sourceCode];
  for (HighlightAction* action in actions) {
    NSDictionary* property = [theme attributesForSelector:action.className];
    if (property) {
      [highlightedStr setAttributes:property range:action.range];
    }
  }

  
  return highlightedStr;
}
@end
