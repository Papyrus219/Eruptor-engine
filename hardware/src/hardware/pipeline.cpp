#include <Eruptor/hardware/pipeline.hpp>
#include <Eruptor/hardware/device.hpp>
#include <Eruptor/hardware/swapchain.hpp>
#include <Eruptor/hardware/uniform_buffers.hpp>
#include <Eruptor/hardware/resources/vertex.hpp>
#include <Eruptor/hardware/resources/resource_manager.hpp>
#include <Eruptor/hardware/resources/push_constant.hpp>
#include <fstream>

void eruptor::hardware::Pipeline::Init(Device& device, Swapchain & swapchain, Uniform_buffers & uniforms_buffers, Resource_manager & resource_manager)
{
    Create_graphics_pipeline(device, swapchain, uniforms_buffers, resource_manager);
}

void eruptor::hardware::Pipeline::Create_graphics_pipeline(Device& device, Swapchain & swapchain, Uniform_buffers & uniforms_buffers, Resource_manager & resource_manager)
{
    vk::raii::ShaderModule shader_module = Create_shader_module(device, Read_file("./engine/hardware/shaders/slang.spv"));

    vk::PipelineShaderStageCreateInfo vertex_shader_stage_info{};
    vertex_shader_stage_info.stage = vk::ShaderStageFlagBits::eVertex;
    vertex_shader_stage_info.module = shader_module;
    vertex_shader_stage_info.pName = "Vert_main";

    vk::PipelineShaderStageCreateInfo fragment_shader_stage_info{};
    fragment_shader_stage_info.stage = vk::ShaderStageFlagBits::eFragment;
    fragment_shader_stage_info.module = shader_module;
    fragment_shader_stage_info.pName = "Frag_main";

    vk::PipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage_info, fragment_shader_stage_info };

    std::vector<vk::DynamicState> dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamic_state_info{};
    dynamic_state_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamic_state_info.pDynamicStates = dynamic_states.data();

    auto biding_description = Vertex::Get_binding_descriptor();
    auto attribute_description = Vertex::Get_attribute_descriptions();
    vk::PipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.pVertexBindingDescriptions = &biding_description;
    vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_description.size());
    vertex_input_info.pVertexAttributeDescriptions = attribute_description.data();

    vk::PipelineInputAssemblyStateCreateInfo input_assembly{};
    input_assembly.topology = vk::PrimitiveTopology::eTriangleList;

    auto swapchain_extent = swapchain.Get_extent();
    vk::Viewport viewport{0.0f, 0.0f, static_cast<float>(swapchain_extent.width), static_cast<float>(swapchain_extent.height), 0.0f, 1.0f};
    vk::Rect2D scissor{vk::Offset2D{0, 0}, swapchain_extent};

    vk::PipelineViewportStateCreateInfo viewport_state{};
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.depthClampEnable = vk::False;
    rasterizer.rasterizerDiscardEnable = vk::False;
    rasterizer.polygonMode = vk::PolygonMode::eFill;
    rasterizer.cullMode = vk::CullModeFlagBits::eNone;
    rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizer.depthBiasEnable = vk::False;
    rasterizer.lineWidth = 1.0f;

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.sampleShadingEnable = vk::False;

    vk::PipelineDepthStencilStateCreateInfo depth_stencil{};
    depth_stencil.depthTestEnable = vk::True;
    depth_stencil.depthWriteEnable = vk::True;
    depth_stencil.depthCompareOp = vk::CompareOp::eLess;
    depth_stencil.depthBoundsTestEnable = vk::False;
    depth_stencil.stencilTestEnable = vk::False;

    vk::PipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.blendEnable = vk::False;
    color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;
    color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;
    color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo color_blending{};
    color_blending.logicOpEnable = vk::False;
    color_blending.logicOp = vk::LogicOp::eCopy;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;

    vk::PushConstantRange push_constant_range{};
    push_constant_range.setStageFlags( vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment );
    push_constant_range.setOffset( 0 );
    push_constant_range.setSize( sizeof(Push_constant) );

    std::array<vk::DescriptorSetLayout, 2> set_layouts
    {
        *uniforms_buffers.Get_descriptor_set_layout(),
        *resource_manager.Get_texture_set_layout()
    };

    vk::PipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.setSetLayouts( set_layouts );
    pipeline_layout_info.setPushConstantRanges( push_constant_range );

    pipeline_layout = vk::raii::PipelineLayout{device.Get_device_handle(), pipeline_layout_info};

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipeline_create_info_chain = {vk::GraphicsPipelineCreateInfo{}, vk::PipelineRenderingCreateInfo{}};

    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().stageCount = 2;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pStages = shader_stages;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pVertexInputState = &vertex_input_info;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pInputAssemblyState = &input_assembly;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pViewportState = &viewport_state;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pRasterizationState = &rasterizer;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDepthStencilState = &depth_stencil;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pMultisampleState = &multisampling;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pColorBlendState = &color_blending;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDynamicState = &dynamic_state_info;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().layout = pipeline_layout;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().renderPass = nullptr;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().colorAttachmentCount = 1;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().pColorAttachmentFormats = & swapchain.Get_surface_format().format;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().depthAttachmentFormat = swapchain.Find_depth_format(device);

    graphics_pipeline = vk::raii::Pipeline{device.Get_device_handle(), nullptr, pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>()};
}

vk::raii::ShaderModule eruptor::hardware::Pipeline::Create_shader_module(Device& device, const std::vector<char>& code) const
{
    vk::ShaderModuleCreateInfo create_info{};
    create_info.codeSize = code.size() * sizeof(char);
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    return vk::raii::ShaderModule{device.Get_device_handle(), create_info};
}

std::vector<char> eruptor::hardware::Pipeline::Read_file(const std::filesystem::path& path)
{
    std::ifstream file{path, std::ios::ate | std::ios::binary};

    if(!file.is_open())
    {
        throw std::runtime_error{"Shader: failed to open file: " + path.string()};
    }

    std::vector<char> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

    file.close();
    return buffer;
}







