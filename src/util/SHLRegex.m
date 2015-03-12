//
//	SHLRegex.m
//  SharpHighlighter
//
//  Created by david on 1/3/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "SHLRegex.h"

#define SHLREGEX_INTERNAL_ENCODING NSUTF16LittleEndianStringEncoding
#define CHAR_SIZE 2


@interface SHLMatch()
- (instancetype)initWith: (OnigRegion*)region regex: (SHLRegex*)regex target:(NSString*)string;
@end


@implementation SHLRegex {
  regex_t* _regex;
  NSString* _pattern;
}

+ (void)initialize {
  //This function is not thread safe,
  //so call it in static initializer
  onig_init();
}

- (void)dealloc {
  if (_regex != NULL) {
    onig_free(_regex);
  }
}

- (instancetype)initWithPatern:(NSString *)pattern option:(SHLRegexOption)option error:(NSError **)error {
  if ([super init] == nil) return nil;
  _regex = NULL;
  
  
  if (pattern == nil) {
    [self makeError:error message:@"pattern can't be nil" code:SHLREGEX_RUNTIME_ERROR_CODE];
    return nil;
  }
  
  if (![pattern canBeConvertedToEncoding:SHLREGEX_INTERNAL_ENCODING] ) {
    [self makeError: error message:@"pattern contains invalid character that can't be converted to UTF16" code:SHLREGEX_INVALID_CHAR];
    return nil;
  }
  
  _pattern = pattern;
  NSData* utf16 = [pattern dataUsingEncoding:SHLREGEX_INTERNAL_ENCODING];
  
  OnigErrorInfo onig_error;
  int status = onig_new(&_regex, utf16.bytes, utf16.bytes + utf16.length, option, ONIG_ENCODING_UTF16_LE, ONIG_SYNTAX_DEFAULT, &onig_error);
  
  if (status != ONIG_NORMAL) {
    NSInteger errorCode = [self translateOniErrorCode:status];
    NSString* errorMessage = [self translateErrorMessage:status errInfo:&onig_error];
    [self makeError:error message:errorMessage code:errorCode];
    return nil;
  }
  
  return self;
}

- (SHLMatch*)match: (NSString*)target range: (NSRange)range option: (SHLRegexOption)option error:(NSError **)error{
  if (target == nil) {
    [self makeError:error message:@"target string for match can't be nil" code:SHLREGEX_RUNTIME_ERROR_CODE];
    return nil;
  }
  
  if (![target canBeConvertedToEncoding:SHLREGEX_INTERNAL_ENCODING]) {
    [self makeError:error message:@"target string contains invalid character that can't be converted to UTF16" code:SHLREGEX_INVALID_CHAR];
    return nil;
  }
  
  if (range.location + range.length > [target length]) {
    [self makeError:error message:@"out of range of target string" code:SHLREGEX_RUNTIME_ERROR_CODE];
    return nil;
  }
  
  NSData* utf16 = [target dataUsingEncoding:SHLREGEX_INTERNAL_ENCODING];
  
  OnigRegion* region = onig_region_new();
  OnigPosition position = onig_match(_regex,
                          utf16.bytes,
                          utf16.bytes + (range.location + range.length) * CHAR_SIZE,
                          utf16.bytes + range.location * CHAR_SIZE, region, option);
  
  SHLMatch* match = nil;
  if (position != ONIG_MISMATCH) {
    match = [[SHLMatch alloc] initWith: region regex: self target: target];
  }
  
  onig_region_free(region, TRUE); //SHLMatch dosen't own OnigRegion object, so we release it
  
  return match;
}

- (SHLMatch*)search: (NSString*)target range: (NSRange)range option: (SHLRegexOption)option error:(NSError **)error{
  if (target == nil) {
    [self makeError:error message:@"target string for match can't be nil" code:SHLREGEX_RUNTIME_ERROR_CODE];
    return nil;
  }
  
  if (![target canBeConvertedToEncoding:SHLREGEX_INTERNAL_ENCODING]) {
    [self makeError:error message:@"target string contains invalid character that can't be converted to UTF16" code:SHLREGEX_INVALID_CHAR];
    return nil;
  }
  
  if (range.location + range.length > [target length]) {
    [self makeError:error message:@"out of range of target string" code:SHLREGEX_RUNTIME_ERROR_CODE];
    return nil;
  }
  
  NSData* utf16 = [target dataUsingEncoding:SHLREGEX_INTERNAL_ENCODING];
  
  OnigRegion* region = onig_region_new();
  OnigPosition position = onig_search(_regex,
                                     utf16.bytes,
                                     utf16.bytes + utf16.length,
                                     utf16.bytes + range.location * CHAR_SIZE,
                                     utf16.bytes + (range.location + range.length) * CHAR_SIZE,region, option);
  
  SHLMatch* match = nil;
  if (position != ONIG_MISMATCH) {
    match = [[SHLMatch alloc] initWith: region regex: self target: target];
  }
  
  onig_region_free(region, TRUE); //SHLMatch dosen't own OnigRegion object, so we release it
  
  return match;
}

- (void)makeError: (NSError**)error message:(NSString*)message code: (NSInteger)code {
  NSDictionary* userInfo = @{NSLocalizedDescriptionKey: message};
  NSError* tmp = [NSError errorWithDomain:SHLREGEX_DOMAIN code:code userInfo:userInfo];
  
  if (error != NULL) {
    *error = tmp;
  }
}

- (NSInteger)translateOniErrorCode: (NSInteger)oniErrorCode {
  return (oniErrorCode > 0)? (SHLREGEX_ONI_ERROR + oniErrorCode*1000) : (oniErrorCode*1000 - SHLREGEX_ONI_ERROR);
}

- (NSString*)translateErrorMessage: (NSInteger)code errInfo:(OnigErrorInfo*)errInfo {
  int length = 0;
  OnigUChar buffer[ONIG_MAX_ERROR_MESSAGE_LEN];
  
  if (errInfo != nil){
      length = onig_error_code_to_str(buffer, code, errInfo);
  } else {
    length = onig_error_code_to_str(buffer, code);
  }
  
  NSString* errorMessage = [NSString stringWithCString:(char*)buffer encoding:NSASCIIStringEncoding];

  return errorMessage;
}
@end





@implementation SHLMatch {
  NSString* _target;
  SHLRegex* _regex;
  NSMutableArray* _captures;
}

- (NSUInteger)getCount {
  return [_captures count];
}

- (instancetype)initWith: (OnigRegion*)region regex: (SHLRegex*)regex target:(NSString*)string {
  if ([super init] == nil) return nil;
  
  _target = string;
  _regex = regex;
  _captures = [[NSMutableArray alloc] init];

  for (int index = 0; index < region->num_regs; index++) {
    OnigPosition begin = *(region->beg + index);
    OnigPosition end = *(region->end + index);
    
    NSAssert((begin%CHAR_SIZE) == 0, @"begin should be multiple of CHAR_SIZE");
    NSAssert((begin%CHAR_SIZE) == 0, @"end should be multiple of CHAR_SIZE");
    [_captures addObject:[NSValue valueWithRange:NSMakeRange(begin/CHAR_SIZE, (end - begin)/CHAR_SIZE)]];
  }
  
  return self;
}

- (NSRange)rangeAtIndex: (NSUInteger)index {
  return [[_captures objectAtIndex:index] rangeValue];
}

- (NSString*)stringAtIndex: (NSUInteger)index {
  return [_target substringWithRange:[self rangeAtIndex:index]];
}

- (NSString*)getString {
  return [self stringAtIndex:0];
}

- (NSRange)getRange {
  return [self rangeAtIndex:0];
}
@end
