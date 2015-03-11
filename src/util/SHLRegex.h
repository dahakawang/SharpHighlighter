//
//  SHLRegex.h
//  SharpHighlighter
//
//  Created by david on 3/10/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <oniguruma.h>

#define SHLREGEX_DOMAIN @"SHLRegex"
#define SHLREGEX_RUNTIME_ERROR_CODE 1
#define SHLREGEX_INVALID_CHAR 2
#define SHLREGEX_ONI_ERROR 3

typedef enum {
  NONE = ONIG_OPTION_NONE,
  SINGLELINE = ONIG_OPTION_SINGLELINE,
  DOTALL = ONIG_OPTION_DOTALL,
  MULTILINE = ONIG_OPTION_MULTILINE,
  IGNORECASE = ONIG_OPTION_IGNORECASE,
  EXTEND = ONIG_OPTION_EXTEND,
  FIND_LONGEST = ONIG_OPTION_FIND_LONGEST,
  FIND_NOT_EMPTY = ONIG_OPTION_FIND_NOT_EMPTY,
  NEGATE_SINGLELINE = ONIG_OPTION_NEGATE_SINGLELINE,
  DONT_CAPTURE_GROUP = ONIG_OPTION_DONT_CAPTURE_GROUP,
  CAPTURE_GROUP = ONIG_OPTION_CAPTURE_GROUP,
  NEWLINE_CRLF = ONIG_OPTION_NEWLINE_CRLF,
}SHLRegexOption;


@interface SHLRegex: NSObject
- (instancetype)initWithPatern: (NSString*)pattern option: (SHLRegexOption)option error: (NSError**)error;
@end