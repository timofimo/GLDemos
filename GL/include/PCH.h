#pragma once

#define WIN32_LEAN_AND_MEAN
#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION
#define GLFW_INCLUDE_NONE

#pragma warning(push, 0)

#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <assert.h>
#include <queue>
#include <vector>
#include <map>
#include <fstream>

#include <glm\glm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtx\norm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>

#include <GLFW\glfw3.h>
#include <glad/glad.h>

#include <Utilities.h>

#undef min
#undef max

#pragma warning pop