#include "Level1.h"
#define LEVEL1_WIDTH 11
#define LEVEL1_HEIGHT 41
#define LEVEL1_ENEMY_COUNT 7

int lives_left;

GLuint powerupTextureID;
GLuint floatingPlatform;

unsigned int level1_data[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 1, 3, 1, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0,
    0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0,
    0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 3, 0, 0, 3, 0, 0, 3, 3, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 1, 2, 1, 1, 1, 1, 1, 1,
    3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 2, 0, 0, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3,
    3, 0, 0, 0, 0, 0, 0, 1, 2, 2, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
};

void Level1::Initialize() {
    textTextureID = Util::LoadTexture("font1.png");
    powerupTextureID = Util::LoadTexture("power_up.png");
    floatingPlatform = Util::LoadTexture("platformPack_tile001.png");

    state.nextScene = -1;
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 4, 1);
    
    // Initialize Game Objects
    
    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(5, -37, 0);
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
    
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];
    GLuint enemyTextureID = Util::LoadTexture("ctg.png");
    
    // Side to side block 1
    state.enemies[0].textureID = floatingPlatform;
    state.enemies[0].position = glm::vec3(5, -34, 0);
    state.enemies[0].movement = glm::vec3(1, 0, 0);
    state.enemies[0].acceleration = glm::vec3(0, 0, 0);
    state.enemies[0].speed = 1.0f;
    state.enemies[0].height = 1.0f;
    state.enemies[0].width = 1.0f;
    state.enemies[0].entityType = ENEMY;
    state.enemies[0].aiType = STAGE3;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].move_block = true;
    state.enemies[0].isActive = true;
    
    // Up and down block 1
    state.enemies[1].textureID = floatingPlatform;
    state.enemies[1].position = glm::vec3(8, -26, 0);
    state.enemies[1].movement = glm::vec3(0, 1, 0);
    state.enemies[1].velocity = glm::vec3(0, 1, 0);
    state.enemies[1].acceleration = glm::vec3(0, .1, 0);
    state.enemies[1].speed = .3f;
    state.enemies[1].height = 1.0f;
    state.enemies[1].width = 1.0f;
    state.enemies[1].entityType = ENEMY;
    state.enemies[1].aiType = STAGE4;
    state.enemies[1].move_block = true;
    state.enemies[1].isActive = true;
    
    state.enemies[2].textureID = enemyTextureID;
    state.enemies[2].position = glm::vec3(8, -31, 0);
    state.enemies[2].movement = glm::vec3(1, 0, 0);
    state.enemies[2].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[2].speed = 1.0f;
    state.enemies[2].height = 1.0f;
    state.enemies[2].width = 1.0f;
    state.enemies[2].entityType = ENEMY;
    state.enemies[2].aiType = STAGE2;
    state.enemies[2].aiState = IDLE;
    state.enemies[2].isActive = true;
    
    state.enemies[3].textureID = enemyTextureID;
    state.enemies[3].position = glm::vec3(3, -23, 0);
    state.enemies[3].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[3].speed = 1.0f;
    state.enemies[3].height = 1.0f;
    state.enemies[3].width = 1.0f;
    state.enemies[3].entityType = ENEMY;
    state.enemies[3].aiType = STAGE5;
    state.enemies[3].aiState = IDLE;
    state.enemies[3].isActive = true;
    
    state.enemies[4].textureID = powerupTextureID;
    state.enemies[4].position = glm::vec3(8.5, -18, 0);
    state.enemies[4].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[4].speed = 1.0f;
    state.enemies[4].height = 1.0f;
    state.enemies[4].width = 1.0f;
    state.enemies[4].entityType = POWERUP;
    state.enemies[4].isActive = true;
    
    state.enemies[5].textureID = enemyTextureID;
    state.enemies[5].position = glm::vec3(3, -4, 0);
    state.enemies[5].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[5].speed = 2.3f;
    state.enemies[5].height = 1.0f;
    state.enemies[5].width = 1.0f;
    state.enemies[5].entityType = ENEMY;
    state.enemies[5].aiType = STAGE5;
    state.enemies[5].aiState = IDLE;
    state.enemies[5].isActive = true;
    
    state.enemies[6].textureID = enemyTextureID;
    state.enemies[6].position = glm::vec3(5, -1, 0);
    state.enemies[6].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[6].speed = 2.5f;
    state.enemies[6].height = 1.0f;
    state.enemies[6].width = 1.0f;
    state.enemies[6].entityType = ENEMY;
    state.enemies[6].aiType = STAGE5;
    state.enemies[6].aiState = IDLE;
    state.enemies[6].isActive = true;
    
}

void Level1::Update(float deltaTime, int *lives) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map, lives);
    
    for(int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map, lives);
    }
    
    lives_left = *lives;
    if(state.player->lostLife) {
        state.player->position = glm::vec3(5, -37, 0);
        state.player->jumpPower = 5;
        state.enemies[4].isActive = true;
    }

    if(*lives == 0) {
        state.player->gameOver = true;
        state.player->speed = 0;
    }
    
    if (state.player->position.y < -42) {
        *lives -= 1;
        state.player->position = glm::vec3(5, -37, 0);
        state.player->jumpPower = 5;
        state.enemies[4].isActive = true;
    }
    
    // Win condition
    if (state.player->position.x >= 9 && state.player->position.y >= -2) {
        state.player->gameWin = true;
        state.player->speed = 0;
    }
    

}

void Level1::Render(ShaderProgram *program) {
    state.map->Render(program);
    state.player->Render(program);
    for(int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].Render(program);
    }
    
    Util::DrawText(program, textTextureID, "Jump Powerup", .5f, -.25f, glm::vec3(7.0f, -17.0, 0));
    
    Util::DrawText(program, textTextureID, "Jump Power: " + std::to_string(state.player->jumpPower), .5f, -.25f,
                   glm::vec3(6.4, state.player->position.y + 6.0, 0));

        
    Util::DrawText(program, textTextureID, "Lives: " + std::to_string(lives_left), .5f, -.25f,
                                                       glm::vec3(.75f, state.player->position.y + 6, 0));

    if(state.player->gameOver) {
        Util::DrawText(program, textTextureID, "Game Over", .5f, -.25f, glm::vec3(4.0f, state.player->position.y + 2, 0));
    }
    
    if(state.player->gameWin) {
        Util::DrawText(program, textTextureID, "WINNER!", .5f, -.25f, glm::vec3(4.0f, state.player->position.y + 2, 0));
    }
    
}
