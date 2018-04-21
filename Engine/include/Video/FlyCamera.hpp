#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Input/IInputManager.hpp"

/// This class is a massive hack, pending rewrite
class FlyCamera {
    float m_yaw, m_pitch;

    glm::vec3 m_position, m_front, m_up, m_right, m_worldUp;

public:
    FlyCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f) :
            m_yaw(yaw),
            m_pitch(pitch),
            m_position(position),
            m_front(glm::vec3(0.0f, 0.0f, -1.0f)),
            m_up(glm::vec3(0.0f, 0.0f, 0.0f)),
            m_worldUp(up)
    {
        UpdateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(m_position, m_position + m_front, m_up);
    }

    void Update(Engine::Input::IInputManager& ipt)
    {
        using namespace Engine::Input;
        if (ipt.GetKeyState(Keys::W)) {
            m_position += m_front * 0.05f;
        }

        if (ipt.GetKeyState(Keys::S)) {
            m_position -= m_front * 0.05f;
        }

        if (ipt.GetKeyState(Keys::A)) {
            m_position -= m_right * 0.05f;
        }

        if (ipt.GetKeyState(Keys::D)) {
            m_position += m_right * 0.05f;
        }


        auto delta = ipt.GetMouseDelta();

        m_yaw += delta.x * 0.15f;
        m_pitch += delta.y * 0.15f;

        if (m_pitch > 89.0f) {
            m_pitch = 89.0f;
        }
        if (m_pitch < -89.0f) {
            m_pitch = -89.0f;
        }

        UpdateCameraVectors();
    }

    void UpdateCameraVectors()
    {
        glm::vec3 front;
        front.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
        front.y = std::sin(glm::radians(m_pitch));
        front.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));

        m_front = glm::normalize(front);
        m_right = glm::normalize(glm::cross(m_front, m_worldUp));
        m_up = glm::normalize(glm::cross(m_right, m_front));
    }
};