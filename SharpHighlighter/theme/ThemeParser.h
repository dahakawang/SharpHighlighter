//
//  ThemeParser.h
//  TMHighlighter
//
//  Created by david on 1/8/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface ThemeParser : NSObject
+ (NSDictionary*)parseThemeData: (NSString*)aString;
@end
