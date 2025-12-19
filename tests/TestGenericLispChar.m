//
//  TestGenericLispChar.m
//  GenericLispTests
//
//  Copyright © 2025 Christopher M. Hanson. All rights reserved.
//  See file COPYING for details.
//

#import "GenericLispTestCase.h"


NS_ASSUME_NONNULL_BEGIN


@interface TestGenericLispChar : GenericLispTestCase
@end


@implementation TestGenericLispChar

- (void)testCreation
{
    lisp_object_t object = lisp_char_create('A');

    lisp_tag_t tag = lisp_object_get_tag(object);
    XCTAssertEqual(lisp_tag_char, tag);

    uintptr_t expectedValue = ((uintptr_t) 'A') << 4;
    uintptr_t value = lisp_object_get_raw_value(object);
    XCTAssertEqual(expectedValue, value);
}

- (void)testPrinting
{
    lisp_object_t X_char = lisp_char_create('X');
    lisp_char_t X_char_value = lisp_char_get_value(X_char);
    lisp_char_print(self.writeStream, X_char_value);

    lisp_object_t Y_char = lisp_char_create('Y');
    lisp_char_t Y_char_value = lisp_char_get_value(Y_char);
    lisp_char_print_quoted(self.writeStream, Y_char_value, lisp_NIL);

    XCTAssertEqualObjects(@"XY", self.writeBuffer);
}

- (void)testEquality
{
    lisp_object_t a = lisp_char_create('A');
    lisp_object_t b = lisp_char_create('B');

    XCTAssert(lisp_char_equal(a, a) != lisp_NIL);
    XCTAssert(lisp_char_equal(b, b) != lisp_NIL);
    XCTAssert(lisp_char_equal(a, b) == lisp_NIL);
    XCTAssert(lisp_char_equal(b, a) == lisp_NIL);
}

- (void)testReading
{
    [self.readBuffer setString:@"#\\A"];
    lisp_object_t read_object = lisp_read(self.rootEnvironment, self.readStream, lisp_NIL);
    XCTAssertNotEqual(read_object, lisp_NIL);
    XCTAssertEqual(lisp_tag_char, lisp_object_get_tag(read_object));
    XCTAssertEqual(lisp_char_create('A'), read_object);
    XCTAssertEqualObjects(@"", self.readBuffer);
}

@end


NS_ASSUME_NONNULL_END
