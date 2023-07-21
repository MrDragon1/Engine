#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <map>
#include <stack>
#include <bitset>

#include "Base/Log.h"
#include "Base/Ref.h"
#include "Base/TimeStamp.h"
#include "Base/Math/Math.h"
#include "Base/Math/Vector.h"
#include "Base/Math/Quaternion.h"
#include "Base/Math/Matrix.h"
#include "Base/UUID.h"
#include "Base/Singleton.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include "glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using std::map;
using std::stack;
using std::string;
using std::unordered_map;
using std::vector;

using StringMap = std::unordered_map<string, string>;

using IntVec = vector<int>;
using BoolVec = vector<bool>;
using FloatVec = vector<float>;
using StringVec = vector<string>;

#define STRING(classname) #classname
