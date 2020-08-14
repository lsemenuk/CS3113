#include "Scene.h"
class LevelMenu : public Scene {
public:
    GLuint textTextureID;
    void Initialize() override;
    void Update(float deltaTime, int *lives) override;
    void Render(ShaderProgram *program) override;
};
