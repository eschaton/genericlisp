//
//  GenericLispTestCase.m
//  GenericLispTests
//
//  Copyright © 2025 Christopher M. Hanson. All rights reserved.
//  See file COPYING for details.
//

#import "GenericLispTestCase.h"

#include "lisp_environment.h"
#include "lisp_memory.h"


NS_ASSUME_NONNULL_BEGIN


static NSMutableString *_writeBuffer = nil;
static lisp_object_t _writeBufferFunctions = NULL;
static lisp_object_t _writeStream = NULL;

static NSMutableString *_readBuffer = nil;
static lisp_object_t _readBufferFunctions = NULL;
static lisp_object_t _readStream = NULL;

static lisp_object_t _rootEnvironment = NULL;



@implementation GenericLispTestCase

- (void)setUpLispEnvironment
{
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        lisp_heap_initialize(1048576);

        _writeBuffer = [[NSMutableString alloc] init];
        _writeBufferFunctions = [self streamFunctionsWritingToString:_writeBuffer];
        _writeStream = lisp_stream_create(_writeBufferFunctions);

        _readBuffer = [[NSMutableString alloc] init];
        _readBufferFunctions = [self streamFunctionsReadingFromString:_readBuffer];
        _readStream = lisp_stream_create(_readBufferFunctions);

        _rootEnvironment = lisp_environment_create_root();

        lisp_stream_open(_readStream, lisp_T, lisp_NIL);
        lisp_environment_set_symbol_value(_rootEnvironment, lisp_STANDARD_OUTPUT, lisp_APVAL, _writeStream, lisp_NIL);
        lisp_stream_open(_writeStream, lisp_NIL, lisp_T);
        lisp_environment_set_symbol_value(_rootEnvironment, lisp_STANDARD_INPUT, lisp_APVAL, _readStream, lisp_NIL);
    });

    [self clearWriteBuffer];
}

- (void)tearDownLispEnvironment
{
#if 0
    lisp_stream_close(_writeStream);
    _writeBuffer = nil;

    lisp_stream_close(_readStream);
    _readBuffer = nil;

    lisp_environment_dispose(_rootEnvironment);

    lisp_heap_finalize();
#endif
}

- (NSMutableString *)writeBuffer
{
    return _writeBuffer;
}

- (lisp_object_t)writeStream
{
    return _writeStream;
}

- (NSMutableString *)readBuffer
{
    return _readBuffer;
}

- (lisp_object_t)readStream
{
    return _readStream;
}

- (lisp_object_t)rootEnvironment
{
    return _rootEnvironment;
}

- (void)setUp
{
    [super setUp];

    [self setUpLispEnvironment];
}

- (void)tearDown
{
    [self.writeBuffer setString:@""];
    [self.readBuffer setString:@""];
    [self tearDownLispEnvironment];

    [super tearDown];
}


// MARK: - lisp_stream_t writing to NSMutableString

static NSMutableString *lisp_stream_mutablestring_output_get_string(lisp_object_t stream)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_interior_t metadata = lisp_interior_get_value(functions->metadata);
    CFMutableStringRef *metadata_string = (CFMutableStringRef *)metadata;
    CFMutableStringRef string = *metadata_string;
    return (__bridge NSMutableString *)string;
}

static lisp_object_t lisp_stream_mutablestring_output_open(lisp_object_t stream, lisp_object_t readable, lisp_object_t writable)
{
    // Just return stream, nothing to do here.
    return stream;
}

static lisp_object_t lisp_stream_mutablestring_output_close(lisp_object_t stream)
{
    // Just return stream, don't deallocate the string.
    return stream;
}

static lisp_object_t lisp_stream_mutablestring_output_read_char(lisp_object_t stream)
{
    // Just return `NIL`, nothing to do here, this is an output stream.
    return lisp_NIL;
}

static lisp_object_t lisp_stream_mutablestring_output_unread_char(lisp_object_t stream, lisp_object_t character)
{
    // Just return the character, nothing to do here, this is an output stream.
    return character;
}

static lisp_object_t lisp_stream_mutablestring_output_write_char(lisp_object_t stream, lisp_object_t value)
{
    NSMutableString *string = lisp_stream_mutablestring_output_get_string(stream);
    lisp_char_t char_value = lisp_char_get_value(value);
    char ch = (char)char_value;
    unichar uch = (unichar)ch;
    NSString *sch = [[NSString alloc] initWithCharactersNoCopy:&uch length:1 freeWhenDone:NO];
    [string appendString:sch];
    return stream;
}

static lisp_object_t lisp_stream_mutablestring_output_eofp(lisp_object_t stream)
{
    // Will never hit EOF so return nil.
    return lisp_NIL;
}

