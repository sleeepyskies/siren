export module siren.render:command_executor;

import :render_command;
import :resource_command;

namespace siren::render {

/**
 * @class CommandExecutor
 * @brief Interface for processing CommandBuffers.
 * Is a backend specific executor. Translates siren commands into API specific calls.
 */
export class CommandExecutor {
public:
    virtual ~CommandExecutor() = default;

    /**
     * @brief Consumes and executes resource related commands.
     * @param resource_command_pacakge The @ref ResourceCommandPacakge to execute.
     */
    virtual auto execute_resource_commands(ResourceCommandBuffer&& resource_command_pacakge) -> void = 0;

    /**
     * @brief Consumes and executes render related commands.
     * @param render_command_package The @ref RenderCommandPackage to execute.
     */
    virtual auto execute_render_commands(RenderCommandBuffer&& render_command_package) -> void = 0;
};

} // namespace siren::render
