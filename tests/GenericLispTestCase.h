//
//  GenericLispTestCase.h
//  GenericLispTests
//
//  Copyright © 2025 Christopher M. Hanson. All rights reserved.
//  See file COPYING for details.
//

#import <XCTest/XCTest.h>

#include "genericlisp.h"


NS_ASSUME_NONNULL_BEGIN


@interface GenericLispTestCase: XCTestCase

@property (readonly) lisp_object_t rootEnvironment;

- (lisp_object_t)streamFunctionsWritingToString:(NSMutableString *)string;

@property (readonly) lisp_object_t writeStream;
@property (readonly, copy) NSMutableString *writeBuffer;
- (void)clearWriteBuffer;

@property (readonly) lisp_object_t readStream;
@property (readonly, copy) NSMutableString *readBuffer;
- (void)clearReadBuffer;

@end


NS_ASSUME_NONNULL_END
