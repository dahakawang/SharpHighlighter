//
//  Renderer.h
//  TMHighlighter
//
//  Created by david on 1/18/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "theme/theme.h"
#import "core/HighlightAction.h"

@interface Renderer : NSObject
- (id)renderString: (NSString*)sourceCode withTheme: (Theme*)theme action: (NSArray*)action;
@end
