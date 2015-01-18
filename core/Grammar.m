//
//  Grammar.m
//  TMHighlighter
//
//  Created by david on 1/3/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "Grammar.h"

@implementation Grammar {
  NSMutableDictionary* _json;
}
- (instancetype)initWithJsonFile:(NSString *)aPath {
  NSData* jsonData = [NSData dataWithContentsOfFile:aPath];
  if (jsonData == nil) {
    @throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"Cannot read grammar definition file" userInfo:NULL];
  }
  
  return [self initWithJsonData:jsonData];
}

- (instancetype)initWithJsonString:(NSString *)aJsonString {
  NSData* data = [aJsonString dataUsingEncoding:NSUTF8StringEncoding];
  return [self initWithJsonData:data];
}

- (instancetype)initWithJsonData: (NSData*) aData {
  if (self = [super init]) {
    NSMutableDictionary* json = [NSJSONSerialization JSONObjectWithData:aData options:NSJSONReadingMutableContainers error:NULL];
    _json = json;
  }
  
  return self;
}

- (NSMutableDictionary*)getTopLevelMode {
  return _json;
}

- (NSString*)description {
  return [_json description];
}

- (NSString*)debugDescription {
  return [_json debugDescription];
}

@end
