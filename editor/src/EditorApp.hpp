#pragma once

#include "core/App.hpp"

namespace siren::editor
{

class EditorApp final : public core::Application
{
public:
    explicit EditorApp(const Properties& properties);
};

} // namespace siren::editor
