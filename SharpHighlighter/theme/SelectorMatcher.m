//
//  SelectorMatcher.m
//  Pods
//
//  Created by david on 1/21/15.
//
//

#import "SelectorMatcher.h"

@implementation SelectorStylePair

- (instancetype)initWithSelector: (NSString*)selector andStyle: (NSDictionary*)style {
  if (self = [super init]) {
    self.selector = [selector componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    self.style = style;
  }
  return self;
}

@end


SelectorStylePair* MakeSelectorStylePair(NSString* selector, NSDictionary*style) {
  return [[SelectorStylePair alloc] initWithSelector:selector andStyle:style];
}

BOOL isSameElem(NSString* selectorElem, NSString* classElem, long* score) {
  if ([selectorElem isEqualToString:@"*"]) {
    (*score)--;
    return YES;
  }
  return [selectorElem isEqualToString:classElem];
}

@implementation SelectorMatcher {
  NSMutableArray* selectors;
}

- (BOOL)isSelector: (NSArray*)selector matchesClass: (NSArray*)className withScore: (long*)score {
  if ([selector count] > [className count]) return NO;
  
  if (!isSameElem(selector[[selector count]-1], className[[className count]-1], score)) return NO;
  
  long selector_index = [selector count] - 1;
  for (long classname_index = [className count] - 1; classname_index >= 0; classname_index--) {
    if (isSameElem(selector[selector_index], className[classname_index], score)) {
      selector_index--;
      if (selector_index < 0) {
        return YES;
      }
    }
  }
  
  return NO;
}


- (NSDictionary*)matchStyleFromClass: (NSString*)className {
  NSArray* _classname = [className componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
  
  NSDictionary* result = nil;
  long score = 0;
  for (SelectorStylePair* pair in selectors) {
    long tmp_score = 0;
    if ([self isSelector:pair.selector matchesClass:_classname withScore: &tmp_score]) {
      if ((result == nil) || (result != nil && tmp_score > score)) {
        result = pair.style;
        score = tmp_score;
      }
    }
  }
  
  return result;
}

- (instancetype)initWithDictionary: (NSDictionary*)dic {
  if (self = [super init]) {
    selectors = [[NSMutableArray alloc] init];
    
    for (NSString* selector in dic) {
      [selectors addObject: MakeSelectorStylePair(selector, dic[selector])];
    }
  }
  return self;
}
@end
