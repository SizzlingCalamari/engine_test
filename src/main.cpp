
#include <iostream>
#include <cassert>

#include "World.h"

#include "SDLWrap.h"

#include "Renderer/Renderer.h"
#include "Renderer/3DRenderer/3DRenderer.h"
#include "Renderer/3DRenderer/Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "SOIL.h"

static const float vertex_data[] = 
{
    -1.0f, -1.0f, -1.0f, // triangle 1 : begin
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, // triangle 1 : end
    1.0f, 1.0f, -1.0f, // triangle 2 : begin
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f, // triangle 2 : end
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
};

static const float colour_data[] =
{
    0.583f, 0.771f, 0.014f,
    0.609f, 0.115f, 0.436f,
    0.327f, 0.483f, 0.844f,
    0.822f, 0.569f, 0.201f,
    0.435f, 0.602f, 0.223f,
    0.310f, 0.747f, 0.185f,
    0.597f, 0.770f, 0.761f,
    0.559f, 0.436f, 0.730f,
    0.359f, 0.583f, 0.152f,
    0.483f, 0.596f, 0.789f,
    0.559f, 0.861f, 0.639f,
    0.195f, 0.548f, 0.859f,
    0.014f, 0.184f, 0.576f,
    0.771f, 0.328f, 0.970f,
    0.406f, 0.615f, 0.116f,
    0.676f, 0.977f, 0.133f,
    0.971f, 0.572f, 0.833f,
    0.140f, 0.616f, 0.489f,
    0.997f, 0.513f, 0.064f,
    0.945f, 0.719f, 0.592f,
    0.543f, 0.021f, 0.978f,
    0.279f, 0.317f, 0.505f,
    0.167f, 0.620f, 0.077f,
    0.347f, 0.857f, 0.137f,
    0.055f, 0.953f, 0.042f,
    0.714f, 0.505f, 0.345f,
    0.783f, 0.290f, 0.734f,
    0.722f, 0.645f, 0.174f,
    0.302f, 0.455f, 0.848f,
    0.225f, 0.587f, 0.040f,
    0.517f, 0.713f, 0.338f,
    0.053f, 0.959f, 0.120f,
    0.393f, 0.621f, 0.362f,
    0.673f, 0.211f, 0.457f,
    0.820f, 0.883f, 0.371f,
    0.982f, 0.099f, 0.879f
};

static const float tri_data[] =
{
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f
};

static const float tri_colour[] =
{
    0.583f, 0.771f, 0.014f,
    0.609f, 0.115f, 0.436f,
    0.327f, 0.483f, 0.844f
};

static const float tri_uv[] =
{
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f
};

int main(int argc, const char *argv[])
{
    std::cout << "Hello World!" << std::endl;

    SDLWrap sdl;
    sdl.Init(INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDLWindow window = sdl.CreateWindow("JORDAN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    auto renderer = Renderer::CreateAndInit3DRenderer(sdl.CreateGLContext(&window));

    Camera cam;
    cam.CalcProjection();

    auto box_transform = glm::translate(glm::vec3(-3.0f, 0.0f, 0.0f))
        * glm::rotate(30.0f, glm::vec3(0, 0, 1.0f))
        * glm::scale(glm::vec3(1.0f));

    Renderable box;
    box.SetTransform(box_transform);
    box.LoadVerticies(vertex_data, sizeof(vertex_data) / 3 / sizeof(vertex_data[0]));

    Renderable tri;
    tri.SetTransform(glm::mat4());
    tri.LoadVerticies(tri_data, sizeof(tri_data) / 3 / sizeof(tri_data[0]));

    Scene scene;
    scene.AddObject(&box);
    scene.AddObject(&tri);

    //GLuint tex = SOIL_load_OGL_texture(
    //    "../textures/jesusbond_feelingfresh.jpg", SOIL_LOAD_AUTO,
    //    SOIL_CREATE_NEW_ID, SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
    /*
    GLuint VertexArrays[2];
    glGenVertexArrays(sizeof(VertexArrays) / sizeof(GLuint), VertexArrays);

    GLuint buffers[4];
    glGenBuffers(sizeof(buffers) / sizeof(GLuint), buffers);

    glBindVertexArray(VertexArrays[0]);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(colour_data), colour_data, GL_STREAM_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindVertexArray(0);

    glBindVertexArray(VertexArrays[1]);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tri_data), tri_data, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(tri_uv), tri_uv, GL_STATIC_DRAW);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glBindVertexArray(0);
    */
    World w;
    w.Initialize();

    const uint SERVER_FRAME_DT = 10000;

    while (sdl.ProcessEvents())
    {
        w.Update(SERVER_FRAME_DT);
        /*
        GLContext::SetActiveProgram(&simple);
            glBindVertexArray(VertexArrays[0]);
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
                    //glBufferData(GL_ARRAY_BUFFER, sizeof(colour_data), nullptr, GL_STREAM_DRAW);
                    {
                        GLfloat *c = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
                        size_t count = sizeof(colour_data)/sizeof(colour_data[0]);
                        for (size_t i = 0; i < count; ++i)
                        {
                            c[i] = c[(i+1)%count];
                        }
                        glUnmapBuffer(GL_ARRAY_BUFFER);
                    }
                glDrawArrays(GL_TRIANGLES, 0, sizeof(vertex_data) / 3 / sizeof(vertex_data[0]));
            glBindVertexArray(0);
        glUseProgram(0);
        GLContext::SetActiveProgram(&texture);
            glBindVertexArray(VertexArrays[1]);
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPTRI[0][0]);
                glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, tex);
                    glUniform1i(TextureID, 0);
                glDrawArrays(GL_TRIANGLES, 0, sizeof(tri_data) / 3 / sizeof(tri_data[0]));
            glBindVertexArray(0);
        glUseProgram(0);
        */

        renderer->RenderScene(&cam, &scene);
        window.SwapBuffers();
    }

    renderer->Shutdown();
    Renderer::FreeRenderer(renderer);
    w.Shutdown();
    /*
    glDeleteBuffers(sizeof(buffers) / sizeof(GLuint), buffers);
    glDeleteVertexArrays(sizeof(VertexArrays) / sizeof(GLuint), VertexArrays);
    glDeleteTextures(1, &tex);
    */
    sdl.Shutdown();
}
