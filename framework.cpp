#include "framework.h"
#include "Include.h"
#include "attributes.h"

namespace GL {
	// buffer --
	template<typename T>
	buffer<T>::buffer()
		: dataType([]() -> GLenum {
		if      constexpr (std::is_same_v<T, float> || std::is_same_v<T, GLfloat>)   return GL_FLOAT;
		else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, GLdouble>)  return GL_DOUBLE;
		else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, GLint>)     return GL_INT;
		else if constexpr (std::is_same_v<T, unsigned int> || std::is_same_v<T, GLuint>)    return GL_UNSIGNED_INT;
		else if constexpr (std::is_same_v<T, short> || std::is_same_v<T, GLshort>)   return GL_SHORT;
		else if constexpr (std::is_same_v<T, unsigned short> || std::is_same_v<T, GLushort>)  return GL_UNSIGNED_SHORT;
		else if constexpr (std::is_same_v<T, char> || std::is_same_v<T, GLbyte>)    return GL_BYTE;
		else if constexpr (std::is_same_v<T, unsigned char> || std::is_same_v<T, GLubyte>)   return GL_UNSIGNED_BYTE;
		else if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, GLboolean>) return GL_BOOL;
		else static_assert(!sizeof(T), "Unsupported type for buffer");
			}()) {}
	template<typename T>
	buffer<T>::~buffer() {
		glDeleteBuffers(1, &ID);
	}
	// buffers --
	namespace Buffer {
		// VBO --
		template<typename T>
		VBO<T>::VBO(GLenum usage) {
			this->type = GL_ARRAY_BUFFER;
			this->usage = usage;
			this->size = 0;

			glGenBuffers(1, &this->ID);
			glBindBuffer(this->type, this->ID);
			glBufferData(this->type, 0, nullptr, this->usage);
			glBindBuffer(this->type, 0);
		}
		template<typename T>
		void VBO<T>::bind() {
			glBindBuffer(this->type, this->ID);
		}
		template<typename T>
		void VBO<T>::unbind() {
			glBindBuffer(this->type, 0);
		}
		template<typename T>
		void VBO<T>::loadData() {
			bind();
			if (this->data.empty()) {
				unbind();
				return;
			}
			size_t dataBytes = this->data.size() * sizeof(T);
			if (dataBytes != this->size) {
				this->size = dataBytes;
				glBufferData(this->type, this->size, this->data.data(), this->usage);
			}
			else {
				glBufferSubData(this->type, 0, this->size, this->data.data());
			}
			unbind();
		}

		// EBO --
		EBO::EBO(GLenum usage) {
			this->type = GL_ELEMENT_ARRAY_BUFFER;
			this->usage = usage;
			this->size = 0;

			glGenBuffers(1, &this->ID);
			glBindBuffer(this->type, this->ID);
			glBufferData(this->type, 0, nullptr, this->usage);
		}
		void EBO::loadData() {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
			if (this->data.empty()) {
				return;
			}
			size_t dataBytes = this->data.size() * sizeof(GLuint);
			if (dataBytes != this->size) {
				this->size = dataBytes;
				glBufferData(this->type, this->size, this->data.data(), this->usage);
			}
			else {
				glBufferSubData(this->type, 0, this->size, this->data.data());
			}
		}
		void EBO::draw() {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ID);
			glDrawElements(GL_TRIANGLES, data.size(), GL_UNSIGNED_INT, 0);
		}

		// TFB --
		template<typename T>
		TFB<T>::TFB(GLenum usage) {
			this->type = GL_TRANSFORM_FEEDBACK_BUFFER;
			this->usage = usage;
			this->size = 0;

			glGenBuffers(1, &this->ID);
			glGenTransformFeedbacks(1, &tfID);
		}
		template<typename T>
		TFB<T>::~TFB() {
			glDeleteTransformFeedbacks(1, &tfID);
		}
		template<typename T>
		void TFB<T>::bindFeedback() {
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfID);
		}
		template<typename T>
		void TFB<T>::bindToFeedback(GLuint index) {
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, this->ID);
		}
		template<typename T>
		void TFB<T>::begin(GLenum primitiveMode) {
			glEnable(GL_RASTERIZER_DISCARD);
			bindFeedback();
			glBeginTransformFeedback(primitiveMode);
		}
		template<typename T>
		void TFB<T>::end() {
			glEndTransformFeedback();
			glDisable(GL_RASTERIZER_DISCARD);
		}
		template<typename T>
		void TFB<T>::readData() {
			glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, this->ID);
			void* ptr = glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_READ_ONLY);
			if (ptr) {
				T* dataPtr = static_cast<T*>(ptr);
				this->data.assign(dataPtr, dataPtr + this->data.size());
				glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
			}
			else {
				std::cerr << "Failed to map transform feedback buffer!" << std::endl;
			}
		}
		template class TFB<float>;
	}

	// VAO --
	VAO::VAO() {
		glGenVertexArrays(1, &ID);
	}
	VAO::~VAO() {
		glDeleteVertexArrays(1, &ID);
	}
	void VAO::bind() {
		glBindVertexArray(ID);
	}
	void VAO::unbind() {
		glBindVertexArray(0);
	}
	template<typename T>
	void VAO::configure(buffer<T>* buffer, GLuint index, GLint size, int dataSize, int offSet) {
		bind();
		glBindBuffer(buffer->type, buffer->ID);
		glVertexAttribPointer(index, size, buffer->dataType, GL_FALSE, dataSize * sizeof(T), (void*)(offSet * sizeof(T)));
		glEnableVertexAttribArray(index);
		unbind();
	}
	template void VAO::configure<float>(buffer<float>*, GLuint, GLint, int, int);
	template void VAO::configure<GLfloat>(buffer<GLfloat>*, GLuint, GLint, int, int);
	template void VAO::configure<double>(buffer<double>*, GLuint, GLint, int, int);
	template void VAO::configure<GLdouble>(buffer<GLdouble>*, GLuint, GLint, int, int);
	template void VAO::configure<int>(buffer<int>*, GLuint, GLint, int, int);
	template void VAO::configure<GLint>(buffer<GLint>*, GLuint, GLint, int, int);
	template void VAO::configure<unsigned int>(buffer<unsigned int>*, GLuint, GLint, int, int);
	template void VAO::configure<GLuint>(buffer<GLuint>*, GLuint, GLint, int, int);
	template void VAO::configure<short>(buffer<short>*, GLuint, GLint, int, int);
	template void VAO::configure<GLshort>(buffer<GLshort>*, GLuint, GLint, int, int);
	template void VAO::configure<unsigned short>(buffer<unsigned short>*, GLuint, GLint, int, int);
	template void VAO::configure<GLushort>(buffer<GLushort>*, GLuint, GLint, int, int);
	template void VAO::configure<char>(buffer<char>*, GLuint, GLint, int, int);
	template void VAO::configure<GLbyte>(buffer<GLbyte>*, GLuint, GLint, int, int);
	template void VAO::configure<unsigned char>(buffer<unsigned char>*, GLuint, GLint, int, int);
	template void VAO::configure<GLubyte>(buffer<GLubyte>*, GLuint, GLint, int, int);
	template void VAO::configure<bool>(buffer<bool>*, GLuint, GLint, int, int);
	template void VAO::configure<GLboolean>(buffer<GLboolean>*, GLuint, GLint, int, int);

	// shaderProgram --
	shaderProgram::shaderProgram() {
		ID = glCreateProgram();
	}
	shaderProgram::~shaderProgram() {
		glDeleteProgram(ID);
	}
	void shaderProgram::addShader(GLenum shaderType, const std::string& shaderFilePath) {
		std::ifstream file(shaderFilePath);
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string shaderData = buffer.str();

		GLuint shader = glCreateShader(shaderType);
		const char* shaderSource = shaderData.c_str();

		glShaderSource(shader, 1, &shaderSource, nullptr);
		glCompileShader(shader);

		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[1024];
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
			std::cerr << "ERROR::SHADER_COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		glAttachShader(ID, shader);
		shaderIDs.push_back(shader);
	}
	void shaderProgram::compile() {
		glLinkProgram(ID);
		size_t size = shaderIDs.size();
		for (int i = 0; i < size; i++)
			glDeleteShader(shaderIDs.at(i));
		shaderIDs.clear();
		shaderIDs.shrink_to_fit();
	}
	void shaderProgram::useProgram() {
		glUseProgram(ID);
	}

	// window --
	void window::onResize(int width, int height) {
		transform.size.x = width;
		transform.size.y = height;
	}
	void window::framebuffer_size_callback(GLFWwindow* win, int width, int height) {
		window* self = static_cast<window*>(glfwGetWindowUserPointer(win));
		if (self) {
			self->onResize(width, height);
		}
	}
	void onMove(GLFWwindow* window, int x, int y) {

	}
	window::window(int widht, int height, bool fullscreen, std::string WINname) {
		transform.size.x = widht;
		transform.size.y = height;
		this->fullscreen = fullscreen;

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);

		monitor = glfwGetPrimaryMonitor();
		mode = glfwGetVideoMode(monitor);
		transform.position.x = (mode->width - transform.size.x) / 2;
		transform.position.y = (mode->height - transform.size.y) / 2;
		ID = glfwCreateWindow(
			(fullscreen ? mode->width : transform.size.x),
			(fullscreen ? mode->height : transform.size.y),
			WINname.c_str(),
			(fullscreen ? monitor : nullptr),
			nullptr);
		glfwMakeContextCurrent(ID);
		gladLoadGL(glfwGetProcAddress);
		glViewport(0, 0, transform.size.x, transform.size.y);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_SCISSOR_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glfwSetWindowUserPointer(ID, this);
		glfwSetFramebufferSizeCallback(ID, framebuffer_size_callback);

		glfwMakeContextCurrent(ID);
	}
	window::~window() {
		glfwDestroyWindow(ID);
		glfwTerminate();
	}
	GLFWmonitor* window::getCurrentMonitor(int X, int Y) {
		int monitorCount;
		GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
		GLFWmonitor* myMonitor = nullptr;

		for (int i = 0; i < monitorCount; i++) {
			int mX, mY;
			glfwGetMonitorPos(monitors[i], &mX, &mY);
			const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);

			if (X >= mX && X <= (mX + mode->width) &&
				Y >= mY && Y <= (mY + mode->height)) {
				myMonitor = monitors[i];
				return myMonitor;
			}
		}
	}
	void window::resize(int widht, int height, int X, int Y, bool fullscreen) {
		transform.size.x = (widht == -1) ? transform.size.x : widht;
		transform.size.y = (height == -1) ? transform.size.y : height;
		transform.position.x = (X == -1) ? transform.position.x : X;
		transform.position.y = (Y == -1) ? transform.position.y : Y;
		this->fullscreen = fullscreen;

		monitor = getCurrentMonitor(transform.position.x + transform.size.x / 2, transform.position.y + transform.size.y / 2);
		mode = glfwGetVideoMode(monitor);

		glfwSetWindowMonitor(ID,
			(fullscreen ? monitor : nullptr),
			transform.position.x,
			transform.position.y,
			(fullscreen ? mode->width : transform.size.x),
			(fullscreen ? mode->height : transform.size.y),
			(fullscreen ? mode->refreshRate : GL_DONT_CARE));
	}
	void window::setView(glm::vec2 pixel_position, glm::vec2 window_position, glm::vec2 pixel_size, glm::vec2 window_size) {
		glm::vec4 viewport = glm::vec4(
			pixel_position.x + window_position.x * transform.size.x,
			pixel_position.y + window_position.y * transform.size.y,
			pixel_size.x + window_size.x * transform.size.x,
			pixel_size.y + window_size.y * transform.size.y);
		glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
		glScissor(viewport.x, viewport.y, viewport.z, viewport.w);
	}
	bool window::getMouse(glm::vec2* position) {
		double cursorX, cursorY;
		glfwGetCursorPos(ID,&cursorX, &cursorY);
		position->x = cursorX - transform.position.x;
		position->y = cursorY - transform.position.y;
		return (cursorX > transform.position.x && cursorX < transform.position.x + transform.size.x &&
			cursorY > transform.position.y && cursorY < transform.position.y + transform.size.y);
	}
	bool window::getMouse() {
		double cursorX, cursorY;
		glfwGetCursorPos(ID, &cursorX, &cursorY);
		return (cursorX > transform.position.x && cursorX < transform.position.x + transform.size.x &&
			cursorY > transform.position.y && cursorY < transform.position.y + transform.size.y);
	}
}

