#include <glad/glad.h>

#include "Common/Camera.h"
#include "Common/Context.h"
#include "Common/Mesh.h"
#include "Common/Shader.h"

int main(int argc, char** argv)
{
    Cm::Context context(PROJECT_NAME, 600, 600);
    context.SetClearOptions(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    Cm::Shader shader({PROJECT_DIR "/Model.vert", PROJECT_DIR "/Model.frag"});
    Cm::Model model(RESOURCE_DIR "/Meshes/backpack/backpack.obj");

    Cm::FirstPersonCamera camera(context);
    camera.Position.z = -10;

    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, &camera.GetProjectionMatrix(context)[0][0], GL_STATIC_DRAW);

    Cm::Transform transform;
    transform.Position = glm::vec3(0.0f);

    while (context.BeginFrame())
    {
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &camera.GetViewMatrix()[0][0]);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, sizeof(glm::mat4) * 2);

        camera.ProcessMouseMovement();
        camera.ProcessMovement(context.GetWindow(), context.GetDeltaTime());

        shader.Bind();
        shader.UniformMat4("u_Model", transform.GetModel());
        model.Draw(shader);

        context.EndFrame();
    }
    return 0;
}
