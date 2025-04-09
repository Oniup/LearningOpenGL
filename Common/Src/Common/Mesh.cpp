#include "Common/Mesh.h"

namespace Cm
{
    std::vector<Cm::Transform> Transform::GenerateMultiRandom(size_t count, float minPos, float maxPos)
    {
        std::vector<Cm::Transform> transforms;
        auto inRange = [](float v, float ov) -> bool
        {
            constexpr float range = 0.5f;
            return v >= ov - range && v <= ov + range;
        };
        for (size_t i = 0; i < count; ++i)
        {
            Cm::Transform transform;
            bool regen = true;
            while (regen)
            {
                regen = false;
                transform.GenerateRandom(minPos, maxPos);
                for (const Cm::Transform& other : transforms)
                {
                    if (inRange(transform.Position.x, other.Position.x))
                        regen = true;
                    if (inRange(transform.Position.y, other.Position.y))
                        regen = true;
                    if (inRange(transform.Position.z, other.Position.z))
                        regen = true;
                }
            }
            transforms.push_back(transform);
        }
        return transforms;
    }

    glm::mat4 Transform::GetModel() const
    {
        glm::vec3 eulerAngles = glm::radians(Rotation);
        // Create quaternions from euler angles
        glm::quat quatX = glm::angleAxis(eulerAngles.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat quatY = glm::angleAxis(eulerAngles.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat quatZ = glm::angleAxis(eulerAngles.z, glm::vec3(0.0f, 0.0f, 1.0f));

        // Combine the quaternions and the order of operations matters
        glm::quat rotation = quatZ * quatY * quatX;

        // Construct transform
        glm::mat4 model(1.0f);
        model = glm::translate(model, Position);
        model *= glm::mat4_cast(rotation);
        model = glm::scale(model, Scale);
        return model;
    }

    void Transform::GenerateRandom(float minPos, float maxPos)
    {
        std::random_device randomDevice;
        std::default_random_engine eng(randomDevice());
        std::uniform_real_distribution rPos(minPos, maxPos);
        std::uniform_real_distribution rRot(0.0f, 360.0f);
        std::uniform_real_distribution rSca(0.1f, 1.0f);
        Position = glm::vec3(rPos(eng), rPos(eng), rPos(eng));
        Scale = glm::vec3(rSca(eng));
        Rotation = glm::vec3(rRot(eng), rRot(eng), rRot(eng));
    }
}
