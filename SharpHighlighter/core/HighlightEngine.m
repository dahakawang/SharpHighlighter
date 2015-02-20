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
#import "RangedString.h"


static HighlightAction* makeAction(NSRange range, NSArray* modeStack, NSString* name) {
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

#pragma mark Public Interface

- (instancetype)initWithTheme: (Theme*)theme {
  if (self = [super init]) {
    _theme = theme;
  }
  return self;
}

- (NSArray*)highlightText: (NSString*) aText withGrammar: (Grammar*) aGrammar ignoreIllegal: (BOOL)ignoreIllegal {
  
  NSMutableArray* action = [[NSMutableArray alloc] init];
  NSMutableArray* modeStack = [[NSMutableArray alloc] initWithArray:@[[aGrammar getTopLevelMode]]];
  RangedString* source = [[RangedString alloc] initWithString:aText];
  
  [self highlight:source skip:0 modes:modeStack actions:action];
  
  return action;
}

#pragma mark Highlight related

/*****************************************************************
 *
 * This method does the actual highlighting work recursively
 *
 */
- (NSRange)highlight: (RangedString*)source skip: (NSUInteger)count modes: (NSMutableArray*)modeStack actions: (NSMutableArray*)action {
  
  NSDictionary *currentMode = [self getCurrentMode:modeStack];
  NSUInteger nextModeProcessIndex = source.location + count;
  NSUInteger nextKeyWordIndex = source.location;
  
  
  while (nextModeProcessIndex < source.endLocation) {
    NSTextCheckingResult *match = [self findLexeme:source begin:nextModeProcessIndex mode:currentMode];
    NSRange lexemeRange = match.range;
    NSString* lexeme = [source newWithRange: lexemeRange].rangedString;
    NSDictionary* newMode = nil;
    BOOL endWithParents = NO;
    
    
    // Current mode can't be ended if we can't find more lexeme. For non-root mode, this
    // should be a problem. For root mode, it's meant to be ended with the end of string,
    // we just process remainning keywords.
    if (match == nil){
      if ([self isCurrentRootMode:modeStack]) {
        RangedString* newstr = [source newWithBegin:nextKeyWordIndex];
        [self processKeywordsForString:newstr modes:modeStack actions:action];
        return newstr.range;
      }
      
      @throw [ShlException exeptionWithReason:@"Can't find the end lexeme for current mode" userInfo:NULL];
    }
    
    
    [self processKeywordsForString:[source newWithBegin:nextKeyWordIndex andEnd:lexemeRange.location] modes:modeStack actions:action];
    
    
    
    if ([self isModeEndBy:lexeme modes:modeStack endWithParentFlag:&endWithParents]) {
      return [self calcModeEndRange:lexemeRange endWithParents:endWithParents];
      
      
    } else if ([self isNewModeStartBy:lexeme mode:currentMode newMode:&newMode]) {
      NSRange newRange = [self ProcessNewMode:newMode aText:source beginLexRange:lexemeRange nextIndex:&nextModeProcessIndex modes:modeStack actions:action];
      
      while (newMode[SHL_STARTS_KEY]) {
        newMode = newMode[SHL_STARTS_KEY];
        newRange = [self ProcessNewMode:newMode aText:source beginLexRange:newRange nextIndex:&nextModeProcessIndex modes:modeStack actions:action  ];
      }
      
      
      /* When an illegal character is detected */
    } else if ([self isAnIlllegalLexeme:lexeme mode:currentMode]) {
      @throw [ShlException exeptionWithReason:@"Illegal character detected" userInfo:NULL];
      
      
    } else {
      // TODO may need to information to debug!!!
      NSLog(@"Unknown error when parsing the text");
      nextModeProcessIndex++;
    }
    
    nextKeyWordIndex = nextModeProcessIndex; //Index catch up with each other
  }
  
  /* we should never reatch here */
  @throw [NSException exceptionWithName:NSInternalInconsistencyException reason:@"" userInfo:NULL];
}

#pragma mark Process and adding actions

/****************************************************************
 *
 * Process all the keywords
 *
 */
- (void)processKeywordsForString: (RangedString*)aText modes:(NSArray*)modeStack actions: (NSMutableArray*)action {
  NSDictionary* mode = modeStack[[modeStack count] - 1];
  NSDictionary* grammar = modeStack[0];
  BOOL isIgnoreCase = (BOOL)grammar[SHL_CASE_INSENSITIVE];
  
  RegularExpressionWrapper* regex = mode[SHL_LEXEMES_RE_KEY];
  NSArray* matches = [aText findMatches:regex];
  
  for (NSTextCheckingResult* match in matches) {
    NSString* keyword = [aText newWithRange:match.range].rangedString;
    if (isIgnoreCase) keyword = [keyword lowercaseString];
    NSDictionary* keywordList = mode[SHL_KEYWORDS_KEY];
    if (keywordList[keyword]) {
      NSString* className = keywordList[keyword][0]; //TODO handle relevance
      [action addObject: makeAction(match.range, modeStack, className)];
    }
  }
}

/****************************************************************
 *
 * Process the new mode by calling highlight:skip:modes:actions recursively
 *
 */
- (NSRange)ProcessNewMode:(NSDictionary *)newMode aText:(RangedString *)aText beginLexRange:(NSRange)begin nextIndex:(NSUInteger *)nextIndex modes:(NSMutableArray *)modeStack actions:(NSMutableArray *)action {
  
  long actionInsertionPoint = [action count];
  [modeStack addObject:newMode];  // *** Push
  
  
  RangedString* subModeText = [aText newWithBegin:begin.location];
  NSUInteger skip_count = [self calcSkipCount:newMode beginLexRange:begin];
  
  
  NSRange subModeEndRange = [self highlight:subModeText skip:skip_count modes:modeStack actions:action];
  [self addActionForNewMode:newMode beginLexRange:begin endLexRange:subModeEndRange at:actionInsertionPoint modes:modeStack actions:action];
  
  
  [modeStack removeObjectAtIndex: [modeStack count] - 1]; // *** Pop and balance the mode stack
  
  
  *nextIndex = [self calcNextIndex:subModeEndRange mode:newMode];
  return NSMakeRange(*nextIndex, 0);
}

#pragma mark Helper methods

- (NSDictionary *)getCurrentMode:(NSArray *)modeStack {
  NSDictionary* currentMode = modeStack[[modeStack count] - 1];
  return currentMode;
}

/****************************************************************
 *
 * Insert an action for the just processed new mode
 *
 */
- (void)addActionForNewMode:(NSDictionary *)newMode beginLexRange:(NSRange)begin endLexRange:(NSRange)end at:(long)offset modes:(NSMutableArray *)modeStack actions:(NSMutableArray *)action {
  
  NSRange curModeRange;
  if (newMode[SHL_CLASS_NAME_KEY] == nil) return;
  NSAssert(begin.location+begin.length <= end.location, @"the end range should be after begin range");
  
  if (newMode[SHL_EXCLUDE_BEGIN_KEY]) {
    curModeRange.location = begin.location + begin.length;
  } else {
    curModeRange.location = begin.location;
  }
  if (newMode[SHL_EXCLUDE_END_KEY]) {
    curModeRange.length = end.location - curModeRange.location;
  } else {
    curModeRange.length = end.location + end.length - curModeRange.location;
  }
  
  // at this time, the modeStack has not been popped, so the named contained
  // in the stack top mode. There is no need to set name manually
  [action insertObject:makeAction(curModeRange, modeStack, nil) atIndex:offset];
  
}

/****************************************************************
 *
 * Find the new next process index after processed new mode
 *
 */
- (NSUInteger)calcNextIndex: (NSRange)endLexRange mode: (NSDictionary*)mode {
  if (mode[SHL_RETURN_END_KEY]) {
    return endLexRange.location;
  } else {
    return endLexRange.location + endLexRange.length;
  }
}

/****************************************************************
 *
 * Calculate the skip count
 *
 *
 */
- (long)calcSkipCount:(NSDictionary *)newMode beginLexRange:(NSRange)begin {
  if (newMode[SHL_RETURN_BEGIN_KEY]) {
    return 0;
  } else {
    return begin.length;
  }
}

/****************************************************************
 *
 * Find next lexeme in the source text from the begin offset
 *
 */
- (NSTextCheckingResult *)findLexeme:(RangedString *)source begin:(NSUInteger)begin mode:(NSDictionary *)currentMode  {
  RegularExpressionWrapper* terminatorsRegex = currentMode[SHL_TERMINATORS_KEY];
  NSAssert(terminatorsRegex != nil, @"the compiled mode definition must has a terminators key");
  
  NSTextCheckingResult* match = [[source newWithBegin:begin] findFirstMatch:terminatorsRegex];
  return match;
}

/*****************************************************************
 *
 * When a mode ends, it will return the range for the last lexeme
 * (i.e. the terminating lexeme matched by the end key), and parent 
 * mode then will resume processing after that terminating lexeme.
 * But if the mode is has property "endWithParents:true", and the
 * found lexeme is also matched by its parent's end key , we should
 * return the lexeme back, otherwise the parent will fail to be ended.
 *
 */
- (NSRange)calcModeEndRange:(NSRange)lexemeRange endWithParents:(BOOL)endWithParents {
  if (endWithParents) {
    lexemeRange.length = 0;
    return lexemeRange;
  } else {
    return lexemeRange;
  }
}

/****************************************************************
 *
 * Determine if curreny mode is the root mode, root mode must on
 * the bottom of the mode stack.
 *
 */
- (BOOL)isCurrentRootMode:(NSArray*)modeStack {
  return [modeStack count] == 1;
}

#pragma mark method to categorize lexemes

/****************************************************************
 *
 * Test if current mode is ended by the lexeme
 *
 */
- (BOOL)isModeEndBy: (NSString*)lexeme modes: (NSArray*)modeStack endWithParentFlag: (BOOL*)flag{
  
  NSDictionary* currentMode = [self getCurrentMode:modeStack];
  if (currentMode == nil) return NO;
  
  *flag = NO;
  RegularExpressionWrapper* regex = currentMode[SHL_END_RE_KEY];
  if ([regex isMatchingText:lexeme]) return YES;
  
  *flag = YES;
  if (currentMode[SHL_ENDS_WITH_PARENT_KEY]) {
    NSDictionary* parent = modeStack[[modeStack count]-2];
    RegularExpressionWrapper*regex = parent[SHL_TERMINATOR_END_RE_KEY]; //TODO consier if parent is also end with its parent
    if ([regex isMatchingText:lexeme]) return YES;
  }
  return NO;
}

/****************************************************************
 *
 * Test if a new mode has been started
 *
 */
- (BOOL)isNewModeStartBy: (NSString*)lexeme mode: (NSDictionary*)currentMode newMode: (NSDictionary**)newMode {
  for (NSDictionary* mode in currentMode[SHL_CONTAINS_KEY]) {
    RegularExpressionWrapper* regex = mode[SHL_BEGIN_RE_KEY];
    if ([regex isMatchingText:lexeme]) {
      *newMode = mode;
      return YES;
    }
  }
  
  return NO;
}

/****************************************************************
 *
 * Test if we have encountered an illegal lexeme
 *
 */
- (BOOL)isAnIlllegalLexeme: (NSString*)lexeme mode: (NSDictionary*)currentMode {
  RegularExpressionWrapper* regex = currentMode[SHL_ILLEGAL_RE_KEY];
  return [regex isMatchingText:lexeme];
}

@end
