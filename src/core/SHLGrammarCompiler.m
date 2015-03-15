//
//  GrammarCompiler.m
//  SharpHighlighter
//
//  Created by david on 3/14/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "SHLGrammarCompiler.h"

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
