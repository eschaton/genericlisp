//
//  TestGenericLispStream.m
//  GenericLispTests
//
//  Copyright © 2025 Christopher M. Hanson. All rights reserved.
//  See file COPYING for details.
//

#import "GenericLispTestCase.h"


NS_ASSUME_NONNULL_BEGIN


@interface TestGenericLispStream : GenericLispTestCase
@end


@implementation TestGenericLispStream {
    NSMutableString *_testWriteBuffer;
    lisp_object_t _testWriteBufferFunctions;
}

- (void)setUp
{
    [super setUp];

    _testWriteBuffer = [[NSMutableString alloc] init];
    _testWriteBufferFunctions = [self streamFunctionsWritingToString:_testWriteBuffer];
}

- (void)tearDown
{
    _testWriteBuffer = nil;
    _testWriteBufferFunctions = NULL;

    [super tearDown];
}

- (void)testCreation
{
    lisp_object_t stream = lisp_stream_create(_testWriteBufferFunctions);
    XCTAssert(stream != NULL);

    lisp_tag_t tag = lisp_object_get_tag(stream);
    XCTAssertEqual(lisp_tag_stream, tag);

    uintptr_t value = lisp_object_get_raw_value(stream);
    XCTAssertNotEqual(NULL, (void *)value);
}

- (void)testWritingCharacters
{
    lisp_object_t stream = lisp_stream_create(_testWriteBufferFunctions);
    XCTAssertNotEqual(NULL, (void *)stream);

    lisp_object_t open_result = lisp_stream_open(stream, lisp_NIL, lisp_T);
    XCTAssertNotEqual(lisp_NIL, open_result);

    lisp_stream_write_char(stream, lisp_char_create('A'));
    lisp_stream_write_char(stream, lisp_char_create('B'));
    lisp_stream_write_char(stream, lisp_char_create('C'));

    XCTAssertEqualObjects(@"ABC", _testWriteBuffer);

    lisp_stream_close(stream);
}

- (void)testWritingString
{
    lisp_object_t stream = lisp_stream_create(_testWriteBufferFunctions);
    XCTAssertNotEqual(NULL, (void *)stream);

    lisp_object_t open_result = lisp_stream_open(stream, lisp_NIL, lisp_T);
    XCTAssertNotEqual(lisp_NIL, open_result);

    lisp_stream_write_string(stream, lisp_string_create_c("ABC"));

    XCTAssertEqualObjects(@"ABC", _testWriteBuffer);

    lisp_stream_close(stream);
}

- (void)testPrintingInterior
{
    lisp_object_t environment = self.rootEnvironment;
    lisp_object_t output_stream = self.writeStream;

    lisp_interior_t interior_value = NULL;
    lisp_object_t interior = lisp_interior_create(16, (void **)&interior_value);
    lisp_print(environment, output_stream, interior);

    NSString *expected = [NSString stringWithFormat:@"#<INTERIOR 0x%llX>", (unsigned long long)interior_value];
    XCTAssertEqualObjects(expected, self.writeBuffer);
}

- (void)testPrintingStructure
{
    lisp_object_t environment = self.rootEnvironment;
    lisp_object_t output_stream = self.writeStream;

    lisp_object_t structure = lisp_struct_create(stdin, sizeof(FILE *), 0);
    lisp_struct_t structure_value = lisp_struct_get_value(structure);
    lisp_print(environment, output_stream, structure);

    NSString *expected = [NSString stringWithFormat:@"#<STRUCT 0x%llX>", (unsigned long long)structure_value];
    XCTAssertEqualObjects(expected, self.writeBuffer);
}

@end


NS_ASSUME_NONNULL_END
