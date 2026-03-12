#include "assert.hpp"

#include <iostream>

import siren.log;
import siren.common;

namespace siren::detail {
auto report_assert(const std::string& msg, const std::source_location sl) -> void {
    if (g_crash_handler) {
        g_crash_handler(sl, msg);
        return;
    }

    std::cerr << "[SIREN FATAL ASSERT (no logger)]"
            << msg << "" << "[" << sl.file_name() << ":" << sl.line() << " " << sl.function_name() << "]" << std::endl;

    std::abort();
}
} // namespace siren::core::detail
