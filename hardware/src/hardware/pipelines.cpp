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
    vk::raii::ShaderModule opaque_shader_module = Create_shader_module(device, Read_file("./engine/hardware/shaders/opaque_shader.spv"));
    vk::raii::ShaderModule debug_shader_module = Create_shader_module(device, Read_file("./engine/hardware/shaders/debug_shader.spv"));

    vk::PipelineShaderStageCreateInfo vertex_shader_stage_opaque_info{};
    vertex_shader_stage_opaque_info.setStage( vk::ShaderStageFlagBits::eVertex );
    vertex_shader_stage_opaque_info.setModule( opaque_shader_module );
    vertex_shader_stage_opaque_info.setPName( "Vert_main" );

    vk::PipelineShaderStageCreateInfo fragment_shader_stage_opaque_info{};
    fragment_shader_stage_opaque_info.setStage( vk::ShaderStageFlagBits::eFragment );
    fragment_shader_stage_opaque_info.setModule( opaque_shader_module);
    fragment_shader_stage_opaque_info.setPName( "Frag_main" );

    vk::PipelineShaderStageCreateInfo vertex_shader_stage_debug_info{};
    vertex_shader_stage_debug_info.setStage( vk::ShaderStageFlagBits::eVertex );
    vertex_shader_stage_debug_info.setModule( debug_shader_module );
    vertex_shader_stage_debug_info.setPName( "Vert_main" );

    vk::PipelineShaderStageCreateInfo fragment_shader_stage_debug_info{};
    fragment_shader_stage_debug_info.setStage( vk::ShaderStageFlagBits::eFragment );
    fragment_shader_stage_debug_info.setModule( debug_shader_module );
    fragment_shader_stage_debug_info.setPName( "Frag_main" );

    vk::PipelineShaderStageCreateInfo opaque_shader_stages[] = {vertex_shader_stage_opaque_info, fragment_shader_stage_opaque_info };
    vk::PipelineShaderStageCreateInfo debug_shader_stages[] = {vertex_shader_stage_debug_info, fragment_shader_stage_debug_info};

    std::vector<vk::DynamicState> dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamic_state_info{};
    dynamic_state_info.setDynamicStates( dynamic_states );

    auto opaque_biding_description = Vertex::Get_binding_descriptor();
    auto opaque_attribute_description = Vertex::Get_attribute_descriptions();
    vk::PipelineVertexInputStateCreateInfo opaque_vertex_input_info{};
    opaque_vertex_input_info.setVertexBindingDescriptions(opaque_biding_description);
    opaque_vertex_input_info.setVertexAttributeDescriptions(opaque_attribute_description);

    vk::PipelineVertexInputStateCreateInfo debug_vertex_input_info{};

    vk::PipelineInputAssemblyStateCreateInfo opaque_input_assembly{};
    opaque_input_assembly.setTopology(vk::PrimitiveTopology::eTriangleList);

    vk::PipelineInputAssemblyStateCreateInfo debug_input_assembly{};
    debug_input_assembly.setTopology( vk::PrimitiveTopology::eLineList );

    auto swapchain_extent = swapchain.Get_extent();
    vk::Viewport viewport{0.0f, 0.0f, static_cast<float>(swapchain_extent.width), static_cast<float>(swapchain_extent.height), 0.0f, 1.0f};
    vk::Rect2D scissor{vk::Offset2D{0, 0}, swapchain_extent};

    vk::PipelineViewportStateCreateInfo viewport_state{};
    viewport_state.setViewports(viewport);
    viewport_state.setScissors(scissor);

    vk::PipelineRasterizationStateCreateInfo opaque_rasterizer{};
    opaque_rasterizer.depthClampEnable = vk::False;
    opaque_rasterizer.rasterizerDiscardEnable = vk::False;
    opaque_rasterizer.polygonMode = vk::PolygonMode::eFill;
    opaque_rasterizer.cullMode = vk::CullModeFlagBits::eNone;
    opaque_rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
    opaque_rasterizer.depthBiasEnable = vk::False;
    opaque_rasterizer.lineWidth = 1.0f;

    vk::PipelineRasterizationStateCreateInfo debug_rasterizer{};
    debug_rasterizer.setDepthClampEnable( vk::False );
    debug_rasterizer.setRasterizerDiscardEnable( vk::False );
    debug_rasterizer.setPolygonMode( vk::PolygonMode::eFill );
    debug_rasterizer.setCullMode( vk::CullModeFlagBits::eNone);
    debug_rasterizer.setFrontFace( vk::FrontFace::eClockwise );
    debug_rasterizer.setDepthBiasEnable( vk::False );
    debug_rasterizer.setLineWidth( 1.0f );

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
    multisampling.sampleShadingEnable = vk::False;

    vk::PipelineDepthStencilStateCreateInfo opaque_depth_stencil{};
    opaque_depth_stencil.depthTestEnable = vk::True;
    opaque_depth_stencil.depthWriteEnable = vk::True;
    opaque_depth_stencil.depthCompareOp = vk::CompareOp::eLess;
    opaque_depth_stencil.depthBoundsTestEnable = vk::False;
    opaque_depth_stencil.stencilTestEnable = vk::False;

    vk::PipelineDepthStencilStateCreateInfo debug_depth_stencil{};
    debug_depth_stencil.setDepthTestEnable( vk::False );
    debug_depth_stencil.setDepthWriteEnable( vk::False );
    debug_depth_stencil.setDepthCompareOp( vk::CompareOp::eLess );
    debug_depth_stencil.setDepthBoundsTestEnable( vk::False );
    debug_depth_stencil.setStencilTestEnable( vk::False );

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
    color_blending.setAttachments( color_blend_attachment );

    vk::PushConstantRange opaque_push_constant_range{};
    opaque_push_constant_range.setStageFlags( vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment );
    opaque_push_constant_range.setOffset( 0 );
    opaque_push_constant_range.setSize( sizeof(Push_constant_opaque) );

    vk::PushConstantRange debug_push_constant_range{};
    debug_push_constant_range.setStageFlags( vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment );
    debug_push_constant_range.setOffset( 0 );
    debug_push_constant_range.setSize( sizeof(Push_constant_debug) );

    std::array<vk::DescriptorSetLayout, 2> opaque_set_layouts
    {
        *uniforms_buffers.Get_descriptor_set_layout(),
        *resource_manager.Get_texture_set_layout()
    };

    std::array<vk::DescriptorSetLayout, 1> debug_set_layouts
    {
        *uniforms_buffers.Get_descriptor_set_layout()
    };

    vk::PipelineLayoutCreateInfo opaque_pipeline_layout_info{};
    opaque_pipeline_layout_info.setSetLayouts( opaque_set_layouts );
    opaque_pipeline_layout_info.setPushConstantRanges( opaque_push_constant_range );

    vk::PipelineLayoutCreateInfo debug_pipeline_layout_info{};
    debug_pipeline_layout_info.setSetLayouts( debug_set_layouts );
    debug_pipeline_layout_info.setPushConstantRanges( debug_push_constant_range );

    pipeline_layouts[ std::to_underlying( Pipeline_id::OPAQUE ) ].emplace( device.Get_device_handle(), opaque_pipeline_layout_info );
    pipeline_layouts[ std::to_underlying( Pipeline_id::DEBUG ) ].emplace( device.Get_device_handle(), debug_pipeline_layout_info );

    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> opaque_pipeline_create_info_chain = {vk::GraphicsPipelineCreateInfo{}, vk::PipelineRenderingCreateInfo{}};
    vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> debug_pipeline_create_info_chain = {vk::GraphicsPipelineCreateInfo{}, vk::PipelineRenderingCreateInfo{}};

    auto depth_format = swapchain.Find_depth_format(device);

    opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().setStages(opaque_shader_stages);
    opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pVertexInputState = &opaque_vertex_input_info;
    opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pInputAssemblyState = &opaque_input_assembly;
    opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pViewportState = &viewport_state;
    opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pRasterizationState = &opaque_rasterizer;
    opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDepthStencilState = &opaque_depth_stencil;
    opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pMultisampleState = &multisampling;
    opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pColorBlendState = &color_blending;
    opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDynamicState = &dynamic_state_info;
    opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().layout = *pipeline_layouts[ std::to_underlying( Pipeline_id::OPAQUE ) ];
    opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().renderPass = nullptr;
    opaque_pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().colorAttachmentCount = 1;
    opaque_pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().pColorAttachmentFormats = & swapchain.Get_surface_format().format;
    opaque_pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().depthAttachmentFormat = depth_format;

    debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().setStages(debug_shader_stages);
    debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pVertexInputState = &debug_vertex_input_info;
    debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pInputAssemblyState = &debug_input_assembly;
    debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pViewportState = &viewport_state;
    debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pRasterizationState = &debug_rasterizer;
    debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDepthStencilState = &debug_depth_stencil;
    debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pMultisampleState = &multisampling;
    debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pColorBlendState = &color_blending;
    debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().pDynamicState = &dynamic_state_info;
    debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().layout = *pipeline_layouts[ std::to_underlying( Pipeline_id::DEBUG ) ];
    debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>().renderPass = nullptr;
    debug_pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().colorAttachmentCount = 1;
    debug_pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().pColorAttachmentFormats = & swapchain.Get_surface_format().format;
    debug_pipeline_create_info_chain.get<vk::PipelineRenderingCreateInfo>().depthAttachmentFormat = depth_format;

    pipelines[ std::to_underlying( Pipeline_id::OPAQUE ) ].emplace( device.Get_device_handle(), nullptr, opaque_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>() );
    pipelines[ std::to_underlying( Pipeline_id::DEBUG ) ].emplace( device.Get_device_handle(), nullptr, debug_pipeline_create_info_chain.get<vk::GraphicsPipelineCreateInfo>() );
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







