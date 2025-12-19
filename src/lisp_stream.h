/*
    File:       lisp_stream.h

    Copyright:  © 2025 Christopher M. Hanson. All rights reserved.
                See file COPYING for details.
 */

#ifndef __lisp_stream__
#define __lisp_stream__ 1


#include "lisp_types.h"


/**
 The functions used to implement a stream.
 */
typedef struct lisp_stream_functions {

    /** Additional metadata, encapsulated in an interior pointer. */
    lisp_object_t metadata;

    /**
     A function to open the stream for read, write, or both.

     - Returns: The stream itself.
    */
    lisp_object_t (*open)(lisp_object_t stream, lisp_object_t for_read, lisp_object_t for_write);

    /**
     A function to close the stream.

     - Returns: The stream itself.
    */
    lisp_object_t (*close)(lisp_object_t stream);

    /**
     A function to read a character from the stream.

     - Returns: The character read, or `NIL` at end-of-stream.
    */
    lisp_object_t (*read_char)(lisp_object_t stream);

    /**
     A function to "unread" a character from the stream.

     - Returns: The character "unread."
    */
    lisp_object_t (*unread_char)(lisp_object_t stream, lisp_object_t ch);

    /**
     A function to write a character to the stream.

     - Returns: The stream itself.
    */
    lisp_object_t (*write_char)(lisp_object_t stream, lisp_object_t ch);

    /**
     A function to indicate whether the stream is at the end.

     - Returns: `T` or `NIL`.
    */
    lisp_object_t (*eofp)(lisp_object_t stream);

} *lisp_stream_functions_t;


/**
 Flags describing the current state of a Lisp stream.
 */
typedef enum lisp_stream_flags {
    /** Indicates whether the stream is at end. */
    lisp_stream_flags_at_eof    = 0x00000001,

    /** Indicates whether the stream is open for read. */
    lisp_stream_flags_readable  = 0x00000002,

    /** Indicates whether the stream is open for write. */
    lisp_stream_flags_writable  = 0x00000004,
} lisp_stream_flags_t;


/**
 A Lisp stream.

 A _stream_ may be either opened or closed, and may be opened for
 reading, for writing, or both. Only sequential read/write and "unread"
 operations are supported on streams at this time.
 */
typedef struct lisp_stream {
    /**
     The underlying object representing the stream, represented by an
     interior pointer to a set of functions.
     */
    lisp_object_t functions;

    /** Flags describing the stream. */
    lisp_stream_flags_t flags;
} *lisp_stream_t;


/**
 Create a stream from a set of underlying stream functions, encapsulated
 within an interior pointer. All of the stream's flags are unset.
 */
LISP_EXTERN lisp_object_t lisp_stream_create(lisp_object_t functions);

/** Get the stream value of the given Lisp object. */
LISP_EXTERN lisp_stream_t lisp_stream_get_value(lisp_object_t object);

/** Get the stream functions for the given stream object. */
LISP_EXTERN lisp_stream_functions_t lisp_stream_get_functions(lisp_object_t object);

/** Opens a Lisp stream object for use. Returns the stream. */
LISP_EXTERN lisp_object_t lisp_stream_open(lisp_object_t stream, lisp_object_t readable, lisp_object_t writable);

/** Closes a Lisp stream. */
LISP_EXTERN lisp_object_t lisp_stream_close(lisp_object_t stream);

/** Read one character from the given stream. */
LISP_EXTERN lisp_object_t lisp_stream_read_char(lisp_object_t stream);

/** Put a character back on the stream. */
LISP_EXTERN lisp_object_t lisp_stream_unread_char(lisp_object_t stream, lisp_object_t character);

/** Peek a character from the stream. */
LISP_EXTERN lisp_object_t lisp_stream_peek_char(lisp_object_t stream);

/** Write one character to the given stream. */
LISP_EXTERN lisp_object_t lisp_stream_write_char(lisp_object_t stream, lisp_object_t value);

/** Write an entire string to the given stream. */
LISP_EXTERN lisp_object_t lisp_stream_write_string(lisp_object_t stream, lisp_object_t value);

/** Check whether the stream has hit EOF. */
LISP_EXTERN lisp_object_t lisp_stream_eofp(lisp_object_t stream);

/** Check whether the stream is open. */
LISP_EXTERN lisp_object_t lisp_stream_openp(lisp_object_t stream);


/** Prints the stream to the given output stream. */
LISP_EXTERN lisp_object_t lisp_stream_print(lisp_object_t stream, lisp_stream_t stream_value);


/**
 Compare two streams for equality.

 Streams are only equal if they're exactly the same stream, not just
 equivalent to each other.
 */
LISP_EXTERN lisp_object_t lisp_stream_equal(lisp_object_t a, lisp_object_t b);


/**
 Determine the best input stream given a stream designator (which may
 itself be a stream).

 - Returns: `*TERMINAL-IO*` if the designator is `T`, `*STANDARD-INPUT*`
            if the designator is `NIL`, the passed stream if a stream is
            passed, and `NIL` otherwise.
 */
lisp_object_t lisp_stream_best_input_stream(lisp_object_t environment,
                                            lisp_object_t stream_designator);

/**
 Determine the best input stream given a stream designator (which may
 itself be a stream).

 - Returns: `*TERMINAL-IO*` if the designator is `T`,
            `*STANDARD-OUTPUT*` if the designator is `NIL`, the passed
            stream if a stream is passed, and `NIL` otherwise.
 */
lisp_object_t lisp_stream_best_output_stream(lisp_object_t environment,
                                             lisp_object_t stream_designator);


#endif  /* __lisp_stream__ */
