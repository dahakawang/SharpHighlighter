//
//  SHLFantomObjectTest.m
//  SharpHighlighter
//
//  Created by david on 3/14/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <XCTest/XCTest.h>
#import "SHLFantomObject.h"

@interface DeallocIndicator : NSObject
@property (nonatomic) bool dealloated;
- (instancetype)init;
@end

@implementation DeallocIndicator
- (instancetype)init {
  if ([super init] == nil) return nil;
  _dealloated = NO;
  return self;
}

@end


@interface ConcreteClass : NSObject
- (instancetype)initWithIndicator: (DeallocIndicator*)indicator;
- (NSString*)speak;
- (void)work;
@end

@implementation ConcreteClass {
  DeallocIndicator* _inidcator;
}

- (instancetype)initWithIndicator: (DeallocIndicator*)indicator {
  if ([super init] == nil) return nil;
  
  _inidcator = indicator;
  return self;
}

- (NSString *)speak {
  return @"Hello";
}

- (void)work {
  int sum = 0;
  for (int i = 0; i < 10000; i++) {
    sum += i;
  }
}

- (void)dealloc {
  _inidcator.dealloated = YES;
}

@end





@interface SHLFantomObjectTest : XCTestCase

@end

@implementation SHLFantomObjectTest

- (void)testCanInvoke {
  DeallocIndicator* ind = [[DeallocIndicator alloc] init];
  ConcreteClass* con = [[ConcreteClass alloc] initWithIndicator:ind];
  id fantom = [[SHLFantomObject alloc] initWithObject:con];
  
  NSString* result = [fantom speak];
  XCTAssertEqualObjects(result, @"Hello");
}

- (void)testCanInvokeWrongMethod {
  DeallocIndicator* ind = [[DeallocIndicator alloc] init];
  ConcreteClass* con = [[ConcreteClass alloc] initWithIndicator:ind];
  id fantom = [[SHLFantomObject alloc] initWithObject:con];
  
  XCTAssertThrows([fantom substringFromIndex:1]);
}

- (void)testNullInvoke {
  id fantom = [[SHLFantomObject alloc] initWithObject:nil];
  
  [fantom speak];
}

- (void)testNotRetained {
  id fantom = nil;
  DeallocIndicator* ind = [[DeallocIndicator alloc] init];
  
  @autoreleasepool {
    ConcreteClass* con = [[ConcreteClass alloc] initWithIndicator:ind];
    fantom = [[SHLFantomObject alloc] initWithObject:con];
    
    XCTAssertEqualObjects([con speak], @"Hello");
    XCTAssertEqualObjects([fantom speak], @"Hello");
    XCTAssertNotNil([fantom getObject]);
    XCTAssertFalse([fantom isNil]);
  }
  
  XCTAssertTrue(ind.dealloated);
  XCTAssertNil([fantom getObject]);
  XCTAssertTrue([fantom isNil]);
  
  XCTAssertNoThrow([fantom speak]);
}

#define MAX_TIME 10000
- (void)testSmallFunctionOverheadForFantom {
  DeallocIndicator* ind = [[DeallocIndicator alloc] init];
  ConcreteClass* con = [[ConcreteClass alloc] initWithIndicator:ind];
  id fantom = [[SHLFantomObject alloc] initWithObject:con];
  
  
  [self measureBlock:^{
    for (int i = 0; i < MAX_TIME; i++) {
      [fantom speak];
    }
  }];
}

- (void)testSmallFunctionOverheadNoFantom {
  DeallocIndicator* ind = [[DeallocIndicator alloc] init];
  ConcreteClass* con = [[ConcreteClass alloc] initWithIndicator:ind];
  
  [self measureBlock:^{
    for (int i = 0; i < MAX_TIME; i++) {
      [con speak];
    }
  }];
}


- (void)testBusyFunctionOverheadForFantom {
  DeallocIndicator* ind = [[DeallocIndicator alloc] init];
  ConcreteClass* con = [[ConcreteClass alloc] initWithIndicator:ind];
  id fantom = [[SHLFantomObject alloc] initWithObject:con];
  
  
  [self measureBlock:^{
    for (int i = 0; i < MAX_TIME; i++) {
      [fantom work];
    }
  }];
}

- (void)testBusyFunctionOverheadNoFantom {
  DeallocIndicator* ind = [[DeallocIndicator alloc] init];
  ConcreteClass* con = [[ConcreteClass alloc] initWithIndicator:ind];
  
  
  [self measureBlock:^{
    for (int i = 0; i < MAX_TIME; i++) {
      [con work];
    }
  }];
}
@end
