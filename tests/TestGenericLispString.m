//
//  TestGenericLispString.m
//  GenericLispTests
//
//  Copyright © 2025 Christopher M. Hanson. All rights reserved.
//  See file COPYING for details.
//

#import "GenericLispTestCase.h"

#import "lisp_string.h"

#import "lisp_interior.h"


NS_ASSUME_NONNULL_BEGIN


@interface TestGenericLispString : GenericLispTestCase
@end


@implementation TestGenericLispString

- (void)testCreation
{
    lisp_object_t object = lisp_string_create_c("ABC");
    lisp_tag_t tag = lisp_object_get_tag(object);
    XCTAssertEqual(lisp_tag_string, tag);

    lisp_string_t string = lisp_string_get_value(object);
    XCTAssert(string != NULL);

    XCTAssert(string->chars != NULL);
    XCTAssertEqual(16, string->capacity);
    XCTAssertEqual(3, string->length);

    XCTAssertEqual(lisp_tag_interior, lisp_object_get_tag(string->chars));

    lisp_object_t *chars = lisp_interior_get_value(string->chars);
    XCTAssertEqual(lisp_char_create('A'), chars[0]);
    XCTAssertEqual(lisp_char_create('B'), chars[1]);
    XCTAssertEqual(lisp_char_create('C'), chars[2]);
}

- (void)testPrinting
{
    lisp_object_t object = lisp_string_create_c("XYZ");

    lisp_string_t string_value = lisp_string_get_value(object);
    lisp_string_print(self.writeStream, string_value);

    XCTAssertEqualObjects(@"XYZ", self.writeBuffer);
}

- (void)testEquality
{
    lisp_object_t abc = lisp_string_create_c("ABC");
    lisp_object_t xyz = lisp_string_create_c("XYZ");

    XCTAssert(lisp_string_equal(abc, abc) != lisp_NIL);
    XCTAssert(lisp_string_equal(xyz, xyz) != lisp_NIL);
    XCTAssert(lisp_string_equal(abc, xyz) == lisp_NIL);
    XCTAssert(lisp_string_equal(xyz, abc) == lisp_NIL);

    lisp_object_t abc2 = lisp_string_create_c("ABC");
    lisp_object_t xyz2 = lisp_string_create_c("XYZ");

    XCTAssertNotEqual(abc, abc2);
    XCTAssertNotEqual(xyz, xyz2);

    XCTAssert(lisp_string_equal(abc, abc2) != lisp_NIL);
    XCTAssert(lisp_string_equal(xyz, xyz2) != lisp_NIL);

    lisp_object_t ab = lisp_string_create_c("AB");

    XCTAssert(lisp_string_equal(abc, ab) == lisp_NIL);
}

- (void)testReading
{
    [self.readBuffer setString:@"; comment\n   \"ABC\""];
    lisp_object_t read_object = lisp_read(self.rootEnvironment, self.readStream, lisp_NIL);
    XCTAssertNotEqual(read_object, lisp_NIL);
    XCTAssertEqual(lisp_tag_string, lisp_object_get_tag(read_object));
    XCTAssert(lisp_equal(lisp_string_create_c("ABC"), read_object) != lisp_NIL);
    XCTAssertEqualObjects(@"", self.readBuffer);
}

- (void)testReallocation
{
    lisp_object_t *chars_buffer;
    lisp_object_t chars_interior = lisp_interior_create(sizeof(lisp_object_t) * 1, (void **)&chars_buffer);
    chars_buffer[0] = lisp_char_create('A');
    lisp_object_t string = lisp_string_create(chars_interior, 1, 1);
    lisp_string_t string_value = lisp_string_get_value(string);
    XCTAssertEqual(1, string_value->capacity);
    XCTAssertEqual(1, string_value->length);
    XCTAssertEqual(chars_interior, string_value->chars);
    lisp_string_append_char(string, lisp_char_create('B'));
    XCTAssertEqual(17, string_value->capacity);
    XCTAssertEqual(2, string_value->length);
    XCTAssertNotEqual(chars_interior, string_value->chars);
}

@end


NS_ASSUME_NONNULL_END
