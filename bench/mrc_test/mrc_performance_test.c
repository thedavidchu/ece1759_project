#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "random/zipfian_random.h"
#include "test/mytester.h"

#include "mimir/mimir.h"
#include "olken/olken.h"
#include "parda_shards/parda_fixed_rate_shards.h"
#include "quickmrc/quickmrc.h"
#include "shards/fixed_size_shards.h"
#include "unused/mark_unused.h"

const uint64_t MAX_NUM_UNIQUE_ENTRIES = 1 << 20;
const double ZIPFIAN_RANDOM_SKEW = 0.5;
const uint64_t RANDOM_SEED = 0;

#define PERFORMANCE_TEST(MRCStructType,                                        \
                         mrc_var_name,                                         \
                         init_expr,                                            \
                         access_item_func_name,                                \
                         destroy_func_name)                                    \
    do {                                                                       \
        const uint64_t trace_length = 1 << 20;                                 \
        struct ZipfianRandom zrng = {0};                                       \
        MRCStructType mrc_var_name = {0};                                      \
                                                                               \
        g_assert_true(ZipfianRandom__init(&zrng,                               \
                                          MAX_NUM_UNIQUE_ENTRIES,              \
                                          ZIPFIAN_RANDOM_SKEW,                 \
                                          RANDOM_SEED));                       \
        /* The maximum trace length is the number of possible unique items */  \
        g_assert_true(((init_expr)));                                          \
        clock_t start_time = clock();                                          \
        for (uint64_t i = 0; i < trace_length; ++i) {                          \
            uint64_t key = ZipfianRandom__next(&zrng);                         \
            ((access_item_func_name))(&((mrc_var_name)), key);                 \
        }                                                                      \
        clock_t end_time = clock();                                            \
        double elapsed_time =                                                  \
            (double)(end_time - start_time) / (double)CLOCKS_PER_SEC;          \
        printf("Elapsed time for '" #MRCStructType "' workload: %.4f.\n",      \
               elapsed_time);                                                  \
        ZipfianRandom__destroy(&zrng);                                         \
        ((destroy_func_name))(&((mrc_var_name)));                              \
    } while (0)

int
main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    PERFORMANCE_TEST(struct Olken,
                     me,
                     Olken__init(&me, MAX_NUM_UNIQUE_ENTRIES),
                     Olken__access_item,
                     Olken__destroy);

    PERFORMANCE_TEST(
        struct FixedSizeShards,
        me,
        FixedSizeShards__init(&me, 1e-3, 10000, MAX_NUM_UNIQUE_ENTRIES),
        FixedSizeShards__access_item,
        FixedSizeShards__destroy);

    PERFORMANCE_TEST(
        struct Mimir,
        me,
        Mimir__init(&me, 1000, MAX_NUM_UNIQUE_ENTRIES, MIMIR_ROUNDER),
        Mimir__access_item,
        Mimir__destroy);

#if 0
    PERFORMANCE_TEST(
        struct Mimir,
        me,
        Mimir__init(&me, 1000, MAX_NUM_UNIQUE_ENTRIES, MIMIR_STACKER),
        Mimir__access_item,
        Mimir__destroy);
#endif

    PERFORMANCE_TEST(struct PardaFixedRateShards,
                     me,
                     PardaFixedRateShards__init(&me, 1e-3),
                     PardaFixedRateShards__access_item,
                     PardaFixedRateShards__destroy);

    PERFORMANCE_TEST(struct QuickMRC,
                     me,
                     QuickMRC__init(&me, 1024, 16, MAX_NUM_UNIQUE_ENTRIES),
                     QuickMRC__access_item,
                     QuickMRC__destroy);

    return EXIT_SUCCESS;
}
