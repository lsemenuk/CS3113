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

enum EntityType {
    PLAYER,
    PLATFORM,
    ENEMY
};

enum AIType {
    STAGE1,
    STAGE2,
    STAGE3
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
    EntityType lastCollided;
    
    AIType aiType;
    AIState aiState;
    
    // 0 = In progress
    // 1 = Win
    // 2 = Lose
    int playerStatus = 0;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    
    float width = 1;
    float height = 1;

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
    
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;

    Entity();
    
    bool CheckCollision(Entity *other);
    void CheckCollisionsY(Entity *objects, int objectCount);
    void CheckCollisionsX(Entity *objects, int objectCount);
    void CheckCollisionsPlayerY(Entity *player);
    void CheckCollisionsPlayerX(Entity *player);

    
    void Update(float deltaTime, Entity *player, Entity *platforms, Entity* enemies, int platformCount, int enemyCount);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    void AI(Entity *player);
    void AIWalker();
    void AIWaitAndCharge(Entity *player);
    void AIJumpAndFollow(Entity *player);
    void AIPatrol(Entity *player);
};
