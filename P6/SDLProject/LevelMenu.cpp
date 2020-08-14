#include "LevelMenu.h"
#define LEVELMENU_WIDTH 14
#define LEVELMENU_HEIGHT 8
#define LEVELMENU_ENEMY_COUNT 2

unsigned int levelMenu_data[] = {
    3, 1, 0, 0, 0, 0, 0, 0, 0, 1, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 1, 1, 1, 0, 0, 0, 1, 1, 1, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3
};

void LevelMenu::Initialize() {
    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVELMENU_WIDTH, LEVELMENU_HEIGHT, levelMenu_data, mapTextureID, 1.0f, 4, 1);
    
    textTextureID = Util::LoadTexture("font1.png");
    
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 0.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
    
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
    
    state.player->height = 0.8f;
    state.player->width = 0.8f;
    
    state.player->jumpPower = 5.0f;
    
    state.enemies = new Entity[LEVELMENU_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = 1.0f;
    state.enemies[0].acceleration = glm::vec3(0, -9.8f, 0);
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].position = glm::vec3(3.5, 0, 0);
    state.enemies[0].aiType = STAGE1;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].speed = 1;
    state.enemies[0].isActive = true;
    
}

void LevelMenu::Update(float deltaTime, int* lives) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVELMENU_ENEMY_COUNT, state.map, lives);    
}

void LevelMenu::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    
    Util::DrawText(program, textTextureID, "Welcome, press enter to start", .5f, -.25f, glm::vec3(1.4f, -1.0, 0));
}
