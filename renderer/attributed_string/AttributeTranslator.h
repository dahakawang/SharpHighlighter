//
//  AttributeTranslator.h
//  TMHighlighter
//
//  Created by david on 1/11/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface AttributeTranslator : NSObject
+ (AttributeTranslator*)sharedTranslator;
- (NSDictionary*)getStyleFromDefinition: (NSDictionary*)definition;
@end

@interface Translator : NSObject
- (BOOL)addDefinition: (NSDictionary*)definitions ToStyle: (NSMutableDictionary*)style;
- (id)translateColorFromString: (NSString*)def;
@end


@interface ColorTranslator : Translator

@end

@interface BackgroundTranslator : Translator

@end

@interface FontTranslator : Translator

@end