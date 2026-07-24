#include <Eruptor/renderer.hpp>
#include <Eruptor/hardware.hpp>
#include <Eruptor/hardware/utilities.hpp>
#include <Eruptor/resource_manager.hpp>
#include <Eruptor/event/event_manager.hpp>
#include <Eruptor/scene/scene.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include <stack>
#include <iostream>

#ifndef NDEBUG
constexpr bool DEBUG_RENDER = true;
bool render_debug_enabled = true;
#else
constexpr bool DEBUG_RENDER = false;
#endif

struct eruptor::renderer::Frame_sync
{
    vk::raii::Semaphore present_complete = nullptr;
    vk::raii::Semaphore render_finished = nullptr;
    vk::raii::Fence in_flight_fences = nullptr;
};

eruptor::renderer::Renderer::Renderer(): event_manager{event::event_manager}
{

}

void eruptor::renderer::Renderer::Init(hardware::Hardware & hardware, resource::Resource_manager & resource_manager)
{
    this->hardware = &hardware;
    this->rs_resource_manager = &resource_manager;
    this->hw_resource_manager = &hardware.Get_resource_manager();

    auto & device = this->hardware->Get_device();

    vk::FenceCreateInfo fence_info{};
    fence_info.flags = vk::FenceCreateFlagBits::eSignaled;

    uint32_t image_count = this->hardware->Get_swapchain().Get_image_count(); // patrz punkt 3 niżej

    for(uint32_t i{}; i < image_count; i++)
    {
        frame_syncs.push_back({});
        frame_syncs.back().present_complete = vk::raii::Semaphore{device.Get_device_handle(), vk::SemaphoreCreateInfo{}};
        frame_syncs.back().render_finished = vk::raii::Semaphore{device.Get_device_handle(), vk::SemaphoreCreateInfo{}};
        frame_syncs.back().in_flight_fences = vk::raii::Fence{device.Get_device_handle(), fence_info};
    }

    event_manager.Add_listener( *this );
}

void eruptor::renderer::Renderer::Stage_scene_render_data(scene::Scene & scene)
{
    for(uint32_t i{1}; i < scene.render_objects.size(); i++)
    {
        Stage_object_render_data( scene.render_objects[i] );
    }
}

void eruptor::renderer::Renderer::Stage_object_render_data(scene::Render_object & object)
{
    auto model_handle = object.Get_model_handle();
    auto model_data = rs_resource_manager->Get_model( model_handle );

    assert(model_data.status == resource::Status::LODADED);

    Render_request<hardware::Push_constant_opaque> opaque_request{};
    hardware::Push_constant_opaque push_constant_opaque{};
    push_constant_opaque.model = object.Get_model_matrix();
    push_constant_opaque.color = object.color;

    for(auto mesh_handle : model_data.Meshes_handles)
    {
        auto mesh = hw_resource_manager->Get_mesh( mesh_handle );

        opaque_request.vertex_offset = mesh.vertex_offset;
        opaque_request.indices_offset = mesh.indices_offset;
        opaque_request.indices_amount = mesh.indices_amount;
        opaque_request.material_id = mesh.material_id;
        opaque_request.push_constant = push_constant_opaque;

        render_queue.opaque_queue.push_back( opaque_request );
    }

    if constexpr(DEBUG_RENDER)
    {
        if(render_debug_enabled)
        {
            Render_request<hardware::Push_constant_debug> debug_request{};
            hardware::Push_constant_debug push_constant_debug{};

            auto aabb = object.Get_aabb();

            push_constant_debug.color = object.is_coliding ?  glm::vec3{1.0f, 0.0f, 0.0f} : glm::vec3{0.0f, 0.0f, 1.0f};
            push_constant_debug.min = aabb.min;
            push_constant_debug.max = aabb.max;

            debug_request.indices_amount = 24;
            debug_request.push_constant = push_constant_debug;

            render_queue.debug_queue.push_back( debug_request );
        }
    }
}

