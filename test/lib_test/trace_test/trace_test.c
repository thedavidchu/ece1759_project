#include <glib.h>
#include <stdlib.h>

#include "logger/logger.h"
#include "trace/reader.h"

int
main(int argc, char **argv)
{
    if (argc != 2) {
        LOGGER_ERROR("expecting trace file name as argument");
        return EXIT_FAILURE;
    }
    struct Trace trace = read_trace_keys(argv[1], TRACE_FORMAT_KIA);
    g_assert_nonnull(trace.trace);
    return 0;
}
