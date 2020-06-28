#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <vector>
#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define PLATFORM_COUNT 18

struct GameState {
    Entity *player;
    Entity *platforms;
    int   win;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint textTextureID;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}

// Initialize game tiles. This could be easily done
// and probably not be necessary if I had time to make
// an auto tiling function

void init_game_state() {
    // Have not won or lost yet so game is in progress
    // 0 = in progress, 1 = lose, 2 = win
    state.win = 0;
    
    // Init text textures
    textTextureID = LoadTexture("font1.png");
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(0.0f, 3.75f, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -.2f, 0);
    state.player->speed = 1.0f;
    state.player->textureID = LoadTexture("george_0.png");
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = .8f;
    state.player->width = .8f;
    
    state.player->jumpPower = .5f;
    
    state.platforms = new::Entity[PLATFORM_COUNT];
    
    GLuint platformTextureGround1ID = LoadTexture("ground_tile_1.png");
    GLuint platformTextureFloating1ID = LoadTexture("floating_tile_1.png");
    GLuint platformTextureWall1ID = LoadTexture("cave_wall_1.png");
    GLuint platformTextureLandingPadID = LoadTexture("landing_pad.png");


    // Initialize ground tiles
    state.platforms[0].textureID = platformTextureGround1ID;
    state.platforms[0].position = glm::vec3(-4.5, -3.5, 0);
    
    state.platforms[1].textureID = platformTextureGround1ID;
    state.platforms[1].position = glm::vec3(-3.5, -3.5, 0);
    
    state.platforms[2].textureID = platformTextureGround1ID;
    state.platforms[2].position = glm::vec3(1.5, -3.5, 0);
    
    state.platforms[8].textureID = platformTextureGround1ID;
    state.platforms[8].position = glm::vec3(2.5, -3.5, 0);
    
                                    
    state.platforms[9].textureID = platformTextureLandingPadID;
    state.platforms[9].position = glm::vec3(3.5, -3.5, 0);
    state.platforms[9].entityType = 2;
    
    state.platforms[10].textureID = platformTextureGround1ID;
    state.platforms[10].position = glm::vec3(4.5, -3.5, 0);
    
    // Initialize floating tiles
    state.platforms[3].textureID = platformTextureFloating1ID;
    state.platforms[3].position = glm::vec3(-4.5, -0, 0);
    
    state.platforms[4].textureID = platformTextureFloating1ID;
    state.platforms[4].position = glm::vec3(-3.5, 0, 0);
    
    state.platforms[5].textureID = platformTextureFloating1ID;
    state.platforms[5].position = glm::vec3(4.5, .5, 0);
    
    state.platforms[6].textureID = platformTextureFloating1ID;
    state.platforms[6].position = glm::vec3(3.5, .5, 0);
    
    state.platforms[7].textureID = platformTextureFloating1ID;
    state.platforms[7].position = glm::vec3(2.5, .5, 0);
    
    // Initialize cave walls
    state.platforms[11].textureID = platformTextureWall1ID;
    state.platforms[11].position = glm::vec3(5.0f, -1.5, 0);
    
    state.platforms[12].textureID = platformTextureWall1ID;
    state.platforms[12].position = glm::vec3(5.0f, -2.5, 0);
    
    state.platforms[13].textureID = platformTextureWall1ID;
    state.platforms[13].position = glm::vec3(5.0f, -.5, 0);
    
    state.platforms[14].textureID = platformTextureWall1ID;
    state.platforms[14].position = glm::vec3(-5.0f, -1.5, 0);
    
    state.platforms[15].textureID = platformTextureWall1ID;
    state.platforms[15].position = glm::vec3(-5.0f, -2.5, 0);
    
    state.platforms[16].textureID = platformTextureWall1ID;
    state.platforms[16].position = glm::vec3(-5.0f, -.5, 0);
    
    state.platforms[17].textureID = platformTextureWall1ID;
    state.platforms[17].position = glm::vec3(5.0f, .3, 0);
    
    // Theyre static and only need to be updated once so they are in
    // correct position.
    for(int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, 0);
    }
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    init_game_state();
    
}

void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                        
                    case SDLK_SPACE:
                        // Some sort of action
                        if(state.player->collidedBottom) {
                            state.player->jump = true;
                        }
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        // Changeing the acceleration give us the drifting effect and the
        // number being multiplied is at what rate the drift speed increases
        // as the arrow keys are held.
        state.player->acceleration.x += state.player->movement.x * 1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->acceleration.x += state.player->movement.x * 1.0f;
        state.player->animIndices = state.player->animRight;
    }
    

    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
                                 float size, float spacing, glm::vec3 position)
{

    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        
        vertices.insert(vertices.end(), {
            offset + (-0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (0.5f * size), -0.5f * size,
            offset + (0.5f * size), 0.5f * size,
            offset + (-0.5f * size), -0.5f * size,
        });
        
        texCoords.insert(texCoords.end(), { u, v,
                u, v + height,
                u + width, v,
                u + width, v + height,
                u + width, v,
                u, v + height,
            });
    }
    
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    program->SetModelMatrix(modelMatrix);
    
    glUseProgram(program->programID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
    // Check players lastCollided value to see if we win or lose
    if(state.player->lastCollided == 1 || state.player->position.y < -3.85) {
        // If we hit a block thats not the pad set win to false
        state.win = 1;
    } else if(state.player->lastCollided == 2) {
        // You win
        state.win = 2;
    }
}


void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for(int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    
    state.player->Render(&program);
    
    // Should we draw the game over text or not
    // 0 = game in progress, 1 = lose, 2 = win
    if(state.win == 1) {
        DrawText(&program, textTextureID, "Mission Failed!", .5f, -.25f, glm::vec3(-2.5f, 0, 0));
        state.player->speed = 0;
        state.player->acceleration = glm::vec3(0);
        state.player->velocity = glm::vec3(0);
    } else if(state.win == 2) {
        DrawText(&program, textTextureID, "Mission Successful!", .5f, -.25f, glm::vec3(-2.5f, 0, 0));
        state.player->speed = 0;
        state.player->acceleration = glm::vec3(0);
        state.player->velocity = glm::vec3(0);
    }
        
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
