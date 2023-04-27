#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "drawing_game_object.h"

namespace game {

    DrawingGameObject::DrawingGameObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, float v, int n, glm::vec4 c, GameObject* p) : GameObject(position, geom, shader, texture) {
        type = n;
        value = v;
        colour = c;
        parent = p;
    }


void DrawingGameObject::Update(double current_time) {
    GameObject::Update(current_time);
}


void DrawingGameObject::Render(glm::mat4 view_matrix, double current_time) {

    // Updating the value
    if (type == 0) {
        value = parent->getFuel();
    }
    else if (type == 1) {
        value = parent->health;
    }
    else if (type == 2) {
        value = parent->shield;
    }
    else if (type == 3) {
        value = parent->inv;
        //parent->inv = value;
        //std::cout << current_time << std::endl;
    }

    // Set up the shader
    shader_->Enable();

    // Set up the view matrix
    shader_->SetUniformMat4("view_matrix", view_matrix);

    // Setup the scaling matrix for the shader
    glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), scale_);

    // Setup the rotation matrix for the shader
    glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0, 0.0, 1.0));

    // Set up the translation matrix for the shader
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

    // Setup the transformation matrix for the shader
    glm::mat4 transformation_matrix = translation_matrix * rotation_matrix * scaling_matrix;

    // Set the transformation matrix in the shader
    shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

    // Set up the geometry
    geometry_->SetGeometry(shader_->GetShaderProgram());
    shader_->SetUniform1f("length", value / 10.0f);
    shader_->SetUniform4f("col", colour);

    // Bind the entity's texture
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Draw the entity
    glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
}

} // namespace game