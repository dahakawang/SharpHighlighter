//
//  ViewController.m
//  SharpHighlighterExample
//
//  Created by david on 1/20/15.
//  Copyright (c) 2015 maxtrixcube. All rights reserved.
//

#import "ViewController.h"
#import "Grammar.h"
#import "GrammarCompiler.h"
#import "HighlightEngine.h"
#import "Theme.h"
#import "HighlightAction.h"
#import "AttributedStringRenderer.h"

@implementation ViewController

- (void)viewDidLoad {
  [super viewDidLoad];
  
  @try {
    NSString* sourceCode = [NSString stringWithContentsOfFile:@"/Users/david/work/Project/TMHighlighter/TMHighlighter/lib/Utility/tmp/test/detect/css/default.txt" encoding:NSUTF8StringEncoding error:NULL];
    if (!sourceCode) {
      @throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"cannot open file" userInfo:NULL];
    }
    
    Grammar* grammar = [[Grammar alloc] initWithJsonFile:@"/Users/david/work/Project/TMHighlighter/TMHighlighter/lib/Themes/css.json"];
    GrammarCompiler* compiler = [[GrammarCompiler alloc] init];
    [compiler compile:grammar];
    
    HighlightEngine* engine = [[HighlightEngine alloc]init];
    NSArray* actions = [engine highlightText:sourceCode withGrammar:grammar ignoreIllegal:NO];
    
    
    Theme* theme = [[Theme alloc] initWithFile:@"/Users/david/work/Project/highlight.js/xcode.theme"];
    
    AttributedStringRenderer* renderer = [[AttributedStringRenderer alloc] init];
    NSAttributedString* highlightedStr =  [renderer renderString:sourceCode withTheme:theme action:actions];
    
    [self.txtView.textStorage setAttributedString:highlightedStr];
    
    
    //  NSData* data = [highlightedStr dataFromRange:NSMakeRange(0, [highlightedStr length]) documentAttributes:@{NSDocumentTypeDocumentAttribute: NSRTFTextDocumentType} error:NULL];
    //  if (!data) {
    //    @throw [NSException exceptionWithName:NSInvalidArgumentException reason:@"cannot generate RTF" userInfo:NULL];
    //  }
    //
    //  [data writeToFile:@"/Users/david/Desktop/code.rtf" atomically:YES];

  }
  @catch (NSException *exception) {
    NSLog(@"%@", [exception callStackSymbols]);
    NSLog(@"%@", [exception reason]);
  }

  

  // Do any additional setup after loading the view.
}

- (void)setRepresentedObject:(id)representedObject {
  [super setRepresentedObject:representedObject];

  // Update the view, if already loaded.
}

@end
