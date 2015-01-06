//
//  GrammarCompiler.h
//  TMHighlighter
//
//  Created by david on 1/3/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Grammar.h"

#define SHL_HAS_COMPILED_KEY @"compiled"
#define SHL_CASE_INSENSITIVE @"case_insensitive"
#define SHL_KEYWORDS_KEY @"keywords"
#define SHL_KEYWORDS_KEYWORD_KEY @"keyword"
#define SHL_KEYWORDS__BUILDTIN_KEY @"built_in"
#define SHL_KEYWORDS_LITERAL_KEY @"literal"
#define SHL_LEXEMES_KEY @"lexemes"
#define SHL_LEXEMES_RE_KEY @"lexemesRe"
#define SHL_ILLEGAL_KEY @"illegal"
#define SHL_ILLEGAL_RE_KEY @"illegalRe"
#define SHL_RELEVANCE_KEY @"relevance"
#define SHL_CONTAINS_KEY @"contains"
#define SHL_BEGIN_KEY @"begin"
#define SHL_BEGIN_RE_KEY @"beginRe"
#define SHL_BEGINKEYWORDS_KEY @"beginKeywords"
#define SHL_END_KEY @"end"
#define SHL_END_RE_KEY @"endRe"
#define SHL_TERMINATOR_END_KEY @"terminator_end"
#define SHL_ENDS_WITH_PARENT_KEY @"endsWithParent"
#define SHL_VARIANTS_KEY @"variants"
#define SHL_STARTS_KEY @"starts"
#define SHL_TERMINATORS_KEY @"terminators"


@interface GrammarCompiler : NSObject
- (void)compile: (Grammar*)aGrammar;
@end
