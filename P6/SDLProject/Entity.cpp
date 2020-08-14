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
            speed = 2;
            aiState = CHARGING;
            break;
            
        case(CHARGING):
            if (position.x >= 9) {
                movement *= -1;
            }
            if (position.x <= 5) {
                movement *= -1;
            }
            break;
            
        case(WALKING):
            break;
            
        case(ATTACKING):
            break;
    }
}

void Entity::AIPatrolLevel3(Entity *player) {
    switch(aiState) {
        case(IDLE):
            speed = 2;
            aiState = CHARGING;
            break;
            
        case(CHARGING):
            if (position.x >= 13) {
                movement *= -1;
            }
            if (position.x <= 11) {
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
            if (glm::distance(position, player->position) < 4.0f && player->position.y >= position.y) {
                //if(position.y == player->position.y) {
                    aiState = CHARGING;
                //}
            }
            break;
            
        case(CHARGING):
            if (player->position.x < position.x) {
                movement = glm::vec3(-1, 0, 0);
            } else {
                movement = glm::vec3(1, 0, 0);
            }
            break;
            
        case(WALKING):
            break;
            
        case(ATTACKING):
            break;
    }
}

void Entity::block_side_to_side(Entity *player) {
    switch(aiState) {
        case(IDLE):
            aiState = CHARGING;
            break;
            
        case(CHARGING):
            if (position.x >= 8.5) {
                movement *= -1;
            }
            if (position.x <= 1.5) {
                movement *= -1;
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
            AIWalker();
            break;
            
        case STAGE2:
            AIPatrol(player);
            break;
        
        case STAGE3:
            block_side_to_side(player);
            break;
        
        case STAGE4:
            if (position.y >= -23.5) {
                velocity = glm::vec3(0, -1, 0);
            }
            if (position.y <= -27.5) {
                velocity = glm::vec3(0, 1, 0);

            }
            break;
        
        case STAGE5:
            AIWaitAndCharge(player);
            break;
    }
}

void Entity::Update(float deltaTime, Entity *player, Entity *objects, int objectCount, Map *map, int *lives)
{
    if(isActive == false) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedEnemyBottom = false;
    collidedLeft = false;
    collidedRight = false;
    lostLife = false;
    
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
    position.y += velocity.y * deltaTime;
    CheckCollisionsY(map);
    
    if (entityType != ENEMY) {
        CheckCollisionsY(objects, objectCount);
    }
    
    position.x += velocity.x * deltaTime;
    CheckCollisionsX(map);
    
    if(entityType != ENEMY) {
        CheckCollisionsX(objects, objectCount);
    }
    
    if(entityType == PLAYER) {
        if(lastCollided != NULL) {
            if(lastCollided->entityType == ENEMY && collidedEnemyBottom && lastCollided->move_block == false) {
                //puts("Killed");
                lastCollided->isActive = false;
            } else if (lastCollided->entityType == ENEMY && lastCollided->move_block == false && (collidedTop || collidedLeft || collidedRight)) {
                //puts("lost life");
                *lives -= 1;
                lostLife = true;
            } else if(lastCollided->entityType == ENEMY && collidedEnemyBottom && lastCollided->move_block == true) {
                position.x = lastCollided->position.x;
            } else if(lastCollided->entityType == POWERUP) {
                jumpPower = 7.5;
                lastCollided->isActive = false;
                puts("Power Up: Jump Power Increased!");
            }
            lastCollided = NULL;
        }
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    
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
        
        if(entityType == PLAYER && (other->entityType == ENEMY || other->entityType == POWERUP)) {
            lastCollided = other;
        }
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
                if(entityType == PLAYER && object->entityType == ENEMY) {
                    collidedEnemyBottom = true;
                }
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

void Entity::CheckCollisionsY(Map *map)
{
// Probes for tiles
    glm::vec3 top = glm::vec3(position.x, position.y + (height / 2), position.z);
    glm::vec3 top_left = glm::vec3(position.x - (width / 2), position.y + (height / 2), position.z);
    glm::vec3 top_right = glm::vec3(position.x + (width / 2), position.y + (height / 2), position.z);
    
    glm::vec3 bottom = glm::vec3(position.x, position.y - (height / 2), position.z);
    glm::vec3 bottom_left = glm::vec3(position.x - (width / 2), position.y - (height / 2), position.z);
    glm::vec3 bottom_right = glm::vec3(position.x + (width / 2), position.y - (height / 2), position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(top, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y;
        velocity.y = 0;
        collidedTop = true;
    } else if (map->IsSolid(top_left, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y; velocity.y = 0;
        collidedTop = true;
    } else if (map->IsSolid(top_right, &penetration_x, &penetration_y) && velocity.y > 0) {
        position.y -= penetration_y; velocity.y = 0;
        collidedTop = true;
    }

    if (map->IsSolid(bottom, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    } else if (map->IsSolid(bottom_left, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    } else if (map->IsSolid(bottom_right, &penetration_x, &penetration_y) && velocity.y < 0) {
        position.y += penetration_y;
        velocity.y = 0;
        collidedBottom = true;
    }
}

void Entity::CheckCollisionsX(Map *map)
{
// Probes for tiles
    glm::vec3 left = glm::vec3(position.x - (width / 2), position.y, position.z);
    glm::vec3 right = glm::vec3(position.x + (width / 2), position.y, position.z);
    
    float penetration_x = 0;
    float penetration_y = 0;
    
    if (map->IsSolid(left, &penetration_x, &penetration_y) && velocity.x < 0) {
        position.x += penetration_x;
        velocity.x = 0;
        collidedLeft = true;
    }

    if (map->IsSolid(right, &penetration_x, &penetration_y) && velocity.x > 0) { position.x -= penetration_x;
        velocity.x = 0;
        collidedRight = true;
    }
}
