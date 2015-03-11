//
//  SHLRegexTest.m
//  SharpHighlighter
//
//  Created by david on 3/11/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>
#import "SHLRegex.h"

@interface SHLRegexTest : XCTestCase

@end

@implementation SHLRegexTest

- (void)testCreateSHLRegex {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"\\d*" option:ONIG_OPTION_NONE error:&error];
  
  XCTAssertNotNil(regex);
  XCTAssertNil(error);
  
}

- (void)testCreateSHLRegexWithNullPattern {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:nil option:ONIG_OPTION_NONE error:&error];
  
  XCTAssertNil(regex);
  XCTAssertNotNil(error);
  XCTAssert([[error domain] isEqualToString:SHLREGEX_DOMAIN]);
  XCTAssert([error code] == SHLREGEX_RUNTIME_ERROR_CODE);
  XCTAssert([[error localizedDescription] isEqualToString:@"pattern can't be nil"]);
}

- (void)testCreateSHLRegexWithInvalidRegularExpression {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"[" option:ONIG_OPTION_NONE error:&error];

  XCTAssertNotNil(error);
  XCTAssertNil(regex);
}

- (void)testCreateSHLRegexNullError {

  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"[" option:ONIG_OPTION_NONE error:NULL];
  XCTAssertNil(regex);
  
  regex = [[SHLRegex alloc] initWithPatern:nil option:ONIG_OPTION_NONE error:NULL];
  XCTAssertNil(regex);
}

@end
