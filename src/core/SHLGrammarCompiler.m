//
//  GrammarCompiler.m
//  SharpHighlighter
//
//  Created by david on 3/14/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "SHLGrammarCompiler.h"


#define SHL_SCOPE_NAME_KEY @"scopeName"
#define SHL_FILE_TYPES_KEY @"fileTypes"
#define SHL_FOLD_START_KEY @"foldingStartMarker"
#define SHL_FOLD_STOP_KEY @"foldingStopMarker"
#define SHL_PATTERNS_KEY @"patterns"
#define SHL_REPOSITORY_KEY @"repository"
#define SHL_NAME_KEY @"name"
#define SHL_MATCH_KEY @"match"
#define SHL_BEGIN_KEY @"begin"
#define SHL_END_KEY @"end"
#define SHL_CONTENT_NAME_KEY @"contentName"
#define SHL_CAPTURE_KEY @"captures"
#define SHL_BEGIN_CAPTURE_KEY @"beginCaptures"
#define SHL_END_CAPTURE_KEY @"endCaptures"
#define SHL_INCLUDE_KEY @"include"


@implementation SHLGrammarCompiler

- (SHLGrammar*)compile:(NSString *)jsonString error:(NSError *__autoreleasing *)error {
  NSAssert(jsonString != nil, @"the json string should not be null");
  NSError* underlyError;
  NSDictionary* jsonGrammar;
  
  
  if (![jsonString canBeConvertedToEncoding:NSUTF16LittleEndianStringEncoding]) {
    [self storeError:error code: SHLGRAMMARCOMPILER_INVALID_JSON msg:@"the json can't be encoded in UTF16"];
    return nil;
  }
  
  
  NSData* jsonData = [jsonString dataUsingEncoding: NSUTF16LittleEndianStringEncoding];
  jsonGrammar = [NSJSONSerialization JSONObjectWithData:jsonData options:0 error:&underlyError];
  if (jsonGrammar == nil) {
    [self storeError:error code:SHLGRAMMARCOMPILER_INVALID_JSON msg:@"the json can't be properly parsed" cause:underlyError];
    return nil;
  }
  
  
  return [self compileTopGrammar:jsonGrammar error: error];
}

- (SHLGrammar*)compileTopGrammar: (NSDictionary*)grammar error:(NSError**)error {
  return nil;
}

- (void)storeError: (NSError**)error code: (NSInteger)code msg: (NSString*)msg cause: (NSError*)cause {
  if (error == nil) return;
  
  NSDictionary* userInfo = @{NSLocalizedDescriptionKey: msg, NSUnderlyingErrorKey: cause};
  *error = [NSError errorWithDomain:SHLGRAMMARCOMPILER_DOMAIN code:code userInfo:userInfo];
}

- (void)storeError: (NSError**)error code: (NSInteger)code msg: (NSString*)msg {
  if (error == nil) return;
  
  NSDictionary* userInfo = @{NSLocalizedDescriptionKey: msg};
  *error = [NSError errorWithDomain:SHLGRAMMARCOMPILER_DOMAIN code:code userInfo:userInfo];
}
@end
