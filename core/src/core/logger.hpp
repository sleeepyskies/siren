#pragma once

namespace siren::core
{
struct Logger {
    using logger = std::shared_ptr<spdlog::logger>;

    static logger core;
    static logger assets;
    static logger ecs;
    static logger renderer;
    static logger ui;
};
} // namespace siren::core
