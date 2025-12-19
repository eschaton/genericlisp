//
//  TestGenericLispCell.m
//  GenericLispTests
//
//  Copyright © 2025 Christopher M. Hanson. All rights reserved.
//  See file COPYING for details.
//

#import "GenericLispTestCase.h"

#import "lisp_built_in_sforms.h"
#import "lisp_reading.h"


NS_ASSUME_NONNULL_BEGIN


@interface TestGenericLispCell : GenericLispTestCase
@end


@implementation TestGenericLispCell

- (void)testCreation
{
    lisp_object_t foo = lisp_string_create_c("foo");
    lisp_object_t bar = lisp_string_create_c("bar");
    lisp_object_t object = lisp_cell_cons(foo, bar);

    lisp_tag_t tag = lisp_object_get_tag(object);
    XCTAssertEqual(lisp_tag_cell, tag);

    uintptr_t value = lisp_object_get_raw_value(object);
    XCTAssertNotEqual(NULL, (void *)value);

    XCTAssertEqual(foo, lisp_cell_car(object));
    XCTAssertEqual(bar, lisp_cell_cdr(object));
}

- (void)testReplacement
{
    lisp_object_t a = lisp_string_create_c("a");
    lisp_object_t b = lisp_string_create_c("b");
    lisp_object_t object = lisp_cell_cons(a, b);

    lisp_object_t x = lisp_string_create_c("x");
    lisp_object_t xobject = lisp_cell_rplaca(object, x);
    XCTAssertEqual(object, xobject);
    XCTAssertEqual(lisp_cell_car(xobject), x);
    XCTAssertEqual(lisp_cell_cdr(xobject), b);

    lisp_object_t y = lisp_string_create_c("y");
    lisp_object_t yobject = lisp_cell_rplacd(object, y);
    XCTAssertEqual(object, yobject);
    XCTAssertEqual(lisp_cell_car(yobject), x);
    XCTAssertEqual(lisp_cell_cdr(yobject), y);
}

- (void)testPrinting
{
    lisp_object_t a = lisp_string_create_c("a");
    lisp_object_t b = lisp_string_create_c("b");
    lisp_object_t object = lisp_cell_cons(a, b);

    lisp_print(self.rootEnvironment, self.writeStream, object);

    XCTAssertEqualObjects(@"(\"a\" . \"b\")", self.writeBuffer);
}

- (void)testListCreation
{
    lisp_object_t A = lisp_char_create('A');
    lisp_object_t B = lisp_char_create('B');
    lisp_object_t C = lisp_char_create('C');

    lisp_object_t list = lisp_cell_list(A, B, C, lisp_NIL);
    XCTAssertNotEqual((lisp_object_t) NULL, list);

    lisp_object_t list_by_cons = lisp_cell_cons(A, lisp_cell_cons(B, lisp_cell_cons(C, lisp_NIL)));
    XCTAssertNotEqual((lisp_object_t) NULL, list_by_cons);

    lisp_object_t first = lisp_cell_car(list);
    lisp_object_t first_rest = lisp_cell_cdr(list);
    XCTAssertEqual(A, first);

    lisp_object_t second = lisp_cell_car(first_rest);
    lisp_object_t second_rest = lisp_cell_cdr(first_rest);
    XCTAssertEqual(B, second);

    lisp_object_t third = lisp_cell_car(second_rest);
    lisp_object_t third_rest = lisp_cell_cdr(second_rest);
    XCTAssertEqual(C, third);

    lisp_object_t fourth = lisp_cell_car(third_rest);
    XCTAssertEqual(lisp_NIL, fourth);
}

- (void)testListPrinting
{
    lisp_object_t list = lisp_cell_list(lisp_atom_create_c("X"), lisp_atom_create_c("Y"), lisp_atom_create_c("Z"), lisp_NIL);

    lisp_print(self.rootEnvironment, self.writeStream, list);

    XCTAssertEqualObjects(@"(X Y Z)", self.writeBuffer);
}

- (void)testListPrintingStructural
{
    lisp_object_t list = lisp_cell_list(lisp_atom_create_c("X"), lisp_atom_create_c("Y"), lisp_atom_create_c("Z"), lisp_NIL);

    lisp_cell_t cell_value = lisp_cell_get_value(list);
    lisp_cell_print_dotted(self.rootEnvironment, self.writeStream, cell_value, lisp_NIL);

    XCTAssertEqualObjects(@"(X . (Y . (Z . NIL)))", self.writeBuffer);
}

