#ifndef _aeRender
#define _aeRender

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include "lodfile.h"
#include "aeTexture.h"

class Render {
    private:
        unsigned int VBO, VAO;
        Shader ourShader;
        std::shared_ptr<aeTexture> tex1,tex2;
    public:
        Render();
        ~Render();
        void Draw(glm::mat4 projection, glm::mat4 view );
};

#endif //_aeRender

