module;
#if ENABLE_IMGUI
// License from: https://github.com/ocornut/imgui/blob/master/LICENSE.txt
// Source code from: https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_opengl3.cpp
/*
The MIT License (MIT)

Copyright (c) 2014-2022 Omar Cornut

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Modifications by Jared Thomson in 2022 are mostly stylistic to fit within the
// Lateralus project. Modifications include:
// * C++20 module support (removal of header, exporting public functions)
// * Moving functionality and state into a class (ImplOpenGL)

//----------------------------------------
// OpenGL    GLSL      GLSL
// version   version   string
//----------------------------------------
//  2.0       110       "#version 110"
//  2.1       120       "#version 120"
//  3.0       130       "#version 130"
//  3.1       140       "#version 140"
//  3.2       150       "#version 150"
//  3.3       330       "#version 330 core"
//  4.0       400       "#version 400 core"
//  4.1       410       "#version 410 core"
//  4.2       420       "#version 410 core"
//  4.3       430       "#version 430 core"
//  ES 2.0    100       "#version 100"      = WebGL 1.0
//  ES 3.0    300       "#version 300 es"   = WebGL 2.0
//----------------------------------------

#if PLATFORM_WIN64
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include "imgui.h"
#include <GL/glew.h> // Needs to be initialized with glewInit() in user's code
#include <stdint.h>  // intptr_t
#include <stdio.h>

#endif

export module Lateralus.Platform.ImGui.OpenGL;

#if ENABLE_IMGUI

import Lateralus.Platform.ImGui.Impl;
import Lateralus.Platform.Error;

#pragma warning(push)
// Warning	C4005 '_CRT_INSECURE_DEPRECATE': macro redefinition
#pragma warning(disable : 4005)
import <format>;
import <optional>;
import <string>;
#pragma warning(pop)

using namespace std;

namespace Lateralus::Platform::ImGui
{
export class ImplOpenGL : public iImpl
{
public:
    ImplOpenGL() = default;
    virtual ~ImplOpenGL() = default;

    optional<Error> Init(const char *glsl_version)
    {
        if (glsl_version == nullptr)
        {
            return Error("null glsl version string.");
        }

        m_GlslVersionString = glsl_version;
        if (m_GlslVersionString.empty())
        {
            return Error("empty glsl version string.");
        }
        m_GlslVersionString += "\n";

        return Init();
    }
    void Shutdown() override { DestroyDeviceObjects(); }

    void Render() override { RenderDrawData(::ImGui::GetDrawData()); }

    bool CreateFontsTexture()
    {
        // Build texture atlas
        ImGuiIO &io = ::ImGui::GetIO();
        unsigned char *pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(
            &pixels, &width,
            &height); // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so
                      // small) because it is more likely to be compatible with user's existing
                      // shaders. If your ImTextureId represent a higher-level concept than just a
                      // GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU
                      // memory.

        // Upload texture to graphics system
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGenTextures(1, &m_FontTexture);
        glBindTexture(GL_TEXTURE_2D, m_FontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef GL_UNPACK_ROW_LENGTH
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     pixels);

        // Store our identifier
        io.Fonts->TexID = (ImTextureID)(intptr_t)m_FontTexture;

        // Restore state
        glBindTexture(GL_TEXTURE_2D, last_texture);

        return true;
    }

    void DestroyFontsTexture()
    {
        if (m_FontTexture)
        {
            ImGuiIO &io = ::ImGui::GetIO();
            glDeleteTextures(1, &m_FontTexture);
            io.Fonts->TexID = 0;
            m_FontTexture = 0;
        }
    }

    bool CreateDeviceObjects()
    {
        // Backup GL state
        GLint last_texture, last_array_buffer;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
#ifndef IMGUI_IMPL_OPENGL_ES2
        GLint last_vertex_array;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
#endif

        const GLchar *vertex_shader_glsl_130 =
            "uniform mat4 ProjMtx;\n"
            "in vec2 Position;\n"
            "in vec2 UV;\n"
            "in vec4 Color;\n"
            "out vec2 Frag_UV;\n"
            "out vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "    Frag_UV = UV;\n"
            "    Frag_Color = Color;\n"
            "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
            "}\n";

        const GLchar *fragment_shader_glsl_130 =
            "uniform sampler2D Texture;\n"
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Color;\n"
            "out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
            "}\n";

        // Select shaders matching our GLSL versions
        const GLchar *vertex_shader = vertex_shader_glsl_130;
        const GLchar *fragment_shader = fragment_shader_glsl_130;

        // Create shaders
        const GLchar *vertex_shader_with_version[2] = {m_GlslVersionString.c_str(), vertex_shader};
        m_VertHandle = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(m_VertHandle, 2, vertex_shader_with_version, NULL);
        glCompileShader(m_VertHandle);
        CheckShader(m_VertHandle, "vertex shader");

        const GLchar *fragment_shader_with_version[2] = {m_GlslVersionString.c_str(),
                                                         fragment_shader};
        m_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(m_FragHandle, 2, fragment_shader_with_version, NULL);
        glCompileShader(m_FragHandle);
        CheckShader(m_FragHandle, "fragment shader");

        m_ShaderHandle = glCreateProgram();
        glAttachShader(m_ShaderHandle, m_VertHandle);
        glAttachShader(m_ShaderHandle, m_FragHandle);
        glLinkProgram(m_ShaderHandle);
        CheckProgram(m_ShaderHandle, "shader program");

        m_AttribLocationTex = glGetUniformLocation(m_ShaderHandle, "Texture");
        m_AttribLocationProjMtx = glGetUniformLocation(m_ShaderHandle, "ProjMtx");
        m_AttribLocationVtxPos = glGetAttribLocation(m_ShaderHandle, "Position");
        m_AttribLocationVtxUV = glGetAttribLocation(m_ShaderHandle, "UV");
        m_AttribLocationVtxColor = glGetAttribLocation(m_ShaderHandle, "Color");

        // Create buffers
        glGenBuffers(1, &m_VboHandle);
        glGenBuffers(1, &m_ElementsHandle);

        CreateFontsTexture();

        // Restore modified GL state
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBindVertexArray(last_vertex_array);

        return true;
    }

    void DestroyDeviceObjects()
    {
        if (m_VboHandle)
        {
            glDeleteBuffers(1, &m_VboHandle);
            m_VboHandle = 0;
        }
        if (m_ElementsHandle)
        {
            glDeleteBuffers(1, &m_ElementsHandle);
            m_ElementsHandle = 0;
        }
        if (m_ShaderHandle && m_VertHandle)
        {
            glDetachShader(m_ShaderHandle, m_VertHandle);
        }
        if (m_ShaderHandle && m_FragHandle)
        {
            glDetachShader(m_ShaderHandle, m_FragHandle);
        }
        if (m_VertHandle)
        {
            glDeleteShader(m_VertHandle);
            m_VertHandle = 0;
        }
        if (m_FragHandle)
        {
            glDeleteShader(m_FragHandle);
            m_FragHandle = 0;
        }
        if (m_ShaderHandle)
        {
            glDeleteProgram(m_ShaderHandle);
            m_ShaderHandle = 0;
        }

        DestroyFontsTexture();
    }

private:
    optional<Error> Init() override
    {
        if (m_GlslVersionString.empty())
        {
            return Error("Missing glsl version string.");
        }

        // Query for GL version
        GLint major, minor;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        m_GlVersion = major * 1000 + minor;

        // Setup back-end capabilities flags
        ImGuiIO &io = ::ImGui::GetIO();
        io.BackendRendererName = "Lateralus::Platform::ImGui::OpenGL";

        if (!m_ShaderHandle)
        {
            CreateDeviceObjects();
        }

        return Success;
    }

    // OpenGL3 Render function.
    // (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now
    // call this directly from your main loop) Note that this implementation is little
    // overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly, in
    // order to be able to run within any OpenGL engine that doesn't do so.
    void RenderDrawData(ImDrawData *draw_data)
    {
        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates
        // != framebuffer coordinates)
        int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
        int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
            return;

        // Backup GL state
        GLenum last_active_texture;
        glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint *)&last_active_texture);
        glActiveTexture(GL_TEXTURE0);
        GLint last_program;
        glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
#ifdef GL_SAMPLER_BINDING
        GLint last_sampler;
        glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
#endif
        GLint last_array_buffer;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        GLint last_vertex_array_object;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array_object);

#ifdef GL_POLYGON_MODE
        GLint last_polygon_mode[2];
        glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
        GLint last_viewport[4];
        glGetIntegerv(GL_VIEWPORT, last_viewport);
        GLint last_scissor_box[4];
        glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
        GLenum last_blend_src_rgb;
        glGetIntegerv(GL_BLEND_SRC_RGB, (GLint *)&last_blend_src_rgb);
        GLenum last_blend_dst_rgb;
        glGetIntegerv(GL_BLEND_DST_RGB, (GLint *)&last_blend_dst_rgb);
        GLenum last_blend_src_alpha;
        glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint *)&last_blend_src_alpha);
        GLenum last_blend_dst_alpha;
        glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint *)&last_blend_dst_alpha);
        GLenum last_blend_equation_rgb;
        glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint *)&last_blend_equation_rgb);
        GLenum last_blend_equation_alpha;
        glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint *)&last_blend_equation_alpha);
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
        bool clip_origin_lower_left = true;
#if defined(GL_CLIP_ORIGIN)
        GLenum last_clip_origin = 0;
        glGetIntegerv(
            GL_CLIP_ORIGIN,
            (GLint *)&last_clip_origin); // Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
        if (last_clip_origin == GL_UPPER_LEFT)
            clip_origin_lower_left = false;
#endif

        // Setup desired GL state
        // Recreate the VAO every time (this is to easily allow multiple GL contexts to be rendered
        // to. VAO are not shared among GL contexts) The renderer would actually work without any
        // VAO bound, but then our VertexAttrib calls would overwrite the default one currently
        // bound.
        GLuint vertex_array_object = 0;
        glGenVertexArrays(1, &vertex_array_object);
        SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);

        // Will project scissor/clipping rectangles into framebuffer space
        ImVec2 clip_off = draw_data->DisplayPos; // (0,0) unless using multi-viewports
        ImVec2 clip_scale =
            draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

        // Render command lists
        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList *cmd_list = draw_data->CmdLists[n];

            // Upload vertex/index buffers
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
                         (const GLvoid *)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
                         (const GLvoid *)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback != NULL)
                {
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user
                    // to request the renderer to reset render state.)
                    if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                        SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
                    else
                        pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    // Project scissor/clipping rectangles into framebuffer space
                    ImVec4 clip_rect;
                    clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                    clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                    clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                    clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                    if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f &&
                        clip_rect.w >= 0.0f)
                    {
                        // Apply scissor/clipping rectangle
                        if (clip_origin_lower_left)
                            glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w),
                                      (int)(clip_rect.z - clip_rect.x),
                                      (int)(clip_rect.w - clip_rect.y));
                        else
                            glScissor((int)clip_rect.x, (int)clip_rect.y, (int)clip_rect.z,
                                      (int)clip_rect.w); // Support for GL 4.5 rarely used
                                                         // glClipControl(GL_UPPER_LEFT)

                        // Bind texture, Draw
                        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                        glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
                                       sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                                       (void *)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
                    }
                }
            }
        }

        // Destroy the temporary VAO
        glDeleteVertexArrays(1, &vertex_array_object);

        // Restore modified GL state
        glUseProgram(last_program);
        glBindTexture(GL_TEXTURE_2D, last_texture);
#ifdef GL_SAMPLER_BINDING
        glBindSampler(0, last_sampler);
#endif
        glActiveTexture(last_active_texture);
        glBindVertexArray(last_vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
        glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha,
                            last_blend_dst_alpha);
        if (last_enable_blend)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
        if (last_enable_cull_face)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
        if (last_enable_depth_test)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
        if (last_enable_scissor_test)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
        glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
        glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2],
                   (GLsizei)last_viewport[3]);
        glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2],
                  (GLsizei)last_scissor_box[3]);
    }

    void SetupRenderState(ImDrawData *draw_data, int fb_width, int fb_height,
                          GLuint vertex_array_object)
    {
        // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor
        // enabled, polygon fill
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

        // Setup viewport, orthographic projection matrix
        // Our visible imgui space lies from draw_data->DisplayPos (top left) to
        // draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for
        // single viewport apps.
        glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
        float L = draw_data->DisplayPos.x;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T = draw_data->DisplayPos.y;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        const float ortho_projection[4][4] = {
            {2.0f / (R - L), 0.0f, 0.0f, 0.0f},
            {0.0f, 2.0f / (T - B), 0.0f, 0.0f},
            {0.0f, 0.0f, -1.0f, 0.0f},
            {(R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f},
        };
        glUseProgram(m_ShaderHandle);
        glUniform1i(m_AttribLocationTex, 0);
        glUniformMatrix4fv(m_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
#ifdef GL_SAMPLER_BINDING
        glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may
                             // set that otherwise.
#endif

        glBindVertexArray(vertex_array_object);

        // Bind vertex/index buffers and setup attributes for ImDrawVert
        glBindBuffer(GL_ARRAY_BUFFER, m_VboHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementsHandle);
        glEnableVertexAttribArray(m_AttribLocationVtxPos);
        glEnableVertexAttribArray(m_AttribLocationVtxUV);
        glEnableVertexAttribArray(m_AttribLocationVtxColor);
        glVertexAttribPointer(m_AttribLocationVtxPos, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                              (GLvoid *)IM_OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(m_AttribLocationVtxUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
                              (GLvoid *)IM_OFFSETOF(ImDrawVert, uv));
        glVertexAttribPointer(m_AttribLocationVtxColor, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                              sizeof(ImDrawVert), (GLvoid *)IM_OFFSETOF(ImDrawVert, col));
    }

    // If you get an error please report on github. You may try different GL context version or GLSL
    // version. See GL<>GLSL version table at the top of this file.
    bool CheckShader(GLuint handle, const char *desc)
    {
        GLint status = 0, log_length = 0;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if ((GLboolean)status == GL_FALSE)
            fprintf(stderr, "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects: failed to compile %s!\n",
                    desc);
        if (log_length > 1)
        {
            ImVector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetShaderInfoLog(handle, log_length, NULL, (GLchar *)buf.begin());
            fprintf(stderr, "%s\n", buf.begin());
        }
        return (GLboolean)status == GL_TRUE;
    }

    // If you get an error please report on GitHub. You may try different GL context version or GLSL
    // version.
    bool CheckProgram(GLuint handle, const char *desc)
    {
        GLint status = 0, log_length = 0;
        glGetProgramiv(handle, GL_LINK_STATUS, &status);
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if ((GLboolean)status == GL_FALSE)
            fprintf(stderr,
                    "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects: failed to link %s! (with GLSL "
                    "'%s')\n",
                    desc, m_GlslVersionString.c_str());
        if (log_length > 1)
        {
            ImVector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetProgramInfoLog(handle, log_length, NULL, (GLchar *)buf.begin());
            fprintf(stderr, "%s\n", buf.begin());
        }
        return (GLboolean)status == GL_TRUE;
    }

    GLuint m_GlVersion =
        0; // Extracted at runtime using GL_MAJOR_VERSION, GL_MINOR_VERSION queries.
    string m_GlslVersionString; // Specified by user in init.
    GLuint m_FontTexture = 0;
    GLuint m_ShaderHandle = 0, m_VertHandle = 0, m_FragHandle = 0;
    int m_AttribLocationTex = 0, m_AttribLocationProjMtx = 0; // Uniforms location
    int m_AttribLocationVtxPos = 0, m_AttribLocationVtxUV = 0,
        m_AttribLocationVtxColor = 0; // Vertex attributes location
    unsigned int m_VboHandle = 0, m_ElementsHandle = 0;
};
} // namespace Lateralus::Platform::ImGui

#endif