//
//  SelectorMatcher.h
//  Pods
//
//  Created by david on 1/21/15.
//
//

#import <Foundation/Foundation.h>

@interface SelectorStylePair : NSObject
@property (nonatomic, strong) NSArray* selector;
@property (nonatomic, strong) NSDictionary* style;

- (instancetype)initWithSelector: (NSString*)selector andStyle: (NSDictionary*)style;
@end


@interface SelectorMatcher : NSObject
- (instancetype)initWithDictionary: (NSDictionary*)dic;
- (NSDictionary*)matchStyleFromClass: (NSString*)className;
@end
