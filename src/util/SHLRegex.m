//
//	SHLRegex.m
//  SharpHighlighter
//
//  Created by david on 1/3/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "SHLRegex.h"

#define SHLREGEX_INTERNAL_ENCODING NSUTF16LittleEndianStringEncoding

@implementation SHLRegex {
  regex_t* _regex;
}

+ (void)initialize {
  //This function is not thread safe,
  //so call it in static initializer
  onig_init();
}

- (instancetype)initWithPatern:(NSString *)pattern option:(SHLRegexOption)option error:(NSError **)error {
  _regex = NULL;
  
  
  if (pattern == nil) {
    [self makeError:error message:@"pattern can't be nil" code:SHLREGEX_RUNTIME_ERROR_CODE];
    return nil;
  }
  
  if (![pattern canBeConvertedToEncoding:SHLREGEX_INTERNAL_ENCODING] ) {
    [self makeError: error message:@"pattern contains invalid character that can't be converted to UTF16" code:SHLREGEX_INVALID_CHAR];
    return nil;
  }
  
  
  NSData* utf16 = [pattern dataUsingEncoding:SHLREGEX_INTERNAL_ENCODING];
  
  OnigErrorInfo onig_error;
  int status = onig_new(&_regex, utf16.bytes, ((unsigned char*)utf16.bytes + utf16.length), option, ONIG_ENCODING_UTF16_LE, ONIG_SYNTAX_DEFAULT, &onig_error);
  
  if (status != ONIG_NORMAL) {
    NSInteger errorCode = [self translateOniErrorCode:status];
    NSString* errorMessage = [self translateErrorMessage:status errInfo:&onig_error];
    [self makeError:error message:errorMessage code:errorCode];
    return nil;
  }
  
  return self;
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

