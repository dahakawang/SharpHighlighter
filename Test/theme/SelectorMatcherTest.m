//
//  SelectorMatcherTest.m
//  SharpHighlighterExample
//
//  Created by david on 1/21/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>
#import <SelectorMatcher.h>

@interface SelectorMatcherTest : XCTestCase

@end

@implementation SelectorMatcherTest {
  NSDictionary* selectors1;
  NSDictionary* selectors2;
}

- (void)setUp {
  [super setUp];
  selectors1 = @{@"hljs class title":@1,@"hljs class definition":@2,
                 @"hljs css title":@3, @"hljs css *":@4};
  
  selectors2 = @{@"hljs * title":@1};
}

- (void)testFullMatch {
  SelectorMatcher* matcher = [[SelectorMatcher alloc] initWithDictionary:selectors1];
  id s = [matcher matchStyleFromClass:@"hljs class title"];
  XCTAssertEqual(s, @1);
  
  s = [matcher matchStyleFromClass:@"hljs class definition"];
  XCTAssertEqual(s, @2);
}

- (void)testPartialMatch {
  SelectorMatcher* matcher = [[SelectorMatcher alloc] initWithDictionary:selectors1];
  id s = [matcher matchStyleFromClass:@"hljs class ns p title"];
  XCTAssertEqual(s, @1);
  
  s = [matcher matchStyleFromClass:@"over hljs class np definition"];
  XCTAssertEqual(s, @2);
}

- (void)testNoMatch {
  SelectorMatcher* matcher = [[SelectorMatcher alloc] initWithDictionary:selectors1];
  id s = [matcher matchStyleFromClass:@"hljs class title2"];
  XCTAssertTrue(s == nil);
}

- (void)testWildcald {
  SelectorMatcher* matcher = [[SelectorMatcher alloc] initWithDictionary:selectors1];
  id s = [matcher matchStyleFromClass:@"hljs css pick"];
  XCTAssertEqual(s, @4);
}

- (void)testWildcaldInMiddle {
  SelectorMatcher* matcher = [[SelectorMatcher alloc] initWithDictionary:selectors2];
  id s = [matcher matchStyleFromClass:@"hljs np title"];
  XCTAssertEqual(s, @1);
  
  s = [matcher matchStyleFromClass:@"hljs plat os mini title"];
  XCTAssertEqual(s, @1);
}

- (void)testMultipleMatch {
  SelectorMatcher* matcher = [[SelectorMatcher alloc] initWithDictionary:selectors1];
  id s = [matcher matchStyleFromClass:@"hljs css pick"];
  XCTAssertEqual(s, @4);
}

- (void)testWildcaldHasLessPriority {
  SelectorMatcher* matcher = [[SelectorMatcher alloc] initWithDictionary:selectors1];
  id s = [matcher matchStyleFromClass:@"hljs css title"];
  XCTAssertEqual(s, @3);
}


@end