- (lisp_object_t)streamFunctionsWritingToString:(NSMutableString *)string
{
    lisp_stream_functions_t underlying_functions;
    lisp_object_t functions = lisp_interior_create(sizeof(struct lisp_stream_functions), (void **)&underlying_functions);
    underlying_functions->open = lisp_stream_mutablestring_output_open;
    underlying_functions->close = lisp_stream_mutablestring_output_close;
    underlying_functions->read_char = lisp_stream_mutablestring_output_read_char;
    underlying_functions->unread_char = lisp_stream_mutablestring_output_unread_char;
    underlying_functions->write_char = lisp_stream_mutablestring_output_write_char;
    underlying_functions->eofp = lisp_stream_mutablestring_output_eofp;
    CFMutableStringRef *underlying_mutablestring;
    underlying_functions->metadata = lisp_interior_create(sizeof(CFMutableStringRef), (void **)&underlying_mutablestring);
    *underlying_mutablestring = (__bridge_retained CFMutableStringRef)string;
    return (lisp_object_t)functions;
}

- (void)clearWriteBuffer
{
    [self.writeBuffer setString:@""];
}


// MARK: - lisp_stream_t reading from NSMutableString

static NSMutableString *lisp_stream_mutablestring_input_get_string(lisp_object_t stream)
{
    lisp_stream_functions_t functions = lisp_stream_get_functions(stream);
    lisp_interior_t metadata = lisp_interior_get_value(functions->metadata);
    CFMutableStringRef *metadata_string = (CFMutableStringRef *)metadata;
    CFMutableStringRef string = *metadata_string;
    return (__bridge NSMutableString *)string;
}

static lisp_object_t lisp_stream_mutablestring_input_open(lisp_object_t stream, lisp_object_t readable, lisp_object_t writable)
{
    // Just return stream, nothing to do here.
    return stream;
}

static lisp_object_t lisp_stream_mutablestring_input_close(lisp_object_t stream)
{
    // Just return stream, don't deallocate the string.
    return stream;
}

static lisp_object_t lisp_stream_mutablestring_input_read_char(lisp_object_t stream)
{
    NSMutableString *string = lisp_stream_mutablestring_input_get_string(stream);
    if (string.length > 0) {
        unichar ch = [string characterAtIndex:0];
        [string deleteCharactersInRange:NSMakeRange(0, 1)];
        lisp_object_t character = lisp_char_create(ch);
        return character;
    } else {
        return lisp_NIL;
    }
}

static lisp_object_t lisp_stream_mutablestring_input_unread_char(lisp_object_t stream, lisp_object_t character)
{
    NSMutableString *string = lisp_stream_mutablestring_input_get_string(stream);
    lisp_char_t character_value = lisp_char_get_value(character);
    char ch = (char)character_value;
    NSString *ch_string = [[NSString alloc] initWithBytesNoCopy:&ch length:1 encoding:NSUTF8StringEncoding freeWhenDone:NO];
    [string insertString:ch_string atIndex:0];
    return character;
}

static lisp_object_t lisp_stream_mutablestring_input_write_char(lisp_object_t stream, lisp_object_t value)
{
    // Just return sttream, nothing to do here, this is an input stream.
    return stream;
}

static lisp_object_t lisp_stream_mutablestring_input_eofp(lisp_object_t stream)
{
    // We're at EOF if there's nothing in the buffer.
    NSMutableString *string = lisp_stream_mutablestring_input_get_string(stream);
    if (string.length == 0) {
        return lisp_T;
    } else {
        return lisp_NIL;
    }
}

- (lisp_object_t)streamFunctionsReadingFromString:(NSMutableString *)string
{
    lisp_stream_functions_t underlying_functions;
    lisp_object_t functions = lisp_interior_create(sizeof(struct lisp_stream_functions), (void **)&underlying_functions);
    underlying_functions->open = lisp_stream_mutablestring_input_open;
    underlying_functions->close = lisp_stream_mutablestring_input_close;
    underlying_functions->read_char = lisp_stream_mutablestring_input_read_char;
    underlying_functions->unread_char = lisp_stream_mutablestring_input_unread_char;
    underlying_functions->write_char = lisp_stream_mutablestring_input_write_char;
    underlying_functions->eofp = lisp_stream_mutablestring_input_eofp;
    CFMutableStringRef *underlying_mutablestring;
    underlying_functions->metadata = lisp_interior_create(sizeof(CFMutableStringRef), (void **)&underlying_mutablestring);
    *underlying_mutablestring = (__bridge_retained CFMutableStringRef)string;
    return (lisp_object_t)functions;
}

- (void)clearReadBuffer
{
    [self.readBuffer setString:@""];
}

@end


NS_ASSUME_NONNULL_END
