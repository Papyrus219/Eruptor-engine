#include <Eruptor/hardware/pipelines.hpp>
#include <Eruptor/hardware/device.hpp>
#include <Eruptor/hardware/swapchain.hpp>
#include <Eruptor/hardware/uniform_buffers.hpp>
#include <Eruptor/hardware/resources/vertex.hpp>
#include <Eruptor/hardware/resources/resource_manager.hpp>
#include <Eruptor/hardware/resources/push_constants.hpp>
#include <fstream>

void eruptor::hardware::Pipelines::Init(Device& device, Swapchain & swapchain, Uniform_buffers & uniforms_buffers, Resource_manager & resource_manager)
{
    Create_graphics_pipelines(device, swapchain, uniforms_buffers, resource_manager);
}

vk::raii::Pipeline & eruptor::hardware::Pipelines::Get_pipeline_handle(Pipeline_id id)
{
    return *pipelines[ std::to_underlying(id) ];
}

vk::raii::PipelineLayout & eruptor::hardware::Pipelines::Get_pipeline_layout(Pipeline_id id)
{
    return *pipeline_layouts[ std::to_underlying(id) ];
}

void eruptor::hardware::Pipelines::Create_graphics_pipelines(Device& device, Swapchain & swapchain, Uniform_buffers & uniforms_buffers, Resource_manager & resource_manager)
{
    std::vector<vk::DynamicState> dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamic_state_info{};
    dynamic_state_info.setDynamicStates( dynamic_states );

    vk::PipelineInputAssemblyStateCreateInfo triangle_input_assembly{};
    triangle_input_assembly.setTopology(vk::PrimitiveTopology::eTriangleList);

    vk::PipelineInputAssemblyStateCreateInfo line_input_assembly{};
    line_input_assembly.setTopology( vk::PrimitiveTopology::eLineList );

    auto swapchain_extent = swapchain.Get_extent();
    vk::Viewport viewport{0.0f, 0.0f, static_cast<float>(swapchain_extent.width), static_cast<float>(swapchain_extent.height), 0.0f, 1.0f};
    vk::Rect2D scissor{vk::Offset2D{0, 0}, swapchain_extent};

    vk::PipelineViewportStateCreateInfo viewport_state{};
    viewport_state.setViewports(viewport);
    viewport_state.setScissors(scissor);

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

    vk::PipelineDepthStencilStateCreateInfo standard_depth_stencil{};
    standard_depth_stencil.depthTestEnable = vk::True;
    standard_depth_stencil.depthWriteEnable = vk::True;
    standard_depth_stencil.depthCompareOp = vk::CompareOp::eLess;
    standard_depth_stencil.depthBoundsTestEnable = vk::False;
    standard_depth_stencil.stencilTestEnable = vk::False;

    vk::PipelineDepthStencilStateCreateInfo debug_text_depth_stencil{};
    debug_text_depth_stencil.setDepthTestEnable( vk::False );
    debug_text_depth_stencil.setDepthWriteEnable( vk::False );
    debug_text_depth_stencil.setDepthCompareOp( vk::CompareOp::eLess );
    debug_text_depth_stencil.setDepthBoundsTestEnable( vk::False );
    debug_text_depth_stencil.setStencilTestEnable( vk::False );

    vk::PipelineColorBlendAttachmentState standard_color_blend_attachment{};
    standard_color_blend_attachment.blendEnable = vk::False;
    standard_color_blend_attachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
    standard_color_blend_attachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
    standard_color_blend_attachment.colorBlendOp = vk::BlendOp::eAdd;
    standard_color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    standard_color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    standard_color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;
    standard_color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendAttachmentState debug_text_color_blend_attachment{};
    debug_text_color_blend_attachment.setBlendEnable( vk::True );
    debug_text_color_blend_attachment.setSrcColorBlendFactor( vk::BlendFactor::eSrcAlpha );
    debug_text_color_blend_attachment.setDstColorBlendFactor( vk::BlendFactor::eOneMinusSrcAlpha );
    debug_text_color_blend_attachment.setColorBlendOp( vk::BlendOp::eAdd );
    debug_text_color_blend_attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
    debug_text_color_blend_attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
    debug_text_color_blend_attachment.alphaBlendOp = vk::BlendOp::eAdd;
    debug_text_color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;

    vk::PipelineColorBlendStateCreateInfo standard_color_blending{};
    standard_color_blending.logicOpEnable = vk::False;
    standard_color_blending.logicOp = vk::LogicOp::eCopy;
    standard_color_blending.setAttachments( standard_color_blend_attachment );

    vk::PipelineColorBlendStateCreateInfo debug_text_color_blending{};
    debug_text_color_blending.logicOpEnable = vk::False;
    debug_text_color_blending.logicOp = vk::LogicOp::eCopy;
    debug_text_color_blending.setAttachments( debug_text_color_blend_attachment );

    auto depth_format = swapchain.Find_depth_format(device);

    Create_opaque_pipeline(device, swapchain, uniforms_buffers, resource_manager, dynamic_state_info, triangle_input_assembly, rasterizer, standard_depth_stencil, multisampling, viewport_state, standard_color_blending, depth_format);
    Create_debug_pipeline(device, swapchain, uniforms_buffers, dynamic_state_info, line_input_assembly, rasterizer, debug_text_depth_stencil, multisampling, viewport_state, debug_text_color_blending, depth_format);
    Create_text_pipeline(device, swapchain, resource_manager, dynamic_state_info, triangle_input_assembly, rasterizer, debug_text_depth_stencil, multisampling, viewport_state, debug_text_color_blending, depth_format);
}