namespace Element {
	// mesh --
	void mesh::triangle(glm::vec3 vertexPos1, glm::vec3 vertexPos2, glm::vec3 vertexPos3, glm::vec4 color) {
		vertacies.push_back(vertexPos1.x);
		vertacies.push_back(vertexPos1.y);
		vertacies.push_back(vertexPos1.z);

		vertacies.push_back(vertexPos2.x);
		vertacies.push_back(vertexPos2.y);
		vertacies.push_back(vertexPos2.z);

		vertacies.push_back(vertexPos3.x);
		vertacies.push_back(vertexPos3.y);
		vertacies.push_back(vertexPos3.z);

		for (int i = 0; i < 3; i++) {
			colors.push_back(color.x);
			colors.push_back(color.y);
			colors.push_back(color.z);
			colors.push_back(color.a);
		}
	}
	void mesh::colorTriangle(glm::vec3 vertexPos1, glm::vec3 vertexPos2, glm::vec3 vertexPos3, glm::vec4 color1, glm::vec4 color2, glm::vec4 color3) {
		vertacies.push_back(vertexPos1.x);
		vertacies.push_back(vertexPos1.y);
		vertacies.push_back(vertexPos1.z);

		vertacies.push_back(vertexPos2.x);
		vertacies.push_back(vertexPos2.y);
		vertacies.push_back(vertexPos2.z);

		vertacies.push_back(vertexPos3.x);
		vertacies.push_back(vertexPos3.y);
		vertacies.push_back(vertexPos3.z);

		colors.push_back(color1.x);
		colors.push_back(color1.y);
		colors.push_back(color1.z);
		colors.push_back(color1.a);

		colors.push_back(color2.x);
		colors.push_back(color2.y);
		colors.push_back(color2.z);
		colors.push_back(color2.a);

		colors.push_back(color3.x);
		colors.push_back(color3.y);
		colors.push_back(color3.z);
		colors.push_back(color3.a);
	}
	void mesh::rectangle(glm::vec3 position, glm::vec4 rotation, glm::vec2 size, glm::vec4 color) {
		glm::quat rotQuat = glm::quat(glm::radians(rotation.a), rotation.x, rotation.y, rotation.z);
		rotQuat = glm::normalize(rotQuat);

		glm::vec3 corners[4] = {
			glm::vec3(0, 0, 0),
			glm::vec3(size.x, 0, 0),
			glm::vec3(0, size.y, 0),
			glm::vec3(size.x, size.y, 0)
		};
		for (int i = 0; i < 4; ++i) {
			corners[i] = rotQuat * corners[i] + position;
		}
		triangle(corners[0], corners[1], corners[2], color);
		triangle(corners[2], corners[1], corners[3], color);
	}
	void mesh::circle(glm::vec3 position, glm::vec4 rotation, float radius, int segments, glm::vec4 color) {
		if (segments < 3) segments = 3;
		float angleStep = glm::two_pi<float>() / segments;

		// Create quaternion from rotation
		glm::quat rotQuat = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);

