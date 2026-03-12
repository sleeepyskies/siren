export module siren.ecs.system;

namespace siren::ecs {

template <typename T>
concept IsSystem = requires() { };

} // namespace siren::ecs

