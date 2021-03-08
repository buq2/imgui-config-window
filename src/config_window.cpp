#include "config_window.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <iostream>
#include <algorithm>
#include <SDL.h>

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING)
#include <glbinding/glbinding.h>  // Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

using namespace google::protobuf;

class ConfigWindowPrivate {
 public:
    SDL_Window* window;
    SDL_GLContext gl_context;
};

ConfigWindow::ConfigWindow(const std::string &title)
    :
      p_(new ConfigWindowPrivate),
      title_(title)
{
}

ConfigWindow::~ConfigWindow()
{
    delete p_;
    p_ = nullptr;
}

bool ConfigWindow::Initialize() {
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    //SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
        p_->window = SDL_CreateWindow(title_.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    p_->gl_context = SDL_GL_CreateContext(p_->window);
    SDL_GL_MakeCurrent(p_->window, p_->gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return false;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(p_->window, p_->gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void ConfigWindow::Uninit()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(p_->gl_context);
    SDL_DestroyWindow(p_->window);
    SDL_Quit();
}

bool ConfigWindow::Draw(google::protobuf::Message &msg)
{
    auto done = StartDraw();
    BuildGui(msg);
    EndDraw();
    return done;
}

bool ConfigWindow::StartDraw()
{
    bool done = false;
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            done = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(p_->window))
            done = true;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(p_->window);
    ImGui::NewFrame();
    ImGui::Begin(title_.c_str());
    return done;
}

void ConfigWindow::EndDraw()
{
    ImVec2 win_size;
    if (auto_resize_) {
        // Auto size
        ImGui::SetWindowPos(ImVec2(0,0));
        ImGui::SetWindowSize(ImVec2(0,0));
        win_size = ImGui::GetWindowSize();
    }

    ImGui::End();

    // Rendering
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(p_->window);


    if (auto_resize_) {
        int top, left, bottom, right;
        SDL_GetWindowBordersSize(p_->window, &top, &left, &bottom, &right);
        int w = std::max<int>(50, win_size.x + left + right);
        int h = std::max<int>(50, win_size.y + top + bottom);
        SDL_SetWindowSize(p_->window, w, h);
    }
}

bool StringToEnumValueDescriptor(const std::string &enum_str,
    const EnumDescriptor *enum_descriptor,
    const EnumValueDescriptor **enum_value_descriptor)
{
    const int num = enum_descriptor->value_count();
    for (int i = 0; i < num; ++i) {
        const EnumValueDescriptor *tmp_desc = enum_descriptor->value(i);
        if (tmp_desc->full_name() == enum_str) {
            *enum_value_descriptor = tmp_desc;
            return true;
        }
    }
    return false;
}

void ProtobufMessageToGui(google::protobuf::Message &msg);


void ProtobufRepeatedFieldToGui(Message &msg, const Reflection *reflection,
                                const FieldDescriptor *fdesc)
{
    // TODO: Implement
}

void ProtobufFieldToGui(Message &msg, const Reflection *reflection,
                        const FieldDescriptor *fdesc) {
    //If this field is type "repeated", we need to use different function
    //for creating the data
    if (fdesc->is_repeated()) {
        if (ImGui::CollapsingHeader(fdesc->name().c_str())) {
            ImGui::Indent();
            // TODO ProtobufRepeatedFieldToGui(msg, reflection, fdesc);
            ImGui::Unindent();
        }
        return;
    }

    //We need to check if this field is not set and it does not have
    //default value.
    const bool has_field = reflection->HasField(msg, fdesc);
    const bool has_default = fdesc->has_default_value();
    if (!has_field && !has_default && fdesc->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE) {
        //No need to do anything (optional value has not been set)
        return;
    }

    //Check if field has not been set (and we need to use default value)
    const bool use_default = has_default && !has_field;

    //Now we know that the field has been set, and that it is not repeated,
    //we can create the correct output
    const std::string field_name = fdesc->name();
    const FieldDescriptor::CppType field_type = fdesc->cpp_type();

#define SET_VALUE(type, type_lower, type_upper, type_imgui) {\
        type val = use_default ? fdesc->default_value_ ## type_lower() : reflection->Get ## type_upper(msg, fdesc); \
        ImGui::Input ## type_imgui(field_name.c_str(), &val); \
        if (!use_default || fdesc->default_value_ ## type_lower() != val) { \
            reflection->Set ## type_upper(&msg, fdesc, val); \
        } \
        return; \
    }

    switch(field_type) {
        case FieldDescriptor::CPPTYPE_INT32:
            SET_VALUE(int, int32, Int32, Int)
        case FieldDescriptor::CPPTYPE_INT64:
            SET_VALUE(int, int64, Int64, Int)
        case FieldDescriptor::CPPTYPE_UINT32:
            SET_VALUE(int, uint32, UInt32, Int)
        case FieldDescriptor::CPPTYPE_UINT64:
            SET_VALUE(int, uint64, UInt64, Int)
        case FieldDescriptor::CPPTYPE_DOUBLE:
            SET_VALUE(double, double, Double, Double)
        case FieldDescriptor::CPPTYPE_FLOAT:
            SET_VALUE(float, float, Float, Float)
        case FieldDescriptor::CPPTYPE_ENUM:
            {
                const auto val_enum = use_default ? fdesc->default_value_enum() : reflection->GetEnum(msg,fdesc);
                auto val = val_enum->full_name();
                const auto res_size = std::max<unsigned int>(1024, val.size());
                val.reserve(res_size);
                ImGui::InputText(field_name.c_str(), &val[0], res_size);
                val = std::string(val.c_str());

                const EnumValueDescriptor *evdesc = nullptr;
                if (StringToEnumValueDescriptor(val, fdesc->enum_type(), &evdesc)) {
                    if (!use_default || val_enum != evdesc) {
                        reflection->SetEnum(&msg, fdesc, evdesc);
                    }
                }

                return;
            }
        case FieldDescriptor::CPPTYPE_STRING:
            {
                auto val = use_default ? fdesc->default_value_string() : reflection->GetString(msg,fdesc);
                const auto res_size = std::max<unsigned int>(1024, val.size());
                val.reserve(res_size);
                ImGui::InputText(field_name.c_str(), &val[0], res_size);
                val = std::string(val.c_str());
                if (!use_default || val != fdesc->default_value_string()) {
                    reflection->SetString(&msg, fdesc, val.c_str());
                }

                return;
            }
        case FieldDescriptor::CPPTYPE_BOOL:
            {
                bool val = use_default ? fdesc->default_value_bool() : reflection->GetBool(msg, fdesc);
                ImGui::Checkbox(field_name.c_str(), &val);
                if (!use_default || val != fdesc->default_value_bool()) {
                    reflection->SetBool(&msg, fdesc, val);
                }

                return;
            }
        case FieldDescriptor::CPPTYPE_MESSAGE:
            if (ImGui::CollapsingHeader(field_name.c_str())) {
                ImGui::Indent();
                auto new_msg = reflection->MutableMessage(&msg, fdesc);
                ProtobufMessageToGui(*new_msg);
                ImGui::Unindent();
                return;
            }
        default:
            //Unknown type
            return;
    }
}

void ProtobufMessageToGui(google::protobuf::Message &msg)
{

    auto ref = msg.GetReflection();
    auto *desc = msg.GetDescriptor();
    auto num_fields = desc->field_count();

    //Iterate trough all fields
    for (int i = 0; i < num_fields; ++i) {
        const FieldDescriptor *fdesc = desc->field(i);
        ProtobufFieldToGui(msg, ref, fdesc);
    }
}

void ConfigWindow::BuildGui(google::protobuf::Message &msg)
{
    ProtobufMessageToGui(msg);
}
