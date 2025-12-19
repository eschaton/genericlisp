//
//  TestGenericLispEnvironment.m
//  GenericLispTests
//
//  Copyright © 2025 Christopher M. Hanson. All rights reserved.
//  See file COPYING for details.
//

#import "GenericLispTestCase.h"

#import "lisp_string.h"


NS_ASSUME_NONNULL_BEGIN


@interface TestGenericLispEnvironment : GenericLispTestCase
@end


@implementation TestGenericLispEnvironment

- (void)testRootEnvironmentCreation
{
    lisp_object_t environment = lisp_environment_parent(self.rootEnvironment);

    XCTAssertNotEqual(lisp_NIL, environment);
    XCTAssertEqual(lisp_tag_cell, lisp_object_get_tag(environment));
}

- (void)testRootEnvironmentHasT
{
    lisp_object_t environment = lisp_environment_parent(self.rootEnvironment);

    lisp_object_t t_plist = lisp_plist_get(environment, lisp_T);
    XCTAssertNotEqual(NULL, t_plist);
    XCTAssertEqual(lisp_tag_cell, lisp_object_get_tag(t_plist));

    lisp_object_t t_pname = lisp_plist_get(t_plist, lisp_PNAME);
    XCTAssertNotEqual(NULL, t_pname);
    XCTAssertEqual(lisp_tag_string, lisp_object_get_tag(t_pname));

    lisp_string_t t_pname_string = lisp_string_get_value(t_pname);
    lisp_string_print_quoted(self.writeStream, t_pname_string, lisp_NIL);
    XCTAssertEqualObjects(@"T", self.writeBuffer);
    [self clearWriteBuffer];

    lisp_object_t t_apval = lisp_plist_get(t_plist, lisp_APVAL);
    XCTAssertNotEqual(NULL, t_apval);
    XCTAssertEqual(lisp_tag_atom, lisp_object_get_tag(t_apval));
    XCTAssert(lisp_equal(lisp_T, t_apval) != lisp_NIL);
    lisp_print(environment, self.writeStream, t_apval);
    XCTAssertEqualObjects(@"T", self.writeBuffer);
    [self clearWriteBuffer];
}

- (void)testNestedEnvironmentGetsTFromRoot
{
    lisp_object_t environment = lisp_environment_create(self.rootEnvironment);

    lisp_object_t t_symbol = lisp_environment_find_symbol(environment, lisp_T, lisp_T);
    XCTAssertNotEqual(NULL, t_symbol);
    XCTAssertEqual(lisp_tag_cell, lisp_object_get_tag(t_symbol));

    lisp_object_t t_plist = lisp_cell_cdr(t_symbol);
    XCTAssertNotEqual(NULL, t_plist);
    XCTAssertEqual(lisp_tag_cell, lisp_object_get_tag(t_plist));

    lisp_object_t t_pname = lisp_plist_get(t_symbol, lisp_PNAME);
    XCTAssertNotEqual(NULL, t_pname);
    XCTAssertEqual(lisp_tag_string, lisp_object_get_tag(t_pname));

    lisp_string_t t_pname_string = lisp_string_get_value(t_pname);
    lisp_string_print_quoted(self.writeStream, t_pname_string, lisp_NIL);
    XCTAssertEqualObjects(@"T", self.writeBuffer);
    [self clearWriteBuffer];

    lisp_object_t t_apval = lisp_plist_get(t_symbol, lisp_APVAL);
    XCTAssertNotEqual(NULL, t_apval);
    XCTAssertEqual(lisp_tag_atom, lisp_object_get_tag(t_apval));
    XCTAssert(lisp_equal(lisp_T, t_apval) != lisp_NIL);
    lisp_print(environment, self.writeStream, t_apval);
    XCTAssertEqualObjects(@"T", self.writeBuffer);
    [self clearWriteBuffer];
}

@end


NS_ASSUME_NONNULL_END
