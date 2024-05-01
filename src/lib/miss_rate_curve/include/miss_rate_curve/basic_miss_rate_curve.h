#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <glib.h> // for MIN()

#include "histogram/basic_histogram.h"
#include "histogram/fractional_histogram.h"

struct BasicMissRateCurve {
    double *miss_rate;
    uint64_t length;
};

bool
MissRateCurve__init_from_fractional_histogram(
    struct BasicMissRateCurve *me,
    struct FractionalHistogram *histogram);

bool
MissRateCurve__init_from_basic_histogram(
    struct BasicMissRateCurve *me,
    struct BasicHistogram *histogram);

/// NOTE    The arguments are in a terrible order. Sorry.
bool
MissRateCurve__init_from_parda_histogram(struct BasicMissRateCurve *me,
                                                 uint64_t histogram_length,
                                                 unsigned int *histogram,
                                                 uint64_t histogram_total,
                                                 uint64_t false_infinities);

bool
MissRateCurve__init_from_file(struct BasicMissRateCurve *me,
                                      char const *restrict const file_name,
                                      const uint64_t length);

bool
MissRateCurve__write_binary_to_file(
    struct BasicMissRateCurve *me,
    char const *restrict const file_name);

/// @return non-negative mean squared error, or a negative number (e.g. -1.0) on
///         error.
double
MissRateCurve__mean_squared_error(struct BasicMissRateCurve *lhs,
                                          struct BasicMissRateCurve *rhs);

/// @return non-negative mean absolute error, or a negative number (e.g. -1.0)
///         on error.
double
MissRateCurve__mean_absolute_error(struct BasicMissRateCurve *lhs,
                                           struct BasicMissRateCurve *rhs);

void
MissRateCurve__print_as_json(struct BasicMissRateCurve *me);

void
MissRateCurve__destroy(struct BasicMissRateCurve *me);