void eruptor::hardware::Pipelines::Create_opaque_pipeline(Device & device, Swapchain & swapchain, Uniform_buffers & uniforms_buffers, Resource_manager & resource_manager, vk::PipelineDynamicStateCreateInfo dynamic_state_info, vk::PipelineInputAssemblyStateCreateInfo input_assembly, vk::PipelineRasterizationStateCreateInfo rasterizer, vk::PipelineDepthStencilStateCreateInfo depth_stencil, vk::PipelineMultisampleStateCreateInfo multisampling, vk::PipelineViewportStateCreateInfo viewport_state, vk::PipelineColorBlendStateCreateInfo color_blending, vk::Format depth_format)
{
    vk::raii::ShaderModule shader_module = Create_shader_module(device, Read_file("./engine/hardware/shaders/opaque_shader.spv"));

    vk::PipelineShaderStageCreateInfo vertex_shader_stage_info{};
    vertex_shader_stage_info.setStage( vk::ShaderStageFlagBits::eVertex );
    vertex_shader_stage_info.setModule( shader_module );
    vertex_shader_stage_info.setPName( "Vert_main" );

    vk::PipelineShaderStageCreateInfo fragment_shader_stage_info{};
    fragment_shader_stage_info.setStage( vk::ShaderStageFlagBits::eFragment );
    fragment_shader_stage_info.setModule( shader_module);
    fragment_shader_stage_info.setPName( "Frag_main" );

    auto biding_description = Opaque_vertex::Get_binding_descriptor();
    auto attribute_description = Opaque_vertex::Get_attribute_descriptions();
    vk::PipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.setVertexBindingDescriptions(biding_description);
    vertex_input_info.setVertexAttributeDescriptions(attribute_description);

    vk::PipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage_info, fragment_shader_stage_info };

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipeline_create_info_chain = {vk::GraphicsPipelineCreateInfo{}, vk::PipelineRenderingCreateInfo{}};

    vk::PushConstantRange push_constant_range{};
    push_constant_range.setStageFlags( vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment );
    push_constant_range.setOffset( 0 );
    push_constant_range.setSize( sizeof(Push_constant_opaque) );

    std::array<vk::DescriptorSetLayout, 3> set_layouts
    {
        *uniforms_buffers.Get_vp_descriptor_set_layout(),
        *uniforms_buffers.Get_light_descriptor_set_layout(),
        *resource_manager.Get_texture_set_layout()
    };

    vk::PipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.setSetLayouts( set_layouts );
    pipeline_layout_info.setPushConstantRanges( push_constant_range );

    pipeline_layouts[ std::to_underlying( Pipeline_id::OPAQUE ) ].emplace( device.Get_device_handle(), pipeline_layout_info );

    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().setStages(shader_stages);
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pVertexInputState = &vertex_input_info;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pInputAssemblyState = &input_assembly;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pViewportState = &viewport_state;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pRasterizationState = &rasterizer;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDepthStencilState = &depth_stencil;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pMultisampleState = &multisampling;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pColorBlendState = &color_blending;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDynamicState = &dynamic_state_info;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().layout = *pipeline_layouts[ std::to_underlying( Pipeline_id::OPAQUE ) ];
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().renderPass = nullptr;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().colorAttachmentCount = 1;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().pColorAttachmentFormats = & swapchain.Get_surface_format().format;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().depthAttachmentFormat = depth_format;

    pipelines[ std::to_underlying( Pipeline_id::OPAQUE ) ].emplace( device.Get_device_handle(), nullptr, pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>() );
}

