//
//  TestGenericLispPlist.m
//  GenericLispTests
//
//  Copyright © 2025 Christopher M. Hanson. All rights reserved.
//  See file COPYING for details.
//

#import "GenericLispTestCase.h"


NS_ASSUME_NONNULL_BEGIN


@interface TestGenericLispPlist : GenericLispTestCase
@end


@implementation TestGenericLispPlist {
    lisp_object_t _A, _B, _C, _D;
    lisp_object_t _ab, _cd;
    lisp_object_t _plist;
}

- (void)setUp
{
    [super setUp];

    /*
     Create a plist of the form:

         ((A . B) . ((C . D))

     Each CAR in the plist is a (key . value) pair.
     Each CDR in the plist is a plist or NIL to terminate it.
     */

    _A = lisp_atom_create_c("A");
    _B = lisp_atom_create_c("B");
    _C = lisp_atom_create_c("C");
    _D = lisp_atom_create_c("D");

    _ab = lisp_cell_cons(_A, _B);
    _cd = lisp_cell_cons(_C, _D);
    _plist = lisp_plist_create(_ab, _cd, NULL);
}

- (void)testCreation
{
    XCTAssertEqual(lisp_tag_cell, lisp_object_get_tag(_plist));
}

- (void)testPrinting
{
    lisp_print(self.rootEnvironment, self.writeStream, _plist);
    XCTAssertEqualObjects(@"((A . B) (C . D))", self.writeBuffer);
    [self clearWriteBuffer];
}

- (void)testSimpleSuccessfulRetrieval
{
    lisp_object_t a_value = lisp_plist_get(_plist, _A);
    XCTAssertEqual(a_value, _B);

    lisp_object_t c_value = lisp_plist_get(_plist, _C);
    XCTAssertEqual(c_value, _D);
}

- (void)testSimpleFailedRetrieval
{
    lisp_object_t b_value = lisp_plist_get(_plist, _B);
    XCTAssertEqual(b_value, lisp_NIL);
}

@end


NS_ASSUME_NONNULL_END
