#pragma once

#ifdef ENGINE_PLATFORM_LINUX

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv) {
    Engine::Log::Init();
    ENGINE_CORE_INFO("Log Initialized!\n");
    auto app = Engine::CreateApplication();
    app->Run();
    delete app;
}

#endif