- (void)testListReading
{
    lisp_object_t environment = self.rootEnvironment;
    lisp_object_t stream = self.readStream;
    [self.readBuffer setString:@"(A B)"];

    lisp_object_t read_object = lisp_read(environment, stream, lisp_NIL);
    XCTAssertNotEqual(lisp_NIL, read_object);
    XCTAssertEqual(lisp_tag_cell, lisp_object_get_tag(read_object));

    XCTAssertNotEqual(lisp_NIL, lisp_equal(lisp_atom_create_c("A"), lisp_cell_car(read_object)));
    XCTAssertNotEqual(lisp_NIL, lisp_equal(lisp_atom_create_c("B"), lisp_cell_car(lisp_cell_cdr(read_object))));
    XCTAssertEqual(lisp_NIL, lisp_cell_car(lisp_cell_cdr(lisp_cell_cdr(read_object))));
}

- (void)testListReadingNested
{
    lisp_object_t environment = self.rootEnvironment;
    lisp_object_t stream = self.readStream;
    [self.readBuffer setString:@"(A (B C) D)"];

    lisp_object_t read_object = lisp_read(environment, stream, lisp_NIL);
    XCTAssertNotEqual(lisp_NIL, read_object);
    XCTAssertEqual(lisp_tag_cell, lisp_object_get_tag(read_object));

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t B = lisp_atom_create_c("B");
    lisp_object_t C = lisp_atom_create_c("C");
    lisp_object_t D = lisp_atom_create_c("D");

    lisp_object_t matching_list = lisp_cell_list(A, lisp_cell_list(B, C, lisp_NIL), D, lisp_NIL);
    XCTAssertNotEqual(lisp_NIL, lisp_equal(read_object, matching_list));

    lisp_object_t matching_cells = lisp_cell_cons(A, lisp_cell_cons(lisp_cell_cons(B, lisp_cell_cons(C, lisp_NIL)), lisp_cell_cons(D, lisp_NIL)));
    XCTAssertNotEqual(lisp_NIL, lisp_equal(read_object, matching_cells));
}

- (void)testListReadingAtomInterning
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);
    lisp_object_t stream = self.readStream;
    [self.readBuffer setString:@"(A A))"];

    lisp_object_t read_list = lisp_read(environment, stream, lisp_NIL);
    XCTAssertNotEqual(lisp_NIL, read_list);
    XCTAssertEqual(lisp_tag_cell, lisp_object_get_tag(read_list));

    lisp_object_t first_A = lisp_cell_car(read_list);
    lisp_object_t second_A = lisp_cell_car(lisp_cell_cdr(read_list));
    XCTAssertEqual(first_A, second_A, @"The A atoms should be EQ, not just EQUAL.");
}

- (void)testReadingQuotedAtom
{
    lisp_object_t environment = self.rootEnvironment;

    [self.readBuffer setString:@"'A"];
    lisp_object_t read_object = lisp_read(environment, self.readStream, lisp_NIL);
    XCTAssertNotEqual(read_object, lisp_NIL);
    XCTAssertEqual(lisp_tag_cell, lisp_object_get_tag(read_object));

    lisp_object_t A = lisp_atom_create_c("A");

    lisp_object_t quote = lisp_cell_car(read_object);
    lisp_object_t atom = lisp_cell_car(lisp_cell_cdr(read_object));
    XCTAssertEqual(lisp_T, lisp_equal(quote, lisp_symbol_QUOTE));
    XCTAssertEqual(lisp_T, lisp_equal(atom, A));
}

- (void)testReadingQuotedList
{
    lisp_object_t environment = self.rootEnvironment;

    [self.readBuffer setString:@"'(A B)"];
    lisp_object_t read_object = lisp_read(environment, self.readStream, lisp_NIL);
    XCTAssertNotEqual(read_object, lisp_NIL);
    XCTAssertEqual(lisp_tag_cell, lisp_object_get_tag(read_object));

    lisp_object_t A = lisp_atom_create_c("A");
    lisp_object_t B = lisp_atom_create_c("B");

    lisp_object_t quote = lisp_cell_car(read_object);
    lisp_object_t cell_A = lisp_cell_car(lisp_cell_cdr(read_object));
    lisp_object_t atom_A = lisp_cell_car(cell_A);
    lisp_object_t atom_B = lisp_cell_car(lisp_cell_cdr(cell_A));

    XCTAssertEqual(lisp_T, lisp_equal(quote, lisp_symbol_QUOTE));
    XCTAssertEqual(lisp_T, lisp_equal(atom_A, A));
    XCTAssertEqual(lisp_T, lisp_equal(atom_B, B));
}

@end


NS_ASSUME_NONNULL_END
