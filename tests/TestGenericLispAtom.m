//
//  TestGenericLispTemplate.m
//  GenericLispTests
//
//  Copyright © 2025 Christopher M. Hanson. All rights reserved.
//  See file COPYING for details.
//

#import "GenericLispTestCase.h"


NS_ASSUME_NONNULL_BEGIN


@interface TestGenericLispTemplate : GenericLispTestCase
@end


@implementation TestGenericLispTemplate

- (void)testCreation
{
    lisp_object_t object = lisp_atom_create_c("ABC");

    lisp_tag_t tag = lisp_object_get_tag(object);
    XCTAssertEqual(lisp_tag_atom, tag);

    const char *name = (const char *)lisp_object_get_raw_value(object);
    XCTAssert(strcmp("ABC", name) == 0);
}

- (void)testPrinting
{
//  lisp_object_t X_char = lisp_char_create('X');
//  lisp_char_t X_char_value = lisp_char_get_value(X_char);
//  lisp_char_print(self.writeStream, X_char_value);
//
//  lisp_object_t Y_char = lisp_char_create('Y');
//  lisp_char_t Y_char_value = lisp_char_get_value(Y_char);
//  lisp_char_print_quoted(self.writeStream, Y_char_value, lisp_nil);
//
//  XCTAssertEqualObjects(@"#\\XY", self.writeBuffer);
}

- (void)testEquality
{
//  lisp_object_t a = lisp_char_create('A');
//  lisp_object_t b = lisp_char_create('B');
//
//  XCTAssert(lisp_char_equal(a, a) != lisp_nil);
//  XCTAssert(lisp_char_equal(b, b) != lisp_nil);
//  XCTAssert(lisp_char_equal(a, b) == lisp_nil);
//  XCTAssert(lisp_char_equal(b, a) == lisp_nil);
}

- (void)testReading
{
    [self.readBuffer setString:@"DEF"];
    lisp_object_t read_object = lisp_read(self.rootEnvironment, self.readStream, lisp_NIL);
    XCTAssertNotEqual(read_object, lisp_NIL);
    XCTAssertEqual(lisp_tag_atom, lisp_object_get_tag(read_object));
    XCTAssertNotEqual(lisp_equal(lisp_atom_create_c("DEF"), read_object), lisp_NIL);
    XCTAssertEqual(0, self.readBuffer.length);

    [self.readBuffer setString:@"GHI \n ; comment \n JKL \n ; comment\n"];
    lisp_object_t read_ghi = lisp_read(self.rootEnvironment, self.readStream, lisp_NIL);
    XCTAssertNotEqual(read_ghi, lisp_NIL);
    XCTAssertNotEqual(lisp_equal(lisp_atom_create_c("GHI"), read_ghi), lisp_NIL);
    lisp_object_t read_jkl = lisp_read(self.rootEnvironment, self.readStream, lisp_NIL);
    XCTAssertNotEqual(read_jkl, lisp_NIL);
    XCTAssertNotEqual(lisp_equal(lisp_atom_create_c("JKL"), read_jkl), lisp_NIL);
}

@end


NS_ASSUME_NONNULL_END
