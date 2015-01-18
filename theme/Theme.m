//
//  Theme.m
//  TMHighlighter
//
//  Created by david on 1/7/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "Theme.h"
#import "ThemeParser.h"
#import "AttributeTranslator.h"

@implementation Theme{
  NSDictionary* _theme;
}

- (instancetype)initWithString: (NSString*) aString {
  if (self = [super init]) {
    _theme = [self parseThemeFromString:aString];
  }
  return self;
}

- (instancetype)initWithFile: (NSString*) filePath {
  NSString* data = [NSString stringWithContentsOfFile:filePath encoding:NSUTF8StringEncoding error:NULL];
  if (data == nil) @throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"Cannot load theme data file" userInfo:NULL];
  
  return [self initWithString:data];
}

- (NSDictionary*)attributesForClass: (NSString*) className {
  return _theme[className];
}

- (NSDictionary*)parseThemeFromString: (NSString*)themeStr {
  NSDictionary* styles = [ThemeParser parseThemeData:themeStr];
  NSMutableDictionary* translatedStyles = [[NSMutableDictionary alloc] init];
  AttributeTranslator* translator = [AttributeTranslator sharedTranslator];
  
  for (NSString* selector in styles) {
    translatedStyles[selector] = [translator getStyleFromDefinition:styles[selector]];
  }
  return translatedStyles;
}

- (NSString*)description {
  return [_theme description];
}

- (NSString*)debugDescription {
  return [_theme debugDescription];
}

@end
