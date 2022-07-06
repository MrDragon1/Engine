#pragma once

#ifdef ET_PLATFORM_LINUX

extern Ethereal::Application* Ethereal::CreateApplication();

int main(int argc, char** argv) {
    Ethereal::Log::Init();
    ET_CORE_INFO("Log Initialized!\n");
    auto app = Ethereal::CreateApplication();
    app->Run();
    delete app;
}

#endif