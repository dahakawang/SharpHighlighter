//
//  RangedString.h
//  SharpHighlighter
//
//  Created by david on 2/19/15.
//
//

#import <Foundation/Foundation.h>
#import "RegularExpressionWrapper.h"

@interface RangedString : NSObject
- (instancetype)initWithString:(NSString*)str;
- (RangedString*)newWithRange: (NSRange)range;
- (RangedString*)newWithBegin:(NSUInteger)begin;
- (RangedString*)newWithEnd: (NSUInteger)end;
- (NSRange)toNSRange;
- (NSString*)toNSString;

- (NSTextCheckingResult*)findFirstMatch: (RegularExpressionWrapper*) pattern;
- (NSArray*)findMatches: (RegularExpressionWrapper*) pattern;
@end
