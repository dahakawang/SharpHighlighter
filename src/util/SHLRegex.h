//
//  SHLRegex.h
//  SharpHighlighter
//
//  Created by david on 3/10/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>
#include <oniguruma.h>

#define SHLREGEX_DOMAIN @"net.davidvoid.SharpHighlighter.SHLRegex"
#define SHLREGEX_RUNTIME_ERROR_CODE 1
#define SHLREGEX_INVALID_CHAR 2
#define SHLREGEX_ONI_ERROR 3

typedef enum {
  SHLREGEX_OPTION_NONE = ONIG_OPTION_NONE,
  SHLREGEX_OPTION_SINGLELINE = ONIG_OPTION_SINGLELINE,
  SHLREGEX_OPTION_DOTALL = ONIG_OPTION_DOTALL,
  SHLREGEX_OPTION_MULTILINE = ONIG_OPTION_MULTILINE,
  SHLREGEX_OPTION_IGNORECASE = ONIG_OPTION_IGNORECASE,
  SHLREGEX_OPTION_EXTEND = ONIG_OPTION_EXTEND,
  SHLREGEX_OPTION_FIND_LONGEST = ONIG_OPTION_FIND_LONGEST,
  SHLREGEX_OPTION_FIND_NOT_EMPTY = ONIG_OPTION_FIND_NOT_EMPTY,
  SHLREGEX_OPTION_NEGATE_SINGLELINE = ONIG_OPTION_NEGATE_SINGLELINE,
  SHLREGEX_OPTION_DONT_CAPTURE_GROUP = ONIG_OPTION_DONT_CAPTURE_GROUP,
  SHLREGEX_OPTION_CAPTURE_GROUP = ONIG_OPTION_CAPTURE_GROUP,
  SHLREGEX_OPTION_NEWLINE_CRLF = ONIG_OPTION_NEWLINE_CRLF,
  
  SHLREGEX_OPTION_NOTBOL = ONIG_OPTION_NOTBOL,
  SHLREGEX_OPTION_NOTEOL = ONIG_OPTION_NOTEOL,
  SHLREGEX_OPTION_NOTBOS = ONIG_OPTION_NOTBOS,
  SHLREGEX_OPTION_NOTEOS = ONIG_OPTION_NOTEOS,
  SHLREGEX_OPTION_POSIX_REGION = ONIG_OPTION_POSIX_REGION
}SHLRegexOption;



@interface SHLMatch : NSObject
@property (readonly, nonatomic, getter=getCount) NSUInteger count;
@property (readonly, nonatomic, getter=getRange) NSRange range;
@property (readonly, nonatomic, getter=getString) NSString* string;
- (NSRange)rangeAtIndex: (NSUInteger)index;
- (NSString*)stringAtIndex: (NSUInteger)index;
@end

@interface SHLRegex: NSObject
@property (readonly, nonatomic) NSString* pattern;
- (instancetype)initWithPatern: (NSString*)pattern option: (SHLRegexOption)option error: (NSError**)error;
- (SHLMatch*)match: (NSString*)target range: (NSRange)range option: (SHLRegexOption)option error:(NSError **)error;
- (SHLMatch*)search: (NSString*)target range: (NSRange)range option: (SHLRegexOption)option error:(NSError **)error;
@end

