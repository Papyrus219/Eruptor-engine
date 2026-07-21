#include <Eruptor/scene/transformation.hpp>
#include <glm/gtc/matrix_transform.hpp>

const glm::mat4x4 & eruptor::scene::Transformation::Get_model_matrix()
{
    if(!has_changed)
    {
        return last_model_matrix;
    }

    glm::mat4x4 model{1.0f};
    model = glm::translate(model, position);
    model *= glm::mat4_cast(rotation);
    model = glm::scale(model, scale);

    last_model_matrix = model;
    has_changed = false;
    return last_model_matrix;
}
