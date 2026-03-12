module;

#include <string_view>

export module siren.app.plugin;

namespace siren {

class App;

export class Plugin {
public:
    virtual ~Plugin() = default;
    virtual auto construct(App& app) const -> void = 0;
    virtual auto shutdown(App& app) const -> void = 0;
};

} // namespace siren