void eruptor::renderer::Renderer::Flush_render_buffor()
{
    auto & device = hardware->Get_device();
    auto & swap_chain = hardware->Get_swapchain();
    auto & command_manager = hardware->Get_command_manager();
    auto & pipelines = hardware->Get_pipelines();

    auto fence_result = device.Get_device_handle().waitForFences(*frame_syncs[current_frame].in_flight_fences, vk::True, UINT64_MAX);
    if (fence_result != vk::Result::eSuccess)
    {
        throw std::runtime_error("failed to wait for fence!");
    }
    device.Get_device_handle().resetFences(*frame_syncs[current_frame].in_flight_fences);

    auto & command_buffor = command_manager.Begin_graphic_command_record(current_frame);
    auto [result, image_index] = swap_chain.Get_swapchain_handle().acquireNextImage(UINT64_MAX, *frame_syncs[current_frame].present_complete, nullptr);


    hardware::utilities::Transition_image_layout(
        command_buffor,
        swap_chain.Get_image(image_index),
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        vk::ImageAspectFlagBits::eColor
    );

    hardware::utilities::Transition_image_layout(
        command_buffor,
        swap_chain.Get_depth_image(),
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        vk::ImageAspectFlagBits::eDepth
    );

    vk::ClearValue clear_color = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::RenderingAttachmentInfo attachment_info = {};
    attachment_info.setImageView(swap_chain.Get_image_view(image_index));
    attachment_info.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal);
    attachment_info.setLoadOp(vk::AttachmentLoadOp::eClear);
    attachment_info.setStoreOp(vk::AttachmentStoreOp::eStore);
    attachment_info.setClearValue( clear_color );

    vk::RenderingAttachmentInfo depth_info{};
    depth_info.setImageView( hardware->Get_swapchain().Get_depth_image_view() );
    depth_info.setImageLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );
    depth_info.setLoadOp( vk::AttachmentLoadOp::eClear );
    depth_info.setStoreOp( vk::AttachmentStoreOp::eDontCare );
    depth_info.setClearValue( vk::ClearDepthStencilValue{1.0f, 0} );

    vk::Rect2D render_area{};
    render_area.setOffset({0,0});
    render_area.setExtent( swap_chain.Get_extent() );

    vk::RenderingInfo rendering_info = {};
    rendering_info.setRenderArea( render_area );
    rendering_info.setLayerCount(1);
    rendering_info.setColorAttachments( attachment_info );
    rendering_info.setPDepthAttachment( &depth_info );

    command_buffor.beginRendering(rendering_info);
    command_buffor.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swap_chain.Get_extent().width), static_cast<float>(swap_chain.Get_extent().height), 0.0f, 1.0f));
    command_buffor.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swap_chain.Get_extent()));

    command_buffor.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.Get_pipeline_handle(hardware::Pipeline_id::OPAQUE));

    hw_resource_manager->Bind_geometry_buffer( command_buffor );
    hardware->Get_uniform_buffers().Bind_vp_buffer(command_buffor, current_frame, pipelines, hardware::Pipeline_id::OPAQUE, fly_camera.Get_view_matrix());

    for(auto & render_request : render_queue.opaque_queue)
    {
        resource::Material_handle mat_handle{render_request.material_id};
        auto material = rs_resource_manager->Get_material( mat_handle );
        auto diffuse_texture = material.diffuse_texture_handle;

        command_buffor.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, hardware->Get_pipelines().Get_pipeline_layout(hardware::Pipeline_id::OPAQUE), 1, *hw_resource_manager->Get_texture_descriptor_set( diffuse_texture.Get_id() ), nullptr);
        command_buffor.pushConstants<hardware::Push_constant_opaque>(hardware->Get_pipelines().Get_pipeline_layout(hardware::Pipeline_id::OPAQUE), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, render_request.push_constant );

        command_buffor.drawIndexed(render_request.indices_amount, 1, render_request.indices_offset, render_request.vertex_offset, 0);
    }

    if constexpr(DEBUG_RENDER)
    {
        if(!render_queue.debug_queue.empty())
        {
            command_buffor.bindPipeline(vk::PipelineBindPoint::eGraphics, pipelines.Get_pipeline_handle(hardware::Pipeline_id::DEBUG));

            hardware->Get_uniform_buffers().Bind_vp_buffer(command_buffor, current_frame, pipelines, hardware::Pipeline_id::DEBUG, fly_camera.Get_view_matrix());
            for(auto & render_request : render_queue.debug_queue)
            {
                command_buffor.pushConstants<hardware::Push_constant_debug>(hardware->Get_pipelines().Get_pipeline_layout(hardware::Pipeline_id::DEBUG), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, render_request.push_constant );

                command_buffor.draw(24, 1, 0, 0);
            }
        }
    }

    command_buffor.endRendering();

    hardware::utilities::Transition_image_layout(
        command_buffor,
        swap_chain.Get_image(image_index),
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::QueueFamilyIgnored,
        vk::QueueFamilyIgnored,
        vk::ImageAspectFlagBits::eColor
    );

    command_buffor.end();
    command_manager.Submit_graphic_commands(command_buffor, std::vector<vk::PipelineStageFlags>{vk::PipelineStageFlagBits::eColorAttachmentOutput}, std::vector{*frame_syncs[current_frame].present_complete}, std::vector{*frame_syncs[image_index].render_finished}, *frame_syncs[current_frame].in_flight_fences);

    vk::PresentInfoKHR presentInfoKHR{};
    presentInfoKHR.setWaitSemaphores( *frame_syncs[image_index].render_finished );
    presentInfoKHR.setSwapchains( *swap_chain.Get_swapchain_handle() );
    presentInfoKHR.setImageIndices( image_index );

    auto result_2 = device.queues.Get_graphics_queue_handle().presentKHR(presentInfoKHR);
    switch (result_2)
    {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
            break;
        default:
            break;
    }

    current_frame = (current_frame + 1) % static_cast<uint32_t>(hardware->MAX_FRAMES_IN_FLIGHT);

    render_queue.opaque_queue.clear();
    render_queue.debug_queue.clear();
}

void eruptor::renderer::Renderer::On_event(const event::Event & event)
{
    if constexpr (DEBUG_RENDER)
    {
        if(auto key_press = event.Get_if<event::Event::Key_pressed>())
        {
            if(key_press->key_type == event::Key::F1)
            {
                render_debug_enabled = !render_debug_enabled;
            }
        }
    }
}

eruptor::hardware::Window & eruptor::renderer::Renderer::Get_window()
{
    return hardware->Get_window();
}

eruptor::renderer::Renderer::~Renderer()
{
    hardware->Get_device().Get_device_handle().waitIdle();
}
