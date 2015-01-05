//
//  GrammarCompiler.m
//  TMHighlighter
//
//  Created by david on 1/3/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "GrammarCompiler.h"
#import "RegularExpressionWrapper.h"

@implementation GrammarCompiler
- (void)compile: (Grammar*)aGrammar {
  [self compileMode:[aGrammar getTopLevelMode] withGrammar:aGrammar withParent:nil];
}

- (BOOL)isGrammarCaseInsensitive:(Grammar*) aGrammar {
  return aGrammar.getTopLevelMode[SHL_CASE_INSENSITIVE] == nil? NO : YES;
}

- (void)flattenKeywords: (NSString*) aKeywords withCategory: (NSString*) category inKeywordList: (NSMutableDictionary*) keywordList withGrammar: (Grammar*)aGrammar {
  NSAssert([aKeywords isKindOfClass:[NSString class]], @"the keywords should be string");
  
  NSString* toProcess = aKeywords;
  if ([self isGrammarCaseInsensitive:aGrammar]) toProcess = [aKeywords lowercaseString];
  
  NSArray* keyword_list = [toProcess componentsSeparatedByString:@" "];
  for (NSString* kw in keyword_list) {
    NSArray* keyword_relevance = [kw componentsSeparatedByString:@"|"];
    NSNumber* relevance = @1;
    if ([keyword_relevance count] > 1) {
      relevance = [NSNumber numberWithInt:[keyword_relevance[1] intValue]];
    }
    
    keywordList[keyword_relevance[0]] = @[category, relevance];
  }
}

- (NSDictionary*)compileKeywords: (id) aKeywords withGrammar: (Grammar*)aGrammar {
  NSMutableDictionary* compiledKeywords = [[NSMutableDictionary alloc] init];
  if ([aKeywords isKindOfClass:[NSString class]]) {
    [self flattenKeywords:aKeywords withCategory:SHL_KEYWORDS_KEYWORD_KEY inKeywordList:compiledKeywords withGrammar:aGrammar];
  } else if ([aKeywords isKindOfClass:[NSMutableDictionary class]]) {
    for (NSString* key in aKeywords){
      [self flattenKeywords:aKeywords[key] withCategory:key inKeywordList:compiledKeywords withGrammar:aGrammar];
    }
  } else {
    NSAssert(FALSE, @"keywords cannot contain values other than object or string");
  }
  
  return compiledKeywords;
}

- (RegularExpressionWrapper*)compileRegexFromString: (NSString*) aPattern withGrammar: (Grammar*)aGrammar{
  return [self compileRegexFromString:aPattern withGrammar:aGrammar global:NO];
}

- (RegularExpressionWrapper*)compileRegexFromString: (NSString*)aPattern withGrammar: (Grammar*)aGrammar global: (BOOL)isGlobal{
  return [[RegularExpressionWrapper alloc] initWithPattern:aPattern caseInsensitive:[self isGrammarCaseInsensitive:aGrammar] isGlobal:isGlobal];
}

- (NSMutableDictionary*)inheritFromParent: (NSDictionary*) aParent forObject: (NSDictionary*) aObject {
  NSMutableDictionary* result = [[NSMutableDictionary alloc] initWithDictionary:aParent];
  [result addEntriesFromDictionary:aObject];
  return result;
}

