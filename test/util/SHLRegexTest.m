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
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"\\d*" option:SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertNotNil(regex);
  XCTAssertNil(error);
  
}

- (void)testSHLRegexPatternPropertyWork {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"\\d*" option:SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertNotNil(regex);
  XCTAssertNil(error);
  
  XCTAssert([@"\\d*" isEqualToString:regex.pattern]);
  
}

- (void)testCreateSHLRegexWithNullPattern {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:nil option:SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertNil(regex);
  XCTAssertNotNil(error);
  XCTAssert([[error domain] isEqualToString:SHLREGEX_DOMAIN]);
  XCTAssert([error code] == SHLREGEX_RUNTIME_ERROR_CODE);
  XCTAssert([[error localizedDescription] isEqualToString:@"pattern can't be nil"]);
}

- (void)testCreateSHLRegexWithInvalidRegularExpression {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"[" option:SHLREGEX_OPTION_NONE error:&error];

  XCTAssertNotNil(error);
  XCTAssertNil(regex);
}

- (void)testCreateSHLRegexNullError {

  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"[" option:ONIG_OPTION_NONE error:NULL];
  XCTAssertNil(regex);
  
  regex = [[SHLRegex alloc] initWithPatern:nil option:ONIG_OPTION_NONE error:NULL];
  XCTAssertNil(regex);
}