		for (int i = 0; i < segments; ++i) {
			float angle1 = i * angleStep;
			float angle2 = (i + 1) * angleStep;

			glm::vec3 point1 = glm::vec3(radius * cos(angle1), radius * sin(angle1), 0);
			glm::vec3 point2 = glm::vec3(radius * cos(angle2), radius * sin(angle2), 0);

			// Apply rotation and translation
			point1 = rotQuat * point1 + position;
			point2 = rotQuat * point2 + position;

			triangle(position, point1, point2, color);
		}
	}
	void mesh::cube(glm::vec3 position, glm::vec4 rotation, glm::vec3 size, glm::vec4 color) {
		glm::quat rotQuat = glm::yawPitchRoll(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
		rotQuat = glm::normalize(rotQuat);

		glm::vec3 corners[8] = {
			glm::vec3(0, 0, 0),
			glm::vec3(size.x, 0, 0),
			glm::vec3(0, size.y, 0),
			glm::vec3(size.x, size.y, 0),
			glm::vec3(0, 0, size.z),
			glm::vec3(size.x, 0, size.z),
			glm::vec3(0, size.y, size.z),
			glm::vec3(size.x, size.y, size.z)
		};
		for (int i = 0; i < 8; ++i) {
			corners[i] = rotQuat * corners[i] + position;
		}
		triangle(corners[0], corners[1], corners[2], color);
		triangle(corners[2], corners[1], corners[3], color);

		triangle(corners[2], corners[3], corners[4], color);
		triangle(corners[4], corners[3], corners[5], color);

		triangle(corners[4], corners[5], corners[6], color);
		triangle(corners[6], corners[5], corners[7], color);

		triangle(corners[6], corners[7], corners[0], color);
		triangle(corners[0], corners[7], corners[1], color);

		triangle(corners[0], corners[1], corners[5], color);
		triangle(corners[0], corners[4], corners[5], color);

		triangle(corners[2], corners[3], corners[6], color);
		triangle(corners[4], corners[6], corners[7], color);
	}
	void mesh::sphere(glm::vec3 position, float radius, float segments, glm::vec3 size, glm::vec4 color) {
		int seg = (int)segments;
		for (int y = 0; y <= seg; y++) {
			float v0 = (float)y / seg;
			float v1 = (float)(y + 1) / seg;

			float theta0 = v0 * glm::pi<float>();
			float theta1 = v1 * glm::pi<float>();

			for (int x = 0; x <= seg; x++) {
				float u0 = (float)x / seg;
				float u1 = (float)(x + 1) / seg;

				float phi0 = u0 * glm::two_pi<float>();
				float phi1 = u1 * glm::two_pi<float>();

				// 4 points of the current quad
				glm::vec3 p0 = {
					radius * sin(theta0) * cos(phi0),
					radius * cos(theta0),
					radius * sin(theta0) * sin(phi0)
				};
				glm::vec3 p1 = {
					radius * sin(theta1) * cos(phi0),
					radius * cos(theta1),
					radius * sin(theta1) * sin(phi0)
				};
				glm::vec3 p2 = {
					radius * sin(theta1) * cos(phi1),
					radius * cos(theta1),
					radius * sin(theta1) * sin(phi1)
				};
				glm::vec3 p3 = {
					radius * sin(theta0) * cos(phi1),
					radius * cos(theta0),
					radius * sin(theta0) * sin(phi1)
				};

				// Offset by sphere position
				p0 += position;
				p1 += position;
				p2 += position;
				p3 += position;

				// Two triangles per quad
				triangle(p0, p1, p2, color);
				triangle(p0, p2, p3, color);
			}
		}
	}
	void mesh::debug(bool debug) {
		if (!debugSeed) {
			std::random_device rd;
			debugSeed = rd();
		}
		if (debugOn == debug)
			return;

		debugOn = debug;

		std::mt19937 rng(debugSeed);
		std::uniform_real_distribution<float> dist(0.0f, 1.0f);

		int triCount = vertacies.size() / 9;
		switch (debug) {
		case true:
			colDebug = colors;
			colors.clear();
			for (int x = 0; x < triCount; x++) {
				glm::vec3 randomCol = glm::vec3(dist(rng), dist(rng), dist(rng));
				for (int i = 0; i < 3; i++) {
					colors.push_back(randomCol.x);
					colors.push_back(randomCol.y);
					colors.push_back(randomCol.z);
					colors.push_back(1);
				}
			}
			break;
		case false:
			colors = colDebug;
			colDebug.clear();
			colDebug.shrink_to_fit();
			break;
		}
	}

	// texture --
	texture::texture(std::string png_path) {
		int w, h, c;
		unsigned char* raw = stbi_load(png_path.c_str(), &w, &h, &c, 4);
		width = w;
		height = h;
		channels = 4;
		data.assign(raw, raw + width * height * 4);
		stbi_image_free(raw);
	}
	texture::texture() {
		width = 1;
		height = 1;
		channels = 4;
		data = { 255, 255, 255, 255 };
	}

	// layer --
	layer::layer::layer(GL::VAO* VAO) :
		EBO(GL_DYNAMIC_DRAW),
		posVBO(GL_DYNAMIC_DRAW),
		colVBO(GL_DYNAMIC_DRAW),
		objectVBO(GL_DYNAMIC_DRAW),
		texVBO(GL_DYNAMIC_DRAW),
		texIDVBO(GL_DYNAMIC_DRAW) {
	}
	layer::~layer() {}
	void layer::render(GL::window* window, GL::shaderProgram* shader, GL::VAO* VAO) {
		if (camera.depth) {
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
		}
		else {
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
		}

		shader->useProgram();

		posVBO.data.clear();
		colVBO.data.clear();
		objectVBO.data.clear();
		EBO.data.clear();

		size_t verticiesCount = 0;

		GLint cameraROT = glGetUniformLocation(shader->ID, "cameraRotation");
		GLint cameraPOS = glGetUniformLocation(shader->ID, "cameraPosition");
		GLint cameraSIZ = glGetUniformLocation(shader->ID, "cameraSize");
		GLint cameraFOV = glGetUniformLocation(shader->ID, "cameraFOV");

		GLint aspectRatio = glGetUniformLocation(shader->ID, "aspectRatio");
		GLint nearPlane = glGetUniformLocation(shader->ID, "nearPlane");
		GLint farPlane = glGetUniformLocation(shader->ID, "farPlane");

		glUniform4f(cameraROT, camera.transform.rotation.x, camera.transform.rotation.y, camera.transform.rotation.z, camera.transform.rotation.a);
		glUniform3f(cameraPOS, camera.transform.position.x, camera.transform.position.y, camera.transform.position.z);
		glUniform3f(cameraSIZ, camera.transform.size.x, camera.transform.size.y, camera.transform.size.z);
		glUniform1f(cameraFOV, camera.FOV);

		glUniform1f(aspectRatio, window->transform.size.x / window->transform.size.y);
		glUniform1f(nearPlane, 0.1f);
		glUniform1f(farPlane, 1000.0f);

		for (auto& [key, object] : objects) {
			if (!object.model) continue;

			for (int i = 0; i < (object.model->mesh.vertacies.size() / 3); i++) {
				posVBO.data.push_back(object.model->mesh.vertacies[i * 3]);
				posVBO.data.push_back(object.model->mesh.vertacies[i * 3 + 1]);
				posVBO.data.push_back(object.model->mesh.vertacies[i * 3 + 2]);

				objectVBO.data.push_back(object.transform.position.x);
				objectVBO.data.push_back(object.transform.position.y);
				objectVBO.data.push_back(object.transform.position.z);

				objectVBO.data.push_back(object.transform.rotation.x);
				objectVBO.data.push_back(object.transform.rotation.y);
				objectVBO.data.push_back(object.transform.rotation.z);
				objectVBO.data.push_back(object.transform.rotation.a);

				objectVBO.data.push_back(object.transform.size.x);
				objectVBO.data.push_back(object.transform.size.y);
				objectVBO.data.push_back(object.transform.size.z);

				colVBO.data.push_back(object.model->mesh.colors[i * 4]);
				colVBO.data.push_back(object.model->mesh.colors[i * 4 + 1]);
				colVBO.data.push_back(object.model->mesh.colors[i * 4 + 2]);
				colVBO.data.push_back(object.model->mesh.colors[i * 4 + 3]);

				EBO.data.push_back(verticiesCount++);
			}
		}
		posVBO.loadData();
		colVBO.loadData();
		objectVBO.loadData();
		EBO.loadData();

		VAO->bind();
		EBO.draw();
		VAO->unbind();
	}
}