- (void)compileMode: (NSMutableDictionary*) aMode withGrammar:(Grammar*)aGrammar withParent: (NSMutableDictionary*) aParent {
  if (aMode[SHL_HAS_COMPILED_KEY]) return;
  aMode[SHL_HAS_COMPILED_KEY] = @YES;
  
  id keyword = aMode[SHL_KEYWORDS_KEY]? aMode[SHL_KEYWORDS_KEY] : aMode[SHL_BEGINKEYWORDS_KEY];
  if (keyword) aMode[SHL_KEYWORDS_KEY] = [self compileKeywords:keyword withGrammar:aGrammar];
  
  NSString* lexemes = aMode[SHL_LEXEMES_KEY]? aMode[SHL_LEXEMES_KEY] : @"\\b[A-Za-z0-9_]+\\b";
  aMode[SHL_LEXEMES_RE_KEY] = [self compileRegexFromString:lexemes withGrammar:aGrammar global:true];
  
  if (aParent) {
    if (aMode[SHL_BEGINKEYWORDS_KEY]) {
      NSString* beginStr = [NSString stringWithFormat:@"\\b(%@)\\b", [[aMode[SHL_BEGINKEYWORDS_KEY] componentsSeparatedByString:@" "] componentsJoinedByString:@"|"]];
      aMode[SHL_BEGIN_KEY] = beginStr;
    }
    if (!aMode[SHL_BEGIN_KEY]) aMode[SHL_BEGIN_KEY] = @"\\B|\\b";
    aMode[SHL_BEGIN_RE_KEY] = [self compileRegexFromString:aMode[SHL_BEGIN_KEY] withGrammar:aGrammar];
    
    if (!aMode[SHL_END_KEY] && !aMode[SHL_ENDS_WITH_PARENT_KEY]) aMode[SHL_END_KEY] = @"\\B|\\b";
    if (aMode[SHL_END_KEY]) aMode[SHL_END_RE_KEY] = [self compileRegexFromString:aMode[SHL_END_KEY] withGrammar:aGrammar];
    
    aMode[SHL_TERMINATOR_END_KEY] = aMode[SHL_END_KEY]? aMode[SHL_END_KEY] : @"";
    if (aMode[SHL_ENDS_WITH_PARENT_KEY]) {
      NSString* terminatorStr = [NSString stringWithFormat:@"%@%@%@", aMode[SHL_TERMINATOR_END_KEY], aMode[SHL_END_KEY]? @"|" : @"", aParent[SHL_TERMINATOR_END_KEY]];
      aMode[SHL_TERMINATOR_END_KEY] = terminatorStr;
    }
  }
  
  if (aMode[SHL_ILLEGAL_KEY]) aMode[SHL_ILLEGAL_RE_KEY] = [self compileRegexFromString:aMode[SHL_ILLEGAL_KEY] withGrammar:aGrammar];
  if (!aMode[SHL_RELEVANCE_KEY]) aMode[SHL_RELEVANCE_KEY] = @1;
  if (!aMode[SHL_CONTAINS_KEY]) aMode[SHL_CONTAINS_KEY] = @[];
  
  NSMutableArray* expanded_contains = [[NSMutableArray alloc] init];
  for (NSDictionary* contain in aMode[SHL_CONTAINS_KEY]) {
    if ([[contain class] isSubclassOfClass:[NSDictionary class]] && contain[SHL_VARIANTS_KEY]) {
      for (NSDictionary* variant in contain[SHL_VARIANTS_KEY]) {
        [expanded_contains addObject:[self inheritFromParent:contain forObject:variant]];
      }
    } else {
      [expanded_contains addObject: [contain isEqualTo:@"self"]? aMode : contain]; //TODO is mem leak? should we use weak pointer???
    }
  }
  aMode[SHL_CONTAINS_KEY] = expanded_contains;
  for (NSMutableDictionary* subMode in aMode[SHL_CONTAINS_KEY]) {
    [self compileMode:subMode withGrammar:aGrammar withParent:aMode];
  }
  
  if (aMode[SHL_STARTS_KEY]) [self compileMode:aMode[SHL_STARTS_KEY] withGrammar:aGrammar withParent:aParent];
  
  NSMutableArray* terminator = [[NSMutableArray alloc]init];
  for (NSDictionary* contain in aMode[SHL_CONTAINS_KEY]) {
    NSString* terminatorStr = [NSString stringWithFormat:@"\\.?(%@)\\.?", contain[SHL_BEGIN_KEY]];
    if (contain[SHL_BEGINKEYWORDS_KEY]) terminatorStr = contain[SHL_BEGIN_KEY];
    
    [terminator addObject:terminatorStr];
  }
  if(aMode[SHL_TERMINATOR_END_KEY]) [terminator addObject:aMode[SHL_TERMINATOR_END_KEY]];
  if(aMode[SHL_ILLEGAL_KEY]) [terminator addObject:aMode[SHL_ILLEGAL_KEY]];
  if ([terminator count] != 0) {
    NSString* pattern = [terminator componentsJoinedByString:@"|"];
    RegularExpressionWrapper* regex = [self compileRegexFromString:pattern withGrammar:aGrammar global:YES];
    aMode[SHL_TERMINATORS_KEY] = regex;
  }
  
}
@end