void eruptor::hardware::Pipelines::Create_debug_pipeline(Device & device, Swapchain & swapchain, Uniform_buffers & uniforms_buffers, vk::PipelineDynamicStateCreateInfo dynamic_state_info, vk::PipelineInputAssemblyStateCreateInfo input_assembly, vk::PipelineRasterizationStateCreateInfo rasterizer, vk::PipelineDepthStencilStateCreateInfo depth_stencil, vk::PipelineMultisampleStateCreateInfo multisampling, vk::PipelineViewportStateCreateInfo viewport_state, vk::PipelineColorBlendStateCreateInfo color_blending, vk::Format depth_format)
{
    vk::raii::ShaderModule shader_module = Create_shader_module(device, Read_file("./engine/hardware/shaders/debug_shader.spv"));

    vk::PipelineShaderStageCreateInfo vertex_shader_stage_info{};
    vertex_shader_stage_info.setStage( vk::ShaderStageFlagBits::eVertex );
    vertex_shader_stage_info.setModule( shader_module );
    vertex_shader_stage_info.setPName( "Vert_main" );

    vk::PipelineShaderStageCreateInfo fragment_shader_stage_info{};
    fragment_shader_stage_info.setStage( vk::ShaderStageFlagBits::eFragment );
    fragment_shader_stage_info.setModule( shader_module );
    fragment_shader_stage_info.setPName( "Frag_main" );

    vk::PipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage_info, fragment_shader_stage_info};

    vk::PipelineVertexInputStateCreateInfo vertex_input_info{};

    vk::PushConstantRange push_constant_range{};
    push_constant_range.setStageFlags( vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment );
    push_constant_range.setOffset( 0 );
    push_constant_range.setSize( sizeof(Push_constant_debug) );

    std::array<vk::DescriptorSetLayout, 1> set_layouts
    {
        *uniforms_buffers.Get_vp_descriptor_set_layout(),
    };

    vk::PipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.setSetLayouts( set_layouts );
    pipeline_layout_info.setPushConstantRanges( push_constant_range );

    pipeline_layouts[ std::to_underlying( Pipeline_id::DEBUG ) ].emplace( device.Get_device_handle(), pipeline_layout_info );

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipeline_create_info_chain = {vk::GraphicsPipelineCreateInfo{}, vk::PipelineRenderingCreateInfo{}};

    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().setStages(shader_stages);
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pVertexInputState = &vertex_input_info;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pInputAssemblyState = &input_assembly;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pViewportState = &viewport_state;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pRasterizationState = &rasterizer;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDepthStencilState = &depth_stencil;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pMultisampleState = &multisampling;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pColorBlendState = &color_blending;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDynamicState = &dynamic_state_info;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().layout = *pipeline_layouts[ std::to_underlying( Pipeline_id::DEBUG ) ];
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().renderPass = nullptr;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().colorAttachmentCount = 1;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().pColorAttachmentFormats = & swapchain.Get_surface_format().format;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().depthAttachmentFormat = depth_format;

    pipelines[ std::to_underlying( Pipeline_id::DEBUG ) ].emplace( device.Get_device_handle(), nullptr, pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>() );
}

