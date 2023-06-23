#include "VulkanImGuiLayer.h"
#include "Base/Application.h"
#include "Base/GlobalContext.h"
#include "Backend/Vulkan/VulkanDriverApi.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

namespace Ethereal {
static std::vector<VkCommandBuffer> sImGuiCommandBuffers;

VulkanImGuiLayer::VulkanImGuiLayer() {}

VulkanImGuiLayer::~VulkanImGuiLayer() {}

void VulkanImGuiLayer::OnAttach() {
    auto context = GlobalContext::GetDriverApi().As<Backend::VulkanDriverApi>()->GetContext();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look
    // identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    SetDarkThemeColors();
    VkDescriptorPool descriptorPool;

    // Create Descriptor Pool
    VkDescriptorPoolSize pool_sizes[] = {{VK_DESCRIPTOR_TYPE_SAMPLER, 100},
                                         {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100},
                                         {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100},
                                         {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100},
                                         {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100},
                                         {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100}};
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 100 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    if (vkCreateDescriptorPool(context->mDevice->GetDevice(), &pool_info, nullptr,
                               &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow(),
                                 true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = context->GetInstance();
    init_info.PhysicalDevice = context->mDevice->GetPhysicalDevice()->GetVulkanPhysicalDevice();
    init_info.Device = context->mDevice->GetDevice();
    init_info.QueueFamily = context->mDevice->GetPhysicalDevice()->GetQueueFamilyIndices().Graphics;
    init_info.Queue = context->mDevice->GetGraphicsQueue();
    init_info.PipelineCache = nullptr;
    init_info.DescriptorPool = descriptorPool;
    init_info.Allocator = nullptr;
    init_info.MinImageCount = 2;

    init_info.ImageCount = context->mSwapchain->GetImageCount();
    ImGui_ImplVulkan_Init(&init_info, context->mSwapchain->GetRenderPass());

    // Upload Fonts
    {
        io.Fonts->AddFontFromFileTTF("C:/windows/Fonts/arial.ttf", 14.0f);
        io.Fonts->AddFontFromFileTTF("C:/windows/Fonts/arialbd.ttf", 14.0f);

        VkCommandBuffer commandBuffer = context->mDevice->GetCommandBuffer(true);
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
        context->mDevice->FlushCommandBuffer(commandBuffer);

        if (vkDeviceWaitIdle(context->mDevice->GetDevice()) != VK_SUCCESS) {
            throw std::runtime_error("failed to wait device idle!");
        }
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    sImGuiCommandBuffers.resize(Backend::MAX_FRAMES_IN_FLIGHT);
    for (uint32_t i = 0; i < Backend::MAX_FRAMES_IN_FLIGHT; i++)
        sImGuiCommandBuffers[i] =
            context->mDevice->CreateSecondaryCommandBuffer("ImGuiSecondaryCoommandBuffer");
}

void VulkanImGuiLayer::OnDetach() {
    auto context = GlobalContext::GetDriverApi().As<Backend::VulkanDriverApi>()->GetContext();
    auto device = context->mDevice->GetDevice();

    if (vkDeviceWaitIdle(device) != VK_SUCCESS) {
        throw std::runtime_error("failed to wait device idle!");
    }

    vkFreeCommandBuffers(device, context->mDevice->GetCommandPool(), sImGuiCommandBuffers.size(),
                         sImGuiCommandBuffers.data());

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void VulkanImGuiLayer::OnImGuiRender() {
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
    // ImGui::ShowDebugLogWindow();
}

void VulkanImGuiLayer::OnEvent(Event& event) {}

void VulkanImGuiLayer::Begin() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void VulkanImGuiLayer::End() {
    auto context = GlobalContext::GetDriverApi().As<Backend::VulkanDriverApi>()->GetContext();

    ImGui::Render();

    VkClearValue clearValues[2];
    clearValues[0].color = {{0.1f, 0.1f, 0.1f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    uint32_t width = context->mSwapchain->GetWidth();
    uint32_t height = context->mSwapchain->GetHeight();

    uint32_t commandBufferIndex = context->mSwapchain->GetCurrentBufferIndex();

    VkCommandBufferBeginInfo drawCmdBufInfo = {};
    drawCmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    drawCmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    drawCmdBufInfo.pNext = nullptr;

    VkCommandBuffer drawCommandBuffer = context->mSwapchain->GetCurrentDrawCommandBuffer();
    if (vkBeginCommandBuffer(drawCommandBuffer, &drawCmdBufInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.pNext = nullptr;
    renderPassBeginInfo.renderPass = context->mSwapchain->GetRenderPass();
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.renderArea.extent.width = width;
    renderPassBeginInfo.renderArea.extent.height = height;
    renderPassBeginInfo.clearValueCount = 2;  // Color + depth
    renderPassBeginInfo.pClearValues = clearValues;
    renderPassBeginInfo.framebuffer = context->mSwapchain->GetCurrentFramebuffer();

    vkCmdBeginRenderPass(drawCommandBuffer, &renderPassBeginInfo,
                         VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

    VkCommandBufferInheritanceInfo inheritanceInfo = {};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inheritanceInfo.renderPass = context->mSwapchain->GetRenderPass();
    inheritanceInfo.framebuffer = context->mSwapchain->GetCurrentFramebuffer();

    VkCommandBufferBeginInfo cmdBufInfo = {};
    cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    cmdBufInfo.pInheritanceInfo = &inheritanceInfo;

    if (vkBeginCommandBuffer(sImGuiCommandBuffers[commandBufferIndex], &cmdBufInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = (float)height;
    viewport.height = (float)height;
    viewport.width = (float)width;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(sImGuiCommandBuffers[commandBufferIndex], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.extent.width = width;
    scissor.extent.height = height;
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    vkCmdSetScissor(sImGuiCommandBuffers[commandBufferIndex], 0, 1, &scissor);

    ImDrawData* main_draw_data = ImGui::GetDrawData();
    ImGui_ImplVulkan_RenderDrawData(main_draw_data, sImGuiCommandBuffers[commandBufferIndex]);

    if (vkEndCommandBuffer(sImGuiCommandBuffers[commandBufferIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    std::vector<VkCommandBuffer> commandBuffers;
    commandBuffers.push_back(sImGuiCommandBuffers[commandBufferIndex]);

    vkCmdExecuteCommands(drawCommandBuffer, uint32_t(commandBuffers.size()), commandBuffers.data());

    vkCmdEndRenderPass(drawCommandBuffer);

    if (vkEndCommandBuffer(drawCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}
}  // namespace Ethereal