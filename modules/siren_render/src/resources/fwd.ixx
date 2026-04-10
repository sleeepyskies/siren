export module siren.render:resource_fwd;

import :render_resource;

namespace siren::render {

// forward declarations for render resources

export class Buffer;
export using BufferHandle = RenderResourceID<Buffer>;
export struct BufferDescriptor;

export class Framebuffer;
export using FramebufferHandle = RenderResourceID<Framebuffer>;
export struct FramebufferDescriptor;

export class GraphicsPipeline;
export using GraphicsPipelineHandle = RenderResourceID<GraphicsPipeline>;
export struct GraphicsPipelineDescriptor;

export class Image;
export using ImageHandle = RenderResourceID<Image>;
export struct ImageDescriptor;

export class Sampler;
export using SamplerHandle = RenderResourceID<Sampler>;
export struct SamplerDescriptor;

export class Shader;
export using ShaderHandle = RenderResourceID<Shader>;
export struct ShaderDescriptor;

} // namespace siren::render
