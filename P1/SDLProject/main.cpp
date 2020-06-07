#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <stdio.h>

// Loading Images
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix, earthMatrix, ssMatrix, star1Matrix, star2Matrix;

// Astronaut movement vars
float astronaut_x = 0.0f;
float astronaut_y = -2.0f;
float astronaut_rotate = 0;

// Earth movement vars
float earth_x = 3.0f;
float earth_y = 2.5f;
float earth_rotate = 0;

// Space Station movement vars
float ss_x = -2.0f;
float ss_y = 1.4f;
float ss_rotate = 0;

// Star movement vars
float star_1_x = 2.8f;
float star_1_y = -.5f;
float star_1_scale = 1.0f;

GLuint playerTextureID;
GLuint earthTextureID;
GLuint ssTextureID;
GLuint starTextureID;


GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n"; assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Textured!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    //program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Blen image with background
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // My TERRIBLE pixel art attemps
    playerTextureID = LoadTexture("astronaut.png");
    earthTextureID = LoadTexture("earth.png");
    ssTextureID = LoadTexture("space_station.png");
    starTextureID = LoadTexture("star.png");
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

float last_ticks = 0.0f;
float astronaut_speed = .8f;
float earth_speed = .1f;
float ss_speed = .1f;
float star_speed = 2.5f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float delta_time = ticks - last_ticks;
    last_ticks = ticks;

    // =====Astronaut move back and forth accross window=====
    // when we reach a certain value start subtracting or adding
    if(astronaut_x > (4.5)) {
        astronaut_speed *= -1.0f;
    }
    if(astronaut_x < (-4.5)) {
        astronaut_speed *= -1.0f;
    }
    
    astronaut_x += astronaut_speed * delta_time;
    astronaut_rotate += 90.0f * delta_time;
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(astronaut_x, astronaut_y, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f, 2.0f, 1.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(astronaut_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // =====Earth float up and down top right of window=====
    if(earth_y > (2.7)) {
        earth_speed *= -1.0f;
    }
    if(earth_y < (2.3)) {
        earth_speed *= -1.0f;
    }
    
    earth_y += earth_speed * delta_time;
    
    earthMatrix = glm::mat4(1.0f);
    earthMatrix = glm::translate(earthMatrix, glm::vec3(earth_x, earth_y, 1.0f));
    earthMatrix = glm::scale(earthMatrix, glm::vec3(4.0f, 4.0f, 1.0f));

    // =====Space station orbit top left window=====
    if(ss_y > (1.6)) {
        ss_speed *= -1.0f;
    }
    if(ss_y < (1.1)) {
        ss_speed *= -1.0f;
    }
    ss_y += ss_speed * delta_time;
    ss_rotate = 45.0f;
    
    ssMatrix = glm::mat4(1.0f);
    ssMatrix = glm::translate(ssMatrix, glm::vec3(ss_x, ss_y, 1.0f));
    ssMatrix = glm::scale(ssMatrix, glm::vec3(4.0f, 4.0f, 1.0f));
    ssMatrix = glm::rotate(ssMatrix, glm::radians(ss_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    
    // =====Star 1=====
    if(star_1_scale >= 5.0f) {
        star_speed *= -1.0f;
    }
    if(star_1_scale <= 0.0f) {
        star_speed *= -1.0f;
    }
    
    star_1_scale += star_speed * delta_time;
    
    star1Matrix = glm::mat4(1.0f);
    star1Matrix = glm::translate(star1Matrix, glm::vec3(star_1_x, star_1_y, 1.0f));
    star1Matrix = glm::scale(star1Matrix, glm::vec3(star_1_scale, star_1_scale, 1.0f));
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    
    // Draw astronaut
    program.SetModelMatrix(modelMatrix);
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Draw earth
    program.SetModelMatrix(earthMatrix);
    glBindTexture(GL_TEXTURE_2D, earthTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Draw Space Station
    program.SetModelMatrix(ssMatrix);
    glBindTexture(GL_TEXTURE_2D, ssTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Draw star 1
    program.SetModelMatrix(star1Matrix);
    glBindTexture(GL_TEXTURE_2D, starTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    
    Shutdown();
    return 0;
}
