//
//  GrammarCompiler.m
//  SharpHighlighter
//
//  Created by david on 3/14/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "SHLGrammarCompiler.h"
#import "SHLFantomObject.h"


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

- (SHLGrammar*)compileTopGrammar: (NSDictionary*)map error:(NSError**)error {
  SHLGrammar* grammar = [[SHLGrammar alloc] init];
  
  
  grammar.name = map[SHL_SCOPE_NAME_KEY];
  grammar.title = map[SHL_NAME_KEY];
  grammar.extensions = map[SHL_FILE_TYPES_KEY];
  grammar.foldingStart = [self compileRegex: map[SHL_FOLD_START_KEY] error:error];
  if (error != nil) return nil;
  grammar.foldingStart = [self compileRegex:map[SHL_FOLD_STOP_KEY] error:error];
  if (error != nil) return nil;
  
  
  grammar.repository = [self compileRepository: map error:error];
  if (error != nil) return nil;
  
  
  NSMutableArray* subPatterns = [[NSMutableArray alloc] init];
  NSDictionary* subRules = map[SHL_PATTERNS_KEY];
  for (NSDictionary* subRule in subRules) {
    [subPatterns addObject: [self compileRule:subRule error:error]];
  }
  grammar.patterns = subPatterns;
  
  
  for (SHLScope* scope in grammar.repository) {
    [self processScope:scope grammar:grammar error:error];
    if (error != nil) return nil;
  }
  [self processScope:grammar grammar:grammar error:error];
  
  return grammar;
}

- (NSDictionary*)compileRepository: (NSDictionary*)map error: (NSError**)error {
  NSMutableDictionary* value = [[NSMutableDictionary alloc] init];
  
  NSDictionary* repository = map[SHL_REPOSITORY_KEY];
  for (NSString* name in repository) {
    NSDictionary* component = repository[name];
    SHLGrammarRule* rule = [self compileRule: component error:error];
    if (error != nil) return nil;
    [value setObject:rule forKey:name];
  }
  
  return value;
}

- (SHLGrammarRule*)compileRule: (NSDictionary*)map error:(NSError**)error {
  SHLGrammarRule* rule = [[SHLGrammarRule alloc] init];
  
  if (map[SHL_INCLUDE_KEY] != nil) {
    rule.include = map[SHL_INCLUDE_KEY];
    return rule;
  }
  rule.name = map[SHL_NAME_KEY];
  rule.contentName = map[SHL_CONTENT_NAME_KEY];
  rule.match = [self compileRegex:map[SHL_MATCH_KEY] error:error];
  rule.captures = map[SHL_CAPTURE_KEY];
  rule.begin = [self compileRegex:map[SHL_BEGIN_KEY] error:error];
  rule.beginCaptures = map[SHL_BEGIN_CAPTURE_KEY];
  rule.end = [self compileRegex:map[SHL_END_KEY] error:error];
  rule.endCaptures = map[SHL_END_CAPTURE_KEY];
  
  
  NSMutableArray* subPatterns = [[NSMutableArray alloc] init];
  NSDictionary* subRules = map[SHL_PATTERNS_KEY];
  for (NSDictionary* subRule in subRules) {
    [subPatterns addObject: [self compileRule:subRule error:error]];
    if (error != nil) return nil;
  }
  rule.patterns = subPatterns;
  
  
  return rule;
}

- (SHLRegex*)compileRegex: (NSString*)regex error: (NSError**)error {
  if (regex == nil || [regex isEqualToString:@""]) return nil;
  
  NSError* underlyingError = nil;
  SHLRegex* result = [[SHLRegex alloc] initWithPatern:regex option:0 error:&underlyingError];
  NSAssert(result != nil, @"invalid pattern encountered");
  
  return result;
}

- (void)compileCapture: (NSDictionary*)capture {
  
}

- (void)processScope: (SHLScope*)scope grammar: (SHLGrammar*)grammar error: (NSError**)error {
  if (scope.compiled) return;
  
  for (int idx = 0; idx < [scope.patterns count]; idx++) {
    SHLFantomObject* weak_wrapper = [self resolveInclude: scope.patterns[idx] parent: scope grammar: grammar];
    if (weak_wrapper != nil) scope.patterns[idx] = weak_wrapper;
  }
  
  //TODO compile terminators
}

- (SHLFantomObject*)resolveInclude: (SHLScope*)patterns parent:(SHLScope*)parent grammar:(SHLGrammar*)grammar {
  if (patterns.include == nil) return nil; // this is not a include rule
  
  if ([patterns.include isEqualToString:@"$self"]) {
    return [[SHLFantomObject alloc] initWithObject:parent];
    
  } else if ([patterns.include characterAtIndex:0] == '#') {
    NSString* repoName = [patterns.include substringFromIndex:1];
    SHLScope* stockedScope = grammar.repository[repoName];
    NSAssert(stockedScope != nil, @"reference to a not existed sotcked rule in repository");
    return [[SHLFantomObject alloc] initWithObject:stockedScope];
    
  } else {
    // TODO return sub language
    
    return nil;
  }
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
