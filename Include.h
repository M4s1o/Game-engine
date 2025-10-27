#pragma once

// open GL
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <gtc/type_ptr.hpp>
#include <stb/stb_image.h>

// Standard math and trig
#include <cmath>
#include <complex>
#include <valarray>

// Time management
#include <chrono>

// Random number generation
#include <random>

// Containers and utilities
#include <glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/euler_angles.hpp>
#include <memory>
#include <map>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif