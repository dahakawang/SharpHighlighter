//
//  Theme.m
//  TMHighlighter
//
//  Created by david on 1/7/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "Theme.h"
#import "ThemeParser.h"
#import "SelectorMatcher.h"

@implementation Theme{
  NSDictionary* _theme;
  SelectorMatcher* _matcher;
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

- (NSDictionary*)attributesForSelector: (NSString*) selector {
  if (_matcher == nil) {
    _matcher = [[SelectorMatcher alloc] initWithDictionary:_theme];
  }
  
  return [_matcher matchStyleFromClass:selector];
}

- (NSDictionary*)parseThemeFromString: (NSString*)themeStr {
  NSDictionary* styles = [ThemeParser parseThemeData:themeStr];
  
  return styles;
}

- (void)compileWithCompiler: (ThemeCompiler*)compiler {
  if (compiler) {
    _theme = [compiler compileTheme:_theme];
    _matcher = [[SelectorMatcher alloc] initWithDictionary:_theme]; //The matcher will be refreshed
  }
}

- (NSString*)description {
  return [_theme description];
}

- (NSString*)debugDescription {
  return [_theme debugDescription];
}

@end