- (void)testMatch {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"abc" option: SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertNil(error);
  XCTAssertNotNil(regex);
  
  SHLMatch* match = [regex match:@"abcdaf" range:NSMakeRange(0, 6) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(error);
  XCTAssertNotNil(match);
}

- (void)testNotMatch {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"abc" option: SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertNil(error);
  XCTAssertNotNil(regex);
  
  SHLMatch* match = [regex match:@"eabcdaf" range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(error);
  XCTAssertNil(match);
}

- (void)testMatchVariousErrorCondition {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"abc" option: SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertNil(error);
  XCTAssertNotNil(regex);
  
  SHLMatch* match = [regex match:nil range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(match);
  XCTAssertNotNil(error);
  
  match = [regex match:@"abcd" range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(match);
  XCTAssertNotNil(error);
}


- (void)testMatchRangeWorks {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"abc" option: SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertNil(error);
  XCTAssertNotNil(regex);
  
  SHLMatch* match = [regex match:@"eabcd" range:NSMakeRange(0, 4) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(match);
  XCTAssertNil(error);
  
  match = [regex match:@"eabcd" range:NSMakeRange(1, 4) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNotNil(match);
  XCTAssertNil(error);
  
  match = [regex match:@"eabcd" range:NSMakeRange(1, 3) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNotNil(match);
  XCTAssertNil(error);
  
  match = [regex match:@"eabcd" range:NSMakeRange(1, 2) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(match);
  XCTAssertNil(error);
  
  match = [regex match:@"eabcd" range:NSMakeRange(2, 3) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(match);
  XCTAssertNil(error);
}

- (void)testFound {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"cda" option: SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertNil(error);
  XCTAssertNotNil(regex);
  
  SHLMatch* match = [regex search:@"abcdaf" range:NSMakeRange(0, 6) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(error);
  XCTAssertNotNil(match);
}

- (void)testNotFound {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"sdf" option: SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertNil(error);
  XCTAssertNotNil(regex);
  
  SHLMatch* match = [regex search:@"abcdaf" range:NSMakeRange(0, 6) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(error);
  XCTAssertNil(match);
}

- (void)testSearchVariousErrorCondition {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"abc" option: SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertNil(error);
  XCTAssertNotNil(regex);
  
  SHLMatch* match = [regex search:nil range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(match);
  XCTAssertNotNil(error);
  
  match = [regex search:@"abcd" range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(match);
  XCTAssertNotNil(error);
}

- (void)testSearchRangeWorks {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"abc" option: SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertNil(error);
  XCTAssertNotNil(regex);
  
  SHLMatch* match = [regex search:@"ababcss" range:NSMakeRange(0, 4) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(match);
  XCTAssertNil(error);
  
  match = [regex search:@"ababcss" range:NSMakeRange(0, 5) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNotNil(match);
  XCTAssertNil(error);
  
  match = [regex search:@"ababcss" range:NSMakeRange(1, 5) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNotNil(match);
  XCTAssertNil(error);
  
  match = [regex search:@"ababcss" range:NSMakeRange(2, 5) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNotNil(match);
  XCTAssertNil(error);
  
  match = [regex search:@"ababcss" range:NSMakeRange(3, 4) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(match);
  XCTAssertNil(error);
  
  regex = [[SHLRegex alloc] initWithPatern:@"测试" option: SHLREGEX_OPTION_NONE error:&error];
  
  match = [regex search:@"ab这是一个测试cd" range:NSMakeRange(0, 8) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNotNil(match);
  XCTAssertNil(error);
  
  match = [regex search:@"ab这是一个测试cd" range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(match);
  XCTAssertNil(error);
  
  match = [regex search:@"ab这是一个测试cd" range:NSMakeRange(5, 3) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNotNil(match);
  XCTAssertNil(error);
  
  match = [regex search:@"ab这是一个测试cd" range:NSMakeRange(6, 2) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNotNil(match);
  XCTAssertNil(error);
  
  match = [regex search:@"ab这是一个测试cd" range:NSMakeRange(7, 2) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNil(match);
  XCTAssertNil(error);
}

- (void)testSearchSHLMatchResult {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"abc" option: SHLREGEX_OPTION_NONE error:&error];
  
  SHLMatch* match = [regex search:@"ababcss" range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertEqual(match.range.length, 3);
  XCTAssertEqual(match.range.location, 2);
  XCTAssertEqualObjects(match.string, @"abc");
  
  
  regex = [[SHLRegex alloc] initWithPatern:@"..ss" option: SHLREGEX_OPTION_NONE error:&error];
  match = [regex search:@"ababcss" range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertEqual(match.range.length, 4);
  XCTAssertEqual(match.range.location, 3);
  XCTAssertEqualObjects(match.string, @"bcss");
  
  
  regex = [[SHLRegex alloc] initWithPatern:@"\\w+" option: SHLREGEX_OPTION_NONE error:&error];
  match = [regex search:@"abab css" range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertEqual(match.range.length, 4);
  XCTAssertEqual(match.range.location, 0);
  XCTAssertEqualObjects(match.string, @"abab");
  
}

- (void)testSearchCaptureResult {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@".(.s)s" option: SHLREGEX_OPTION_NONE error:&error];
  SHLMatch* match = [regex search:@"ababcss" range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertEqual([match rangeAtIndex:0].location, 3);
  XCTAssertEqual( [match rangeAtIndex:0].length, 4);
  XCTAssertEqualObjects([match stringAtIndex:0], @"bcss");
  
  XCTAssertEqual([match rangeAtIndex:1].location, 4);
  XCTAssertEqual( [match rangeAtIndex:1].length, 2);
  XCTAssertEqualObjects([match stringAtIndex:1], @"cs");
  
  XCTAssertThrows([match rangeAtIndex:3]);
  XCTAssertThrows([match stringAtIndex:3]);
}

- (void)testMatchSHLMatchResult {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@"abc" option: SHLREGEX_OPTION_NONE error:&error];
  
  SHLMatch* match = [regex match:@"abcssss" range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertEqual(match.range.length, 3);
  XCTAssertEqual(match.range.location, 0);
  XCTAssertEqualObjects(match.string, @"abc");
  
  
  regex = [[SHLRegex alloc] initWithPatern:@"..ab" option: SHLREGEX_OPTION_NONE error:&error];
  match = [regex match:@"ababcss" range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertEqual(match.range.length, 4);
  XCTAssertEqual(match.range.location, 0);
  XCTAssertEqualObjects(match.string, @"abab");
  
  
  regex = [[SHLRegex alloc] initWithPatern:@"\\w+" option: SHLREGEX_OPTION_NONE error:&error];
  match = [regex match:@"abab css" range:NSMakeRange(0, 7) option:SHLREGEX_OPTION_NONE error:&error];
  
  XCTAssertEqual(match.range.length, 4);
  XCTAssertEqual(match.range.location, 0);
  XCTAssertEqualObjects(match.string, @"abab");
}

- (void)testMatchCaptureResult {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@".(.s)s" option: SHLREGEX_OPTION_NONE error:&error];
  SHLMatch* match = [regex search:@"apssdf" range:NSMakeRange(0, 6) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNotNil(match);
  
  XCTAssertEqual([match rangeAtIndex:0].location, 0);
  XCTAssertEqual( [match rangeAtIndex:0].length, 4);
  XCTAssertEqualObjects([match stringAtIndex:0], @"apss");
  
  XCTAssertEqual([match rangeAtIndex:1].location, 1);
  XCTAssertEqual( [match rangeAtIndex:1].length, 2);
  XCTAssertEqualObjects([match stringAtIndex:1], @"ps");
  
  XCTAssertThrows([match rangeAtIndex:3]);
  XCTAssertThrows([match stringAtIndex:3]);
}

- (void)testSHLMatchCount {
  NSError* error;
  SHLRegex* regex = [[SHLRegex alloc] initWithPatern:@".(.s)s" option: SHLREGEX_OPTION_NONE error:&error];
  SHLMatch* match = [regex search:@"apssdf" range:NSMakeRange(0, 6) option:SHLREGEX_OPTION_NONE error:&error];
  XCTAssertNotNil(match);
  
  XCTAssertEqual(match.count, 2);
}
@end
