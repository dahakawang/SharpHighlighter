//
//  SHLFantomObject.h
//  Pods
//
//  Created by david on 3/14/15.
//
//

#import <Foundation/Foundation.h>

@interface SHLFantomObject : NSObject
@property (readonly, nonatomic, getter=getObject) id object;
- (instancetype)initWithObject: (id)object;
- (BOOL)isNil;
@end
