#include "imgui.h"
#include "opengl_shader.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdio.h>
#include <vector>

#include <GL/glew.h> // Initialize with glewInit()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#include <Core.Log.h>

#include "spdlog/spdlog.h"

import Lateralus.Core.Signal;
import Lateralus.Core.ByteConversion;
import Lateralus.Platform;
import Lateralus.Platform.iPlatform;
import Lateralus.Platform.iWindow;

#define PI 3.14159265358979323846

std::string ReadFileToMemory(const std::string &filename)
{
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::stringstream file_stream;
    try
    {
        file.open(filename.c_str());
        file_stream << file.rdbuf();
        file.close();
    }
    catch (std::ifstream::failure e)
    {
        // LATERALUS_LOG("Error reading Shader File!");
    }
    return file_stream.str();
}

void render_conan_logo()
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    float sz = 300.0f;
    static ImVec4 col1 = ImVec4(68.0 / 255.0, 83.0 / 255.0, 89.0 / 255.0, 1.0f);
    static ImVec4 col2 = ImVec4(40.0 / 255.0, 60.0 / 255.0, 80.0 / 255.0, 1.0f);
    static ImVec4 col3 = ImVec4(50.0 / 255.0, 65.0 / 255.0, 82.0 / 255.0, 1.0f);
    static ImVec4 col4 = ImVec4(20.0 / 255.0, 40.0 / 255.0, 60.0 / 255.0, 1.0f);
    const ImVec2 p = ImGui::GetCursorScreenPos();
    float x = p.x + 4.0f, y = p.y + 4.0f;
    draw_list->AddQuadFilled(ImVec2(x, y + 0.25 * sz), ImVec2(x + 0.5 * sz, y + 0.5 * sz),
                             ImVec2(x + sz, y + 0.25 * sz), ImVec2(x + 0.5 * sz, y), ImColor(col1));
    draw_list->AddQuadFilled(ImVec2(x, y + 0.25 * sz), ImVec2(x + 0.5 * sz, y + 0.5 * sz),
                             ImVec2(x + 0.5 * sz, y + 1.0 * sz), ImVec2(x, y + 0.75 * sz),
                             ImColor(col2));
    draw_list->AddQuadFilled(ImVec2(x + 0.5 * sz, y + 0.5 * sz), ImVec2(x + sz, y + 0.25 * sz),
                             ImVec2(x + sz, y + 0.75 * sz), ImVec2(x + 0.5 * sz, y + 1.0 * sz),
                             ImColor(col3));
    draw_list->AddLine(ImVec2(x + 0.75 * sz, y + 0.375 * sz), ImVec2(x + 0.75 * sz, y + 0.875 * sz),
                       ImColor(col4));
    draw_list->AddBezierCurve(ImVec2(x + 0.72 * sz, y + 0.24 * sz),
                              ImVec2(x + 0.68 * sz, y + 0.15 * sz),
                              ImVec2(x + 0.48 * sz, y + 0.13 * sz),
                              ImVec2(x + 0.39 * sz, y + 0.17 * sz), ImColor(col4), 10, 18);
    draw_list->AddBezierCurve(ImVec2(x + 0.39 * sz, y + 0.17 * sz),
                              ImVec2(x + 0.2 * sz, y + 0.25 * sz),
                              ImVec2(x + 0.3 * sz, y + 0.35 * sz),
                              ImVec2(x + 0.49 * sz, y + 0.38 * sz), ImColor(col4), 10, 18);
}

void create_triangle(unsigned int &vbo, unsigned int &vao, unsigned int &ebo)
{

    // create the triangle
    float triangle_vertices[] = {
        0.0f,   0.25f,  0.0f, // position vertex 1
        1.0f,   0.0f,   0.0f, // color vertex 1
        0.25f,  -0.25f, 0.0f, // position vertex 1
        0.0f,   1.0f,   0.0f, // color vertex 1
        -0.25f, -0.25f, 0.0f, // position vertex 1
        0.0f,   0.0f,   1.0f, // color vertex 1
    };
    unsigned int triangle_indices[] = {0, 1, 2};
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices,
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main(int, char **)
{
    using namespace std;
    using namespace Lateralus;
    using namespace Lateralus::Platform;

    spdlog::set_level(static_cast<spdlog::level::level_enum>(SPDLOG_ACTIVE_LEVEL));

    // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] %^[%l] %s(%#):%$ %v");
    LOG_INFO("Starting log.");
    spdlog::set_pattern("[%H:%M:%S] %^[%l] %s(%#):%$ %v");

    LOG_INFO("1234 : {}", Bytes_to_String(1234));

    LOG_INFO("123456 : {}", Bytes_to_String(123456));

    LOG_INFO("1024*1024: {}", Bytes_to_String(1024 * 1024));

    LOG_INFO("123: {}", Bytes_to_String(123));

    shared_ptr<Lateralus::Platform::iPlatform> platform = Lateralus::Platform::CreatePlatform();
    if (platform == nullptr)
    {
        LOG_CRITICAL_ALWAYS("Unsupported platform.");
        return 1;
    }
    shared_ptr<Lateralus::Platform::iWindow> window = Lateralus::Platform::CreateWindow();
    if (window == nullptr)
    {
        LOG_CRITICAL_ALWAYS("Unsupported platform.");
        return 2;
    }

    if (auto err = platform->Init(); err.has_value())
    {
        LOG_CRITICAL_ALWAYS("Error initializing platform: {}", err.value().GetErrorMessage());
        return 3;
    }

    Lateralus::Platform::WindowCreateContext windowCreateContext(platform);
    if (auto err = window->Create(windowCreateContext); err.has_value())
    {
        LOG_CRITICAL_ALWAYS("Error creating window: {}", err.value().GetErrorMessage());
        return 4;
    }

    // create our geometries
    unsigned int vbo, vao, ebo;
    create_triangle(vbo, vao, ebo);

    // init shader
    Shader triangle_shader;
    triangle_shader.init(ReadFileToMemory("Assets/simple-shader.vs"),
                         ReadFileToMemory("Assets/simple-shader.fs"));

    // Setup Dear ImGui context

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();

    // display a first frame quickly (better than a blank frame)

    char buff[2048] = {0};
    while (!window->ShouldClose())
    {
        window->PollEvents();

        window->Clear();

        window->NewFrame();

        // rendering our geometries
        triangle_shader.use();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // render your GUI
        ImGui::Begin("Triangle Position/Color");
        static float rotation = 0.0;
        ImGui::SliderFloat("rotation", &rotation, 0, 2 * PI);
        static float translation[] = {0.0, 0.0};
        ImGui::SliderFloat2("position", translation, -1.0, 1.0);
        static float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        // pass the parameters to the shader
        triangle_shader.setUniform("rotation", rotation);
        triangle_shader.setUniform("translation", translation[0], translation[1]);
        // color picker
        ImGui::ColorEdit3("color", color);
        // multiply triangle's color with this color
        triangle_shader.setUniform("color", color[0], color[1], color[2]);
        ImGui::End();

        ImGui::Begin("Conan logo");
        // render_conan_logo();

        ImGui::InputTextMultiline("Some input", buff, 2048);
        ImGui::End();

        ImGui::ShowStyleEditor();

        window->Render();
        window->SwapBuffers();
    }

    window.reset();
    platform.reset();

    return 0;
}