void eruptor::hardware::Pipelines::Create_text_pipeline(Device & device, Swapchain & swapchain, Resource_manager & resource_manager, vk::PipelineDynamicStateCreateInfo dynamic_state_info, vk::PipelineInputAssemblyStateCreateInfo input_assembly, vk::PipelineRasterizationStateCreateInfo rasterizer, vk::PipelineDepthStencilStateCreateInfo depth_stencil, vk::PipelineMultisampleStateCreateInfo multisampling, vk::PipelineViewportStateCreateInfo viewport_state, vk::PipelineColorBlendStateCreateInfo color_blending, vk::Format depth_format)
{
    vk::raii::ShaderModule shader_module = Create_shader_module(device, Read_file("./engine/hardware/shaders/text_shader.spv"));

    vk::PipelineShaderStageCreateInfo vertex_shader_stage_info{};
    vertex_shader_stage_info.setStage( vk::ShaderStageFlagBits::eVertex );
    vertex_shader_stage_info.setModule( shader_module );
    vertex_shader_stage_info.setPName( "Vert_main" );

    vk::PipelineShaderStageCreateInfo fragment_shader_stage_info{};
    fragment_shader_stage_info.setStage( vk::ShaderStageFlagBits::eFragment );
    fragment_shader_stage_info.setModule( shader_module );
    fragment_shader_stage_info.setPName( "Frag_main" );

    vk::PipelineShaderStageCreateInfo shader_stages[] = {vertex_shader_stage_info, fragment_shader_stage_info};

    auto biding_description = Text_vertex::Get_binding_descriptor();
    auto attribute_descripton = Text_vertex::Get_attribute_descriptions();
    vk::PipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.setVertexBindingDescriptions( biding_description );
    vertex_input_info.setVertexAttributeDescriptions( attribute_descripton );

    vk::PushConstantRange push_constant_range{};
    push_constant_range.setStageFlags( vk::ShaderStageFlagBits::eVertex );
    push_constant_range.setOffset( 0 );
    push_constant_range.setSize( sizeof(Push_constant_text) );

    std::array<vk::DescriptorSetLayout, 1> set_layouts
    {
        *resource_manager.Get_texture_set_layout()
    };

    vk::PipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.setSetLayouts( set_layouts  );
    pipeline_layout_info.setPushConstantRanges( push_constant_range );

    pipeline_layouts[ std::to_underlying( Pipeline_id::TEXT ) ].emplace( device.Get_device_handle(), pipeline_layout_info );

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipeline_create_info_chain = {vk::GraphicsPipelineCreateInfo{}, vk::PipelineRenderingCreateInfo{}};

    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().setStages(shader_stages);
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pVertexInputState = &vertex_input_info;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pInputAssemblyState = &input_assembly;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pViewportState = &viewport_state;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pRasterizationState = &rasterizer;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDepthStencilState = &depth_stencil;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pMultisampleState = &multisampling;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pColorBlendState = &color_blending;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDynamicState = &dynamic_state_info;
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().layout = *pipeline_layouts[ std::to_underlying( Pipeline_id::TEXT ) ];
    pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().renderPass = nullptr;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().colorAttachmentCount = 1;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().pColorAttachmentFormats = & swapchain.Get_surface_format().format;
    pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().depthAttachmentFormat = depth_format;

    pipelines[ std::to_underlying( Pipeline_id::TEXT ) ].emplace( device.Get_device_handle(), nullptr, pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>() );
}

vk::raii::ShaderModule eruptor::hardware::Pipelines::Create_shader_module(Device& device, const std::vector<char>& code) const
{
    vk::ShaderModuleCreateInfo create_info{};
    create_info.codeSize = code.size() * sizeof(char);
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

    return vk::raii::ShaderModule{device.Get_device_handle(), create_info};
}

std::vector<char> eruptor::hardware::Pipelines::Read_file(const std::filesystem::path& path)
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







