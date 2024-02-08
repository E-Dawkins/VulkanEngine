#pragma once
inline long double g_timeSinceAppStart = 0.f;
inline glm::vec3 g_gravity = glm::vec3(0, 0, -0.02f);
inline bool g_visualizeColliders = true;

inline glm::vec3 g_rightVector = glm::vec3(1, 0, 0);
inline glm::vec3 g_forwardVector = glm::vec3(0, 1, 0);
inline glm::vec3 g_upVector = glm::vec3(0, 0, 1);