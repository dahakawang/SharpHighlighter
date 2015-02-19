//
//  RangedStringTest.m
//  SharpHighlighterExample
//
//  Created by david on 2/19/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>
#import <RangedString.h>
#import <RegularExpressionWrapper.h>

@interface RangedStringTest : XCTestCase

@end


BOOL isSameNSRange(NSRange n1, NSRange n2) {
  return n1.location == n2.location && n1.length == n2.length;
}

@implementation RangedStringTest

- (void)setUp {
  [super setUp];
}



- (void)testCreateOne {
  RangedString* string = [[RangedString alloc] initWithString:@"abcd"];
  
  XCTAssert([[string toNSString] isEqualToString:@"abcd"]);
  XCTAssert(isSameNSRange([string toNSRange], NSMakeRange(0, 4)));
}


- (void)testNewSeries {
  RangedString* string = [[RangedString alloc] initWithString:@"today is a good day, and i'm really happy."];
  
  RangedString* sub1 = [string newWithRange:NSMakeRange(0, 5)];
  XCTAssert([[sub1 toNSString] isEqualToString:@"today"]);
  
  
  RangedString* sub2 = [string newWithBegin:6];
  XCTAssert([[sub2 toNSString] isEqualToString:@"is a good day, and i'm really happy."]);
  
  RangedString* sub3 = [string newWithEnd:5];
  XCTAssert([[sub3 toNSString] isEqualToString:@"today"]);
}

- (void)testNewSeriesException {
  RangedString* string = [[RangedString alloc] initWithString:@"today is a good day, and i'm really happy."];
  
  XCTAssertThrows([string newWithRange:NSMakeRange(0, 100)]);
  XCTAssertThrows([string newWithBegin:-1]);
  XCTAssertNoThrow([string newWithBegin:4]);
  XCTAssertThrows([string newWithBegin:100]);
  XCTAssertThrows([string newWithEnd:-1]);
  XCTAssertNoThrow([string newWithEnd:20]);
  XCTAssertThrows([string newWithEnd:120]);
}

- (void)testMatches {
  RangedString* string = [[RangedString alloc] initWithString:@"today is a good day, and i'm really happy."];
  
  RegularExpressionWrapper* regex = [[RegularExpressionWrapper alloc] initWithPattern:@"today" caseInsensitive:NO isGlobal:YES];

  NSTextCheckingResult* result = [string findFirstMatch:regex];
  XCTAssert(result != nil);
  XCTAssert(isSameNSRange(result.range, NSMakeRange(0, 5)));
  
  
}
@end
