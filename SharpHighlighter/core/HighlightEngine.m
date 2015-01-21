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


HighlightAction* MakeAction(NSRange range, NSArray* modeStack, NSString* name) {
  NSMutableArray* className = [[NSMutableArray alloc] init];
  
  [className addObject: modeStack[0][SHL_SYNTAX_NAME_KEY]];
  for (long it = 0; it < [modeStack count]; it++) {
    if (modeStack[it][SHL_CLASS_NAME_KEY]) {
      [className addObject:modeStack[it][SHL_CLASS_NAME_KEY]];
    }
  }
  if (name != nil) {
    [className addObject:name];
  }
  
  NSString* classname = [className componentsJoinedByString:@" "];
  return [[HighlightAction alloc] initWithRange:range andclassName:classname];
}


@implementation HighlightEngine {
  Theme* _theme;
}

- (instancetype)initWithTheme: (Theme*)theme {
  if (self = [super init]) {
    _theme = theme;
  }
  return self;
}

- (NSArray*)highlightText: (NSString*) aText withGrammar: (Grammar*) aGrammar ignoreIllegal: (BOOL)ignoreIllegal {
  NSMutableArray* action = [[NSMutableArray alloc] init];
  NSMutableArray* modeStack = [[NSMutableArray alloc] initWithArray:@[[aGrammar getTopLevelMode]]];
  [self highlight:aText inRange:NSMakeRange(0, [aText length]) skipCount:0 withModeStack:modeStack andAction:action];
  
  return action;
}

- (void)processKeywordsForString: (NSString*)aText withinRange: (NSRange) range withModeStack:(NSArray*)modeStack action: (NSMutableArray*)action {
  NSDictionary* mode = modeStack[[modeStack count] - 1];
  
  RegularExpressionWrapper* regex = mode[SHL_LEXEMES_RE_KEY];
  NSArray* matches = [regex matchText:aText inRange:range];
  
  for (NSTextCheckingResult* match in matches) {
    NSString* keyword = [aText substringWithRange:match.range];
    NSDictionary* keywordList = mode[SHL_KEYWORDS_KEY];
    if (keywordList[keyword]) {
      NSString* className = keywordList[keyword][0]; //TODO handle relevance
      [action addObject: MakeAction(match.range, modeStack, className)];
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


- (NSRange)highlight: (NSString*)aText inRange:(NSRange)range skipCount: (NSUInteger)count withModeStack: (NSMutableArray*)modeStack andAction: (NSMutableArray*)action {
  NSDictionary* currentMode = modeStack[[modeStack count] - 1];
  
  // process sub-modes
  long nextModeProcessIndex = range.location + count;
  long nextKeyWordIndex = range.location;
  while (nextModeProcessIndex < range.location + range.length) {
    RegularExpressionWrapper* terminatorsRegex = currentMode[SHL_TERMINATORS_KEY];
    NSArray* matches = [terminatorsRegex matchText:aText inRange:NSMakeRange(nextModeProcessIndex, range.location + range.length - nextModeProcessIndex)];
    NSDictionary* newMode = nil;
    
    if ([matches count] == 0){
      if (!currentMode[SHL_END_KEY]) {
        [self processKeywordsForString:aText withinRange:NSMakeRange(nextKeyWordIndex, range.location + range.length - nextKeyWordIndex) withModeStack:modeStack action:action];
        return NSMakeRange(nextModeProcessIndex, range.location + range.length - nextModeProcessIndex);
      }
      
      @throw [ShlException exeptionWithReason:@"and unknown error has occured" userInfo:NULL];
    }
    
    NSRange lexemeRange = ((NSTextCheckingResult*)matches[0]).range;
    NSString* lexeme = [aText substringWithRange: lexemeRange];
    
    [self processKeywordsForString:aText withinRange:NSMakeRange(nextKeyWordIndex, lexemeRange.location - nextKeyWordIndex) withModeStack:modeStack action:action];
    
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
      NSRange workingRange = NSMakeRange(lexemeRange.location, range.length + range.location - lexemeRange.location);
      long skip_count = lexemeRange.length; // by default, the sub-model will bypass the matched begin pattern
      if (newMode[SHL_RETURN_BEGIN_KEY]) {
        skip_count = 0;
      }
    
      NSRange subModeEndRange = [self highlight:aText inRange:workingRange skipCount:skip_count withModeStack:modeStack andAction:action];
      
      if (newMode[SHL_RETURN_END_KEY]) {
        nextKeyWordIndex = nextModeProcessIndex = subModeEndRange.location;
      } else {
        nextKeyWordIndex = nextModeProcessIndex = subModeEndRange.location + subModeEndRange.length;
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
      
      if (newMode[SHL_CLASS_NAME_KEY]) {
        // at this time, the modeStack has not been popped, so it's still the submode's stack
        [action insertObject:MakeAction(curModeRange, modeStack, nil) atIndex:currentActionSize];
      }
      [modeStack removeObjectAtIndex: [modeStack count] - 1]; //balance the mode stack

      
      
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
