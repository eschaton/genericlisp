//
//  TestGenericLispFixnum.m
//  GenericLispTests
//
//  Copyright © 2025 Christopher M. Hanson. All rights reserved.
//  See file COPYING for details.
//

#import "GenericLispTestCase.h"


NS_ASSUME_NONNULL_BEGIN


@interface TestGenericLispFixnum : GenericLispTestCase
@end


@implementation TestGenericLispFixnum {
    NSString *_min_fixnum_str;
    lisp_fixnum_t _min_fixnum_value;

    NSString *_max_fixnum_str;
    lisp_fixnum_t _max_fixnum_value;
}

- (void)setUp
{
    [super setUp];

#if __L6P4__
    _min_fixnum_str = @"-576460752303423488";
    _min_fixnum_value = -576460752303423488;

    _max_fixnum_str = @"+576460752303423487";
    _max_fixnum_value = +576460752303423487;
#else
    _min_fixnum_str = @"-134217728";
    _min_fixnum_value = -134217728;

    _max_fixnum_str = @"+134217727";
    _max_fixnum_value = +134217727;
#endif
}

- (void)testCreationMinFixnum
{
    lisp_object_t min_fixnum = lisp_fixnum_create(_min_fixnum_value);
    XCTAssertNotEqual(lisp_NIL, min_fixnum);

    XCTAssertEqual(lisp_tag_fixnum, lisp_object_get_tag(min_fixnum));

    lisp_fixnum_t extracted_min_fixnum = lisp_fixnum_get_value(min_fixnum);
    XCTAssertEqual(_min_fixnum_value, extracted_min_fixnum);
}

- (void)testCreationMaxFixnum
{
    lisp_object_t max_fixnum = lisp_fixnum_create(_max_fixnum_value);
    XCTAssertNotEqual(lisp_NIL, max_fixnum);

    XCTAssertEqual(lisp_tag_fixnum, lisp_object_get_tag(max_fixnum));

    lisp_fixnum_t extracted_max_fixnum = lisp_fixnum_get_value(max_fixnum);
    XCTAssertEqual(_max_fixnum_value, extracted_max_fixnum);
}

- (void)testPrinting
{
    lisp_object_t object = lisp_fixnum_create(1234567890);

    lisp_fixnum_t fixnum_value = lisp_fixnum_get_value(object);
    lisp_fixnum_print(self.writeStream, fixnum_value);

    XCTAssertEqualObjects(@"1234567890", self.writeBuffer);
}

- (void)testPrintingMaxFixnum
{
    lisp_object_t max_fixnum = lisp_fixnum_create(_max_fixnum_value);

    lisp_print(self.rootEnvironment, self.writeStream, max_fixnum);

    // Printing elides the + prefix that the reader supports (but does not require).
    NSString *max_fixnum_str = [_max_fixnum_str substringFromIndex:1];

    XCTAssertEqualObjects(max_fixnum_str, self.writeBuffer);
}

- (void)testPrintingMinFixnum
{
    lisp_object_t min_fixnum = lisp_fixnum_create(_min_fixnum_value);

    lisp_print(self.rootEnvironment, self.writeStream, min_fixnum);

    XCTAssertEqualObjects(_min_fixnum_str, self.writeBuffer);
}

- (void)testEquality
{
#if __LP64__
    lisp_object_t a = lisp_fixnum_create(0x07665544332211AA);
    lisp_object_t b = lisp_fixnum_create(0x0AA1122334455667);
#else
    lisp_object_t a = lisp_fixnum_create(0x07665544);
    lisp_object_t b = lisp_fixnum_create(0x04455667);
#endif

    XCTAssert(lisp_fixnum_equal(a, a) != lisp_NIL);
    XCTAssert(lisp_fixnum_equal(b, b) != lisp_NIL);
    XCTAssert(lisp_fixnum_equal(a, b) == lisp_NIL);
    XCTAssert(lisp_fixnum_equal(b, a) == lisp_NIL);
}

- (void)testReadingMaxFixnum
{
    lisp_object_t environment = self.rootEnvironment;

    [self.readBuffer setString:_max_fixnum_str];
    lisp_object_t read_object = lisp_read(environment, self.readStream, lisp_NIL);
    XCTAssertNotEqual(read_object, lisp_NIL);
    XCTAssertEqual(lisp_tag_fixnum, lisp_object_get_tag(read_object));
    XCTAssertEqual(_max_fixnum_value, lisp_fixnum_get_value(read_object));
}

- (void)testReadingMinFixnum
{
    lisp_object_t environment = self.rootEnvironment;

    [self.readBuffer setString:_min_fixnum_str];
    lisp_object_t read_object = lisp_read(environment, self.readStream, lisp_NIL);
    XCTAssertNotEqual(read_object, lisp_NIL);
    XCTAssertEqual(lisp_tag_fixnum, lisp_object_get_tag(read_object));
    XCTAssertEqual(_min_fixnum_value, lisp_fixnum_get_value(read_object));
}

@end


NS_ASSUME_NONNULL_END
