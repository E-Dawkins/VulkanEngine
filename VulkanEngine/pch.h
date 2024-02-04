﻿#pragma once
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/gtx/euler_angles.hpp>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <array>
#include <chrono>
#include <iostream>
#include <string>

using namespace std::chrono;

#include "StringConverter.h"
#include "Renderer.h"

/* Global Constants */
inline long double g_timeSinceAppStart = 0.f;
inline glm::vec3 g_gravity = glm::vec3(0, 0, -0.098f);
inline bool g_visualizeColliders = false;

inline glm::vec3 g_rightVector = glm::vec3(1, 0, 0);
inline glm::vec3 g_forwardVector = glm::vec3(0, 1, 0);
inline glm::vec3 g_upVector = glm::vec3(0, 0, 1);