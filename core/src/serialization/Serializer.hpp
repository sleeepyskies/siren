#pragma once

namespace siren::core
{

template <typename T>
concept Serializable = requires (const T& t) { t.to_string(); };

} // namespace siren::core
