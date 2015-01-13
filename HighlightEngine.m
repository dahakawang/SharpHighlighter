//
//  HighlightEngine.m
//  TMHighlighter
//
//  Created by david on 1/5/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "HighlightEngine.h"
#import "GrammarCompiler.h"
#import "Grammar.h"
#import "RegularExpressionWrapper.h"
#import "ShlException.h"
#import "HighlightAction.h"

@implementation HighlightEngine
- (NSAttributedString*)highlightText: (NSString*) aText withGrammar: (Grammar*) aGrammar ignoreIllegal: (BOOL)ignoreIllegal withContinuation: (NSDictionary*)continuation {
  return [[NSAttributedString alloc]initWithString:@"sss"];
}

- (NSAttributedString*)highlightText: (NSString*) aText withGrammar: (Grammar*) aGrammar ignoreIllegal: (BOOL)ignoreIllegal {
  NSMutableArray* action = [[NSMutableArray alloc] init];
  NSMutableArray* modeStack = [[NSMutableArray alloc] initWithArray:@[[aGrammar getTopLevelMode]]];
  [self highlight:aText inRange:NSMakeRange(0, [aText length]) withModeStack:modeStack andAction:action];
  
  return [self highlightText:aText withGrammar:aGrammar ignoreIllegal:ignoreIllegal withContinuation:nil];
}

- (void)processKeywordsForString: (NSString*)aText withinRange: (NSRange) range withMode:(NSDictionary*)mode action: (NSMutableArray*)action {
  RegularExpressionWrapper* regex = mode[SHL_LEXEMES_RE_KEY];
  NSArray* matches = [regex matchText:aText inRange:range];
  
  for (NSTextCheckingResult* match in matches) {
    NSString* keyword = [aText substringWithRange:match.range];
    NSDictionary* keywordList = mode[SHL_KEYWORDS_KEY];
    if (keywordList[keyword]) {
      NSString* className = keywordList[keyword][0]; //TODO handle relevance
      [action addObject: MakeAction(match.range, className)];
    }
  }
}

- (BOOL)isCurrentMode: (NSDictionary*)currentMode endedByLexeme: (NSString*)lexeme withModeStack: (NSArray*)modeStack {
  if (currentMode == nil) return NO;
  
  RegularExpressionWrapper* regex = currentMode[SHL_END_RE_KEY];
  if ([regex isMatchingText:lexeme]) return YES;
  
  // TODO consider for endWithParent
  return NO;
}

- (BOOL)isCurrentMode: (NSDictionary*)currentMode openingNewModeByLexeme: (NSString*)lexeme newMode: (NSDictionary**)newMode {
  for (NSDictionary* mode in currentMode[SHL_CONTAINS_KEY]) {
    RegularExpressionWrapper* regex = mode[SHL_BEGIN_RE_KEY];
    if ([regex isMatchingText:lexeme]) {
      *newMode = mode;
      return YES;
    }
  }
  
  return NO;
}

- (BOOL)isCurrentMode: (NSDictionary*)currentMode encounterIllgalByLexeme: (NSString*) lexeme {
  RegularExpressionWrapper* regex = currentMode[SHL_ILLEGAL_RE_KEY];
  return [regex isMatchingText:lexeme];
}


- (NSRange)highlight: (NSString*)aText inRange:(NSRange)range withModeStack: (NSMutableArray*)modeStack andAction: (NSMutableArray*)action {
  NSDictionary* currentMode = modeStack[[modeStack count] - 1];
  
  // process sub-modes
  long nextProcessIndex = range.location;
  while (nextProcessIndex < range.location + range.length) {
    RegularExpressionWrapper* terminatorsRegex = currentMode[SHL_TERMINATORS_KEY];
    NSArray* matches = [terminatorsRegex matchText:aText inRange:NSMakeRange(nextProcessIndex, range.location + range.length - nextProcessIndex)];
    NSDictionary* newMode = nil;
    
    if ([matches count] == 0){
      //1 end by end of string,  2. error
    }
    
    NSRange lexemeRange = ((NSTextCheckingResult*)matches[0]).range;
    NSString* lexeme = [aText substringWithRange: lexemeRange];
    
    [self processKeywordsForString:aText withinRange:NSMakeRange(nextProcessIndex, lexemeRange.location - nextProcessIndex) withMode:currentMode action:action];
    
    long currentActionSize = [action count];
    /* When an illegal character is detected */
    if ([self isCurrentMode:currentMode encounterIllgalByLexeme:lexeme]) {
      @throw [ShlException exeptionWithReason:@"Illegal character detected" userInfo:NULL];
      
    /* When end of mode pattern is detected*/
    } else if ([self isCurrentMode:currentMode endedByLexeme:lexeme withModeStack:modeStack]) {
      return lexemeRange;
      
    /* When a new sub mode begin pattern is detected*/
    } else if ([self isCurrentMode:currentMode openingNewModeByLexeme:lexeme newMode:&newMode]) {
      [modeStack addObject:newMode];
      NSRange workingRange = NSMakeRange(lexemeRange.location + lexemeRange.length, 0);
      if (newMode[SHL_RETURN_BEGIN_KEY]) {
        workingRange.location = lexemeRange.location;
      }
      workingRange.length = range.length + range.location - workingRange.location;

      NSRange subModeEndRange = [self highlight:aText inRange:workingRange withModeStack:modeStack andAction:action];
      
      if (newMode[SHL_RETURN_END_KEY]) {
        nextProcessIndex = subModeEndRange.location;
      } else {
        nextProcessIndex = subModeEndRange.location + subModeEndRange.length;
      }
      
      NSRange curModeRange = NSMakeRange(lexemeRange.location, 0);
      if (newMode[SHL_EXCLUDE_BEGIN_KEY]) {
        curModeRange.location = lexemeRange.location + lexemeRange.length;
      }
      if (newMode[SHL_EXCLUDE_END_KEY]) {
        curModeRange.length = subModeEndRange.location - curModeRange.location;
      } else {
        curModeRange.length = subModeEndRange.location + subModeEndRange.length - curModeRange.location;
      }
      
      [modeStack removeObjectAtIndex: [modeStack count] - 1]; //balance the mode stack
      [action insertObject:MakeAction(curModeRange, newMode[SHL_CLASS_NAME_KEY]) atIndex:currentActionSize];
      
    } else {
      // TODO may need to information to debug!!!
      @throw [ShlException exeptionWithReason:@"Unknown error when parsing the text" userInfo:NULL];
    }
}
  
  /* we should never reatch here */
  // TODO throw
  return NSMakeRange(0, 0);
}
@end
