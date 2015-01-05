//
//  HighlightEngine.m
//  TMHighlighter
//
//  Created by david on 1/5/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "HighlightEngine.h"

@implementation HighlightEngine
- (NSAttributedString*)highlightText: (NSString*) aText withGrammar: (Grammar*) aGrammar ignoreIllegal: (BOOL)ignoreIllegal withContinuation: (NSDictionary*)continuation {
  return [[NSAttributedString alloc]initWithString:@"sss"];
}

- (NSAttributedString*)highlightText: (NSString*) aText withGrammar: (Grammar*) aGrammar ignoreIllegal: (BOOL)ignoreIllegal {
  return [self highlightText:aText withGrammar:aGrammar ignoreIllegal:ignoreIllegal withContinuation:nil];
}
@end
