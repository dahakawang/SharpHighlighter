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

@interface RangedStringTest : XCTestCase

@end

@implementation RangedStringTest

- (void)setUp {
  [super setUp];
}

- (void)testCreateOne {
  RangedString* string = [[RangedString alloc] initWithString:@"abcd"];
  
  XCTAssert([[string toNSString] isEqualToString:@"abcd"]);
}


- (void)testSubString {
  RangedString* string = [[RangedString alloc] initWithString:@"today is a good day, and i'm really happy."];
  
  RangedString* sub1 = [string substringWithRange:NSMakeRange(0, 5)];
  RangedString* sub2 = [string substringWithRange:NSMakeRange(21, 21)];
  
  XCTAssert([[sub1 toNSString] isEqualToString:@"today"]);
  XCTAssert([[sub2 toNSString] isEqualToString:@"and i'm really happy."]);
  
  RangedString* sub3 = [sub2 substringWithRange:NSMakeRange(4, 3)];
  XCTAssert([[sub3 toNSString] isEqualToString:@"i'm"]);
  
}

- (void)testSubStringWillFailWhenRangeOutOfBound {
  RangedString* str = [[RangedString alloc] initWithString:@"this is a test"];
  XCTAssertNoThrow([str substringWithRange:NSMakeRange(2, 10)]);
  XCTAssertThrows([str substringWithRange:NSMakeRange(0, 15)]);
  
  RangedString* str2 = [str substringWithRange:NSMakeRange(2, 10)];
  XCTAssert([[str2 toNSString] isEqualToString:@"is is a te"]);
  XCTAssertThrows([str2 substringWithRange:NSMakeRange(5, 6)]);
  XCTAssertNoThrow([str2 substringWithRange:NSMakeRange(5, 5)]);
}


- (void)testTranslateToGlobalRange {
  RangedString* str = [[RangedString alloc] initWithString:@"this is a test"];
  [str translateToGlobalRange:NSMakeRange(0, 14)];
  
  RangedString* str1 = [str substringWithRange:NSMakeRange(4, 9)];
  XCTAssert([str1 translateToGlobalRange:NSMakeRange(0, 1)].length == 1) ;
  XCTAssert([str1 translateToGlobalRange:NSMakeRange(0, 1)].location == 4) ;
  
}

@end
