#include "Level3.h"
#define LEVEL3_WIDTH 21
#define LEVEL3_HEIGHT 8
#define LEVEL3_ENEMY_COUNT 2

int lives_left_3;

unsigned int level3_data[] = {
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1, 1,
    3, 1, 1, 1, 1, 2, 2, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 1, 1, 2, 2,
    3, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 2, 2, 2, 2
};

void Level3::Initialize() {
    textTextureID = Util::LoadTexture("font1.png");

    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(2, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.0f;
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
    
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    state.enemies[0].textureID = enemyTextureID;
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = 1.0f;
    state.enemies[0].acceleration = glm::vec3(0, -9.8f, 0);
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].position = glm::vec3(9, 0, 0);
    state.enemies[0].aiType = STAGE1;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].speed = 2;
    state.enemies[0].isActive = true;
    
    state.enemies[1].textureID = enemyTextureID;
    state.enemies[1].height = 1.0f;
    state.enemies[1].width = 1.0f;
    state.enemies[1].acceleration = glm::vec3(0, -12.8f, 0);
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].position = glm::vec3(12, 0, 0);
    state.enemies[1].movement = glm::vec3(1, 0, 0);
    state.enemies[1].aiType = STAGE3;
    state.enemies[1].aiState = IDLE;
    state.enemies[1].speed = 2;
    state.enemies[1].isActive = true;
}

void Level3::Update(float deltaTime, int *lives) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map, lives);
    
    for(int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map, lives);
    }
    
    lives_left_3 = *lives;
    if(state.player->lostLife) {
        state.player->position = glm::vec3(2, 0, 0);
    }
    
    if(*lives == 0) {
        state.player->gameOver = true;
        state.player->speed = 0;
    }
    
    if(state.player->position.x >= 18) {
        state.player->speed = 0;
        state.player->gameWin = true;
    }
}

void Level3::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    
    for(int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
    
    Util::DrawText(program, textTextureID, "Lives: " + std::to_string(lives_left_3), .5f, -.25f, glm::vec3(1.0f, -.5, 0));
    if(state.player->gameOver) {
        Util::DrawText(program, textTextureID, "Game Over", .5f, -.25f, glm::vec3(5.0f, -1.0, 0));
    }
    
    if(state.player->gameWin) {
        Util::DrawText(program, textTextureID, "WINNER!", .5f, -.25f, glm::vec3(14.0f, -1.0, 0));
    }
}
