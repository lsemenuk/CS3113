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
glm::mat4 viewMatrix, player1Matrix, player2Matrix, pongBallMatrix, projectionMatrix;

// Paddle width and height
float PADDLE_WIDTH = .2f;
float PADDLE_HEIGHT = 1.0f;

// Pong ball init
glm::vec3 pong_ball_position = glm::vec3(0,0,0);
float pong_ball_speed = 2.0f;
float ball_xdir = 1.0f;
float ball_ydir = 1.0f;

// Pong ball width and height
float PONG_BALL_WIDTH = .5f;
float PONG_BALL_HEIGHT = .5f;


// Player 1 movement vars
glm::vec3 player1_position = glm::vec3(-4.5f, 0, 0);
// Dont move player yet
glm::vec3 player1_movement = glm::vec3(0,0,0);
float player1_speed = 4.0f;

// Player 2 movement vars
glm::vec3 player2_position = glm::vec3(4.5f, 0, 0);
// Dont move player yet
glm::vec3 player2_movement = glm::vec3(0,0,0);
float player2_speed = 4.0f;

GLuint playerTextureID;
GLuint pongBallTextureID;

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
    displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix =    glm::mat4(1.0f);
    player1Matrix = glm::mat4(1.0f);
    player2Matrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Blend image with background
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    playerTextureID = LoadTexture("paddle.png");
    pongBallTextureID = LoadTexture("ball.png");
}

void ProcessInput() {
    player1_movement = glm::vec3(0);
    player2_movement = glm::vec3(0);

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_UP:
                        // Move player1 up
                        player2_movement.y = 1.0f;
                        break;
                    case SDLK_DOWN:
                        // Move player1 down
                        player2_movement.y = -1.0f;
                        break;
                    case SDLK_w:
                        // Move player2 up
                        player1_movement.y = 1.0f;
                        break;
                    case SDLK_s:
                        // Move player2 down
                        player1_movement.y = -1.0f;
                }
                break;
         }
     }
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_UP]) {
        player2_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        player2_movement.y = -1.0f;
    }
    
    if (keys[SDL_SCANCODE_W]) {
        player1_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S]) {
        player1_movement.y = -1.0f;
    }
 }

bool ball_collide_paddle() {
    float xdist1 = fabs(pong_ball_position.x - player1_position.x) - ((PONG_BALL_WIDTH + PADDLE_WIDTH) / 2.0f);
    float ydist1 = fabs(pong_ball_position.y - player1_position.y) - ((PONG_BALL_HEIGHT + PADDLE_HEIGHT) / 2.0f);

    float xdist2 = fabs(pong_ball_position.x - player2_position.x) - ((PONG_BALL_WIDTH + PADDLE_WIDTH) / 2.0f);
    float ydist2 = fabs(pong_ball_position.y - player2_position.y) - ((PONG_BALL_HEIGHT + PADDLE_HEIGHT) / 2.0f);

    if(xdist1 < 0 && ydist1 < 0) {
        printf("COLLIDING WITH P1\n");
        return true;
    }
    if(xdist2 < 0 && ydist2 < 0) {
        printf("COLLIDING WITH P2\n");
        return true;
    }
    return false;
}

bool ball_collide_wall() {
    if(pong_ball_position.y > 3.65f || pong_ball_position.y < -3.75f) {
        printf("HIT top or bottom WALL\n");
        return true;
    }
    return false;
}

float lastTicks = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    // Player1 movement
    // Add (direction * units per second * elapsed time)
    player1_position += player1_movement * player1_speed * deltaTime;
    player1Matrix = glm::mat4(1.0f);
    player1Matrix = glm::translate(player1Matrix, player1_position);
    player1Matrix = glm::scale(player1Matrix, glm::vec3(4.0f, 6.0f, 1.0f));
    
    // player 1 stay within bounds of window
    if(player1_position.y > 3.2f) {player1_position.y = 3.2f;}
    if(player1_position.y < -3.0f) {player1_position.y = -3.0f;}
    
    // Player2 movement
    // Add (direction * units per second * elapsed time)
    player2_position += player2_movement * player2_speed * deltaTime;
    player2Matrix = glm::mat4(1.0f);
    player2Matrix = glm::translate(player2Matrix, player2_position);
    player2Matrix = glm::scale(player2Matrix, glm::vec3(4.0f, 6.0f, 1.0f));
    
    // player 2 stay within bounds of window
    if(player2_position.y > 3.2f) {player2_position.y = 3.2f;}
    if(player2_position.y < -3.0f) {player2_position.y = -3.0f;}
    
    //Pong ball logic
    pong_ball_position.x += ball_xdir * pong_ball_speed * deltaTime;
    pong_ball_position.y += ball_ydir * pong_ball_speed * deltaTime;
    pongBallMatrix = glm::mat4(1.0f);
    pongBallMatrix = glm::translate(pongBallMatrix, pong_ball_position);
    pongBallMatrix = glm::scale(pongBallMatrix, glm::vec3(4.0f, 4.0f, 1.0f));
    
    // Pong ball collision logic
    if(ball_collide_paddle()) {
        ball_xdir *= -1.0f;
    }
    
    if(ball_collide_wall()) {
        printf("Hit top wall, changing ball y direction\n");
        ball_ydir *= -1.0f;
    }
    
    // If ball goes out of bounds we want to freeze everything to specify game is over
    if(pong_ball_position.x < -4.5f || pong_ball_position.x > 4.5f) {
        printf("Ouf of bounds, game will now stop.\n");
        player1_speed = 0;
        player2_speed = 0;
        pong_ball_speed = 0;

    }

}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
       
       float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
       float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
       
       glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
       glEnableVertexAttribArray(program.positionAttribute);
       glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
       glEnableVertexAttribArray(program.texCoordAttribute);
       
       program.SetModelMatrix(player1Matrix);
       glBindTexture(GL_TEXTURE_2D, playerTextureID);
       glDrawArrays(GL_TRIANGLES, 0, 6);
       
       program.SetModelMatrix(player2Matrix);
       glBindTexture(GL_TEXTURE_2D, playerTextureID);
       glDrawArrays(GL_TRIANGLES, 0, 6);
       
       program.SetModelMatrix(pongBallMatrix);
       glBindTexture(GL_TEXTURE_2D, pongBallTextureID);
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
