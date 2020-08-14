#pragma once
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
#include "Map.h"

enum EntityType {
    PLAYER,
    PLATFORM,
    ENEMY,
    BLOCK,
    POWERUP
};

//enum BlockAIType {
//    UPDOWN,
//    SIDETOSIDE
//};

enum AIType {
    STAGE1,
    STAGE2,
    STAGE3,
    STAGE4,
    STAGE5
};

enum AIState {
    IDLE,
    WALKING,
    ATTACKING,
    CHARGING,
};

class Entity {
public:
    EntityType entityType;
    
    AIType aiType;
    AIState aiState;
    Entity *lastCollided = NULL;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    
    float width = .8;
    float height = .8;

    bool jump = false;
    float jumpPower = 0;

    float speed;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;

    int *animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    
    bool isActive = true;
    bool move_block = false;
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    bool collidedEnemyBottom = false;
    bool lostLife = false;
    bool gameOver = false;
    bool gameWin = false;

    Entity();
    
    bool CheckCollision(Entity *other);
    void CheckCollisionsY(Entity *objects, int objectCount);
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsX(Map *map);
    void CheckCollisionsY(Map *map);

    
    void Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Map *map, int *lives);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    void AI(Entity *player);
    void AIWalker();
    void AIWaitAndCharge(Entity *player);
    void AIJumpAndFollow(Entity *player);
    void AIPatrol(Entity *player);
    void AIPatrolLevel3(Entity *player);
    
    void block_side_to_side(Entity *player);
    void block_up_and_down(Entity *player);
};
