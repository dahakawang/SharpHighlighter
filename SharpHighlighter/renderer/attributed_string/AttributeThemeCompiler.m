//
//  AttributeTranslator.m
//  TMHighlighter
//
//  Created by david on 1/11/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AttributeThemeCompiler.h"

static NSMutableArray* translators;

@implementation AttributeThemeCompiler

+ (void)initialize {
  translators = [[NSMutableArray alloc] init];
  [translators addObject:[[ColorTranslator alloc] init]];
  [translators addObject:[[BackgroundTranslator alloc] init]];
  [translators addObject:[[FontTranslator alloc] init]];
}

- (NSDictionary*)getStyleFromDefinition: (NSDictionary*)definition {
  NSMutableDictionary* style = [[NSMutableDictionary alloc] init];
  
  for (Translator* translator in translators) {
    [translator addDefinition:definition ToStyle:style];
  }
  
  return style;
}

- (NSDictionary*)compileTheme:(NSDictionary *)theme {
  NSMutableDictionary* style = [[NSMutableDictionary alloc] initWithCapacity:[theme count]];
  
  for (NSString* key in theme) {
    style[key] = [self getStyleFromDefinition:theme[key]];
  }
  
  return style;
}

@end




@implementation Translator

- (BOOL)addDefinition: (NSString*)definition ToStyle: (NSMutableDictionary*)style {
  @throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"Not implemented" userInfo:NULL];
}

- (float)parseHexFromString: (NSString*)string {
  NSScanner *scanner = [NSScanner scannerWithString:string];
  
  unsigned int result;
  [scanner scanHexInt:&result];
  return result;
}

- (id)translateColorFromString: (NSString*)def {
  float r, g, b;
  NSString* definition = [def lowercaseString];
  r = [self parseHexFromString:[definition substringWithRange:NSMakeRange(1, 2)]] / 255;
  g = [self parseHexFromString:[definition substringWithRange:NSMakeRange(3, 2)]] / 255;
  b = [self parseHexFromString:[definition substringWithRange:NSMakeRange(5, 2)]] / 255;
  
#if TARGET_OS_MAC
  NSColor* color =  [NSColor colorWithRed:r green:g blue:b alpha:1];
  return color;
#elif TARGET_OS_IPHONE
  //TODO add ios implementation
  @throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"Not implemented" userInfo:NULL];
#endif
}
@end




@implementation ColorTranslator
- (BOOL)addDefinition: (NSDictionary*)definition ToStyle: (NSMutableDictionary*)style {
  NSString* color = definition[@"color"];
  if (!color) return NO;
  
  style[NSForegroundColorAttributeName] = [self translateColorFromString:color];
  return YES;
}
@end




@implementation BackgroundTranslator : Translator
- (BOOL)addDefinition: (NSDictionary*)definition ToStyle: (NSMutableDictionary*)style {
  NSString* color = definition[@"background"];
  if (!color) return NO;
  
  style[NSBackgroundColorAttributeName] = [self translateColorFromString:color];
  return YES;
}
@end




@implementation FontTranslator : Translator
- (BOOL)addDefinition:(NSDictionary *)definitions ToStyle:(NSMutableDictionary *)style {
  
//  style[NSFontAttributeName] = nil;
  return NO;
}
@end