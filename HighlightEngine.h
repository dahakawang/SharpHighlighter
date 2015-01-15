//
//  HighlightEngine.h
//  TMHighlighter
//
//  Created by david on 1/5/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Grammar.h"
#import "Theme.h"

@interface HighlightEngine : NSObject
- (instancetype)initWithTheme: (Theme*)theme;
- (NSArray*)highlightText: (NSString*) aText withGrammar: (Grammar*) aGrammar ignoreIllegal: (BOOL)ignoreIllegal;
@end
