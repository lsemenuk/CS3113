#include "Entity.h"
#include <stdio.h>

Entity::Entity()
{
    position     = glm::vec3(0);
    movement     = glm::vec3(0);
    acceleration = glm::vec3(0);
    velocity     = glm::vec3(0);
    speed        = 0;
    
    modelMatrix = glm::mat4(1.0f);
}

void Entity::AIWalker() {
    movement = glm::vec3(-1, 0, 0);
}

void Entity::AIPatrol(Entity *player) {
    switch(aiState) {
        case(IDLE):
            speed = 4;
            position = glm::vec3(-3, -2.25f, 0);
            movement = glm::vec3(1, 0, 0);
            aiState = CHARGING;
            break;
            
        case(CHARGING):
            if (position.x >= 3.25) {
                movement *= -1;
            }
            if (position.x <= -3.25) {
                movement *= -1;
            }
            break;
            
        case(WALKING):
            break;
            
        case(ATTACKING):
            break;
    }
}

void Entity::AIJumpAndFollow(Entity *player) {
    switch(aiState) {
        case(IDLE):
            position = glm::vec3(-3, -2.25f, 0);
            acceleration = glm::vec3(0, -9.8, 0);
            if (glm::distance(position, player->position) < 3.0f) {
            aiState = CHARGING;
            }
            
        case(CHARGING):
            if (player->position.x < position.x) {
                movement = glm::vec3(-1, 0, 0);
            } else {
                movement = glm::vec3(1, 0, 0);
            }
            if(position.y <= -2.0f) {
                acceleration.y += .3f;
            }
            if(position.y >= -1.5) {
                acceleration.y -= .3f;
            }
            break;
            
        case(WALKING):
            break;
            
        case(ATTACKING):
            break;
    }
}


void Entity::AIWaitAndCharge(Entity *player) {
    switch(aiState) {
        case(IDLE):
            if (glm::distance(position, player->position) < 4.0f) {
                aiState = CHARGING;
            }
            break;
            
        case(CHARGING):
            if (player->position.x < position.x) {
                acceleration += glm::vec3(-3.0f, 0, 0);
                movement = glm::vec3(-1, 0, 0);
            } else {
                acceleration += glm::vec3(3.0f, 0, 0);
                movement = glm::vec3(1, 0, 0);
            }
            break;
            
        case(WALKING):
            break;
            
        case(ATTACKING):
            break;
    }
}

void Entity::AI(Entity *player) {
    switch(aiType) {
        case STAGE1:
            AIWaitAndCharge(player);
            break;
            
        case STAGE2:
            AIJumpAndFollow(player);
            break;
        
        case STAGE3:
            AIPatrol(player);
            break;
    }
}

void Entity::Update(float deltaTime, Entity *player, Entity *platforms, Entity *enemies, int platformCount, int enemyCount)
{
    if(isActive == false) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (entityType == ENEMY) {
        AI(player);
    }
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    if (jump) {
        jump = false;
        
        velocity.y += jumpPower;
    }
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    // Check for collision before render and adjust accordingly
    position.y += velocity.y * deltaTime;       // Move on Y
    CheckCollisionsY(platforms, platformCount); // Fix if needed
        
    position.x += velocity.x * deltaTime;       // Move on X
    CheckCollisionsX(platforms, platformCount); // Fix if needed
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    
    if(entityType == PLAYER) {
        //position.y += velocity.y * deltaTime;       // Move on Y
        CheckCollisionsY(enemies, enemyCount);
        
        //position.x += velocity.x * deltaTime;       // Move on X
        CheckCollisionsX(enemies, enemyCount);
        
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        
        if(lastCollided == ENEMY) {
            if(collidedTop || collidedLeft || collidedRight) {
                //puts("=====================YOU LOSE======================");
                enemies[0].isActive = false;
                speed = 0;
                velocity = glm::vec3(0);
                acceleration = glm::vec3(0);
                movement = glm::vec3(0);
                playerStatus = 2;
            }
            else if(collidedBottom == true) {
                if(enemies[0].aiType == STAGE1) { enemies[0].aiType = STAGE2; enemies[0].aiState = IDLE; }
                else if(enemies[0].aiType == STAGE2) { enemies[0].aiType = STAGE3; enemies[0].aiState = IDLE; }
                else if(enemies[0].aiType == STAGE3) {
                    enemies[0].isActive = false;
                    speed = 0;
                    velocity = glm::vec3(0);
                    acceleration = glm::vec3(0);
                    movement = glm::vec3(0);
                    playerStatus = 1;
                    
                }
            }
        }
    }
    
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index)
{
    float u = (float)(index % animCols) / (float)animCols;
    float v = (float)(index / animCols) / (float)animRows;
    
    float width = 1.0f / (float)animCols;
    float height = 1.0f / (float)animRows;
    
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Render(ShaderProgram *program) {
    
    if(isActive == false) return;

    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[]  = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

bool Entity::CheckCollision(Entity *other) {
    
    if(isActive == false || other->isActive == false) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) {
        lastCollided = other->entityType;
        return true;
    }
    
    return false;
}

void Entity::CheckCollisionsY(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        if (CheckCollision(object)) {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
            }
            else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
            }
        }
    }
}

void Entity::CheckCollisionsX(Entity *objects, int objectCount)
{
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        if (CheckCollision(object)) {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
            }
            else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
            }
        }
    }
}

void Entity::CheckCollisionsPlayerY(Entity *player) {
    Entity *object = player;
    if (CheckCollision(object)) {
        float ydist = fabs(position.y - object->position.y);
        float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
        if (velocity.y > 0) {
            position.y -= penetrationY;
            velocity.y = 0;
            collidedTop = true;
        }
        else if (velocity.y < 0) {
            position.y += penetrationY;
            velocity.y = 0;
            collidedBottom = true;
        }
    }
}

void Entity::CheckCollisionsPlayerX(Entity *player) {
    Entity *object = player;
    if (CheckCollision(object)) {
        float xdist = fabs(position.x - object->position.x);
        float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
        if (velocity.x > 0) {
            position.x -= penetrationX;
            velocity.x = 0;
            collidedRight = true;
        }
        else if (velocity.x < 0) {
            position.x += penetrationX;
            velocity.x = 0;
            collidedLeft = true;
        }
    }
}
