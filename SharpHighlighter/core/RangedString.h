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
- (RangedString*)substringWithRange:(NSRange)range;
- (RangedString*)substringFrom:(NSUInteger)start;
- (RangedString*)substringTo: (NSUInteger)end;
- (NSRange)translateToGlobalRange: (NSRange)range;
- (BOOL)findFirstMatch: (RegularExpressionWrapper*) pattern result: (NSRange*)range;
- (BOOL)findFirstMatch: (RegularExpressionWrapper*) pattern from: (NSUInteger)start result: (NSRange*)range;
- (NSString*)toNSString;
- (NSUInteger)length;
@end
