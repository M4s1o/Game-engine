#pragma once
#include "Include.h"
#include "attributes.h"

namespace GL {
    template<typename T>
	class buffer {
	public:
		GLuint ID;
		GLenum type;
		GLenum usage;
		size_t size;

        std::vector<T> data;
        GLenum dataType;

        buffer();
        ~buffer();
	};
    // finish buffers
    namespace Buffer {
        // TODO: ---------------------------------
        //    CRB -> GL_COPY_READ_BUFFER
        //    CWB -> GL_COPY_WRITE_BUFFER
        //    PBO -> GL_PIXEL_PACK_BUFFER
        //    PBO -> GL_PIXEL_UNPACK_BUFFER
        //    TBO -> GL_TEXTURE_BUFFER
        //    TFB -> GL_TRANSFORM_FEEDBACK_BUFFER
        //    UBO -> GL_UNIFORM_BUFFER
        //    SSBO -> GL_SHADER_STORAGE_BUFFER
        //    DIB -> GL_DRAW_INDIRECT_BUFFER
        //    DPIB -> GL_DISPATCH_INDIRECT_BUFFER
        //    ACBO -> GL_ATOMIC_COUNTER_BUFFER
        //    QBO -> GL_QUERY_BUFFER

        // Vertex Buffer Object
        template<typename T>
        class VBO : public buffer<T> {
        public:
            VBO(GLenum usage = GL_STATIC_DRAW);
            void bind();
            void unbind();
            void loadData();
        };
        // Element Buffer Object (Index Buffer)
        class EBO : public buffer<GLuint> {
        public:
            EBO(GLenum usage = GL_STATIC_DRAW);
            void loadData();
            void draw();
        };
        // Uniform Buffer Object
        template<typename T>
        class UBO : public buffer<T> {
        public:
            UBO(GLenum usage = GL_STATIC_DRAW) {
                this->type = GL_UNIFORM_BUFFER;
                this->usage = usage;
            }
        };
        // Shader Storage Buffer Object
        template<typename T>
        class SSBO : public buffer<T> {
        public:
            SSBO(GLenum usage = GL_DYNAMIC_COPY) {
                this->type = GL_SHADER_STORAGE_BUFFER;
                this->usage = usage;
            }
        };
        // Atomic Counter Buffer
        template<typename T>
        class ACBO : public buffer<T> {
        public:
            ACBO(GLenum usage = GL_DYNAMIC_DRAW) {
                this->type = GL_ATOMIC_COUNTER_BUFFER;
                this->usage = usage;
            }
        };
        // Transform Feedback Buffer
        template<typename T>
        class TFB : public buffer<T> {
        public:
            GLuint tfID;

            TFB(GLenum usage = GL_STATIC_DRAW);
            ~TFB();

            void bindFeedback();
            void bindToFeedback(GLuint index);
            void begin(GLenum primitiveMode = GL_TRIANGLES);
            void end();
            void readData();
        };
        // Texture Buffer Object
        template<typename T>
        class TBO : public buffer<T> {
        public:
            TBO(GLenum usage = GL_STATIC_DRAW) {
                this->type = GL_TEXTURE_BUFFER;
                this->usage = usage;
            }
        };
        // Pixel Pack Buffer
        template<typename T>
        class PBO_Pack : public buffer<T> {
        public:
            PBO_Pack(GLenum usage = GL_STREAM_READ) {
                this->type = GL_PIXEL_PACK_BUFFER;
                this->usage = usage;
            }
        };
        // Pixel Unpack Buffer
        template<typename T>
        class PBO_Unpack : public buffer<T> {
        public:
            PBO_Unpack(GLenum usage = GL_STREAM_DRAW) {
                this->type = GL_PIXEL_UNPACK_BUFFER;
                this->usage = usage;
            }
        };
        // Copy Read Buffer
        template<typename T>
        class CRB : public buffer<T> {
        public:
            CRB(GLenum usage = GL_STATIC_COPY) {
                this->type = GL_COPY_READ_BUFFER;
                this->usage = usage;
            }
        };
        // Copy Write Buffer
        template<typename T>
        class CWB : public buffer<T> {
        public:
            CWB(GLenum usage = GL_STATIC_COPY) {
                this->type = GL_COPY_WRITE_BUFFER;
                this->usage = usage;
            }
        };
        // Draw Indirect Buffer
        template<typename T>
        class DIB : public buffer<T> {
        public:
            DIB(GLenum usage = GL_STATIC_DRAW) {
                this->type = GL_DRAW_INDIRECT_BUFFER;
                this->usage = usage;
            }
        };
        // Dispatch Indirect Buffer
        template<typename T>
        class DPIB : public buffer<T> {
        public:
            DPIB(GLenum usage = GL_STATIC_DRAW) {
                this->type = GL_DISPATCH_INDIRECT_BUFFER;
                this->usage = usage;
            }
        };
        // Query Buffer
        template<typename T>
        class QBO : public buffer<T> {
        public:
            QBO(GLenum usage = GL_STATIC_READ) {
                this->type = GL_QUERY_BUFFER;
                this->usage = usage;
            }
        };
    }

    class VAO {
    private:
        GLuint ID;
    public:
        VAO();
        ~VAO();

        void bind();
        void unbind();
        template<typename T>
        void configure(buffer<T>* buffer, GLuint index, GLint size, int vertexSize, int offSet);
    };

	class shaderProgram {
    public:
		GLuint ID;
        std::vector<GLuint> shaderIDs;

		shaderProgram();
		~shaderProgram();

		void addShader(GLenum shaderType, const std::string& shaderFilePath);
		void compile();
		void useProgram();
	};

    class window {
    public:
        GLFWwindow* ID;
        GLFWmonitor* monitor;
        const GLFWvidmode* mode;
        bool fullscreen;
        Transform transform;

        window(int wight, int height, bool fullscreen, std::string WINname);
        ~window();

        GLFWmonitor* getCurrentMonitor(int X, int Y);
        void resize(int wight, int height, int X, int Y, bool fullscreen);
        void setView(glm::vec2 pixel_position, glm::vec2 window_position, glm::vec2 pixel_size, glm::vec2 window_size);
        bool getMouse(glm::vec2* position);
        bool getMouse();
    private:
        static void framebuffer_size_callback(GLFWwindow* win, int width, int height);
        void onResize(int width, int height);
        void onMove(GLFWwindow* window, int x, int y);
    };
}

namespace Element {
    class mesh {
        bool debugOn;
        unsigned int debugSeed;
    public:
        std::vector<GLfloat> vertacies;
        std::vector<GLfloat> colors;
        std::vector<GLfloat> colDebug;

        void triangle(glm::vec3 vertexPos1, glm::vec3 vertexPos2, glm::vec3 ver3vertexPos3, glm::vec4 color);
        void colorTriangle(glm::vec3 vertexPos1, glm::vec3 vertexPos2, glm::vec3 vertexPos3, glm::vec4 color1, glm::vec4 color2, glm::vec4 color3);
        void rectangle(glm::vec3 position, glm::vec4 rotation, glm::vec2 size, glm::vec4 color);
        void circle(glm::vec3 position, glm::vec4 rotation, float radius, int segments, glm::vec4 color);

        void cube(glm::vec3 position, glm::vec4 rotation, glm::vec3 size, glm::vec4 color);
        void sphere(glm::vec3 position, float radius, float segments, glm::vec3 size, glm::vec4 color);
        void bean();

        void debug(bool debug);
    };
    class texture {
    public:
        int width;
        int height;
        int channels;
        std::vector<unsigned char> data;

        texture(std::string png_path);
        texture();
    };
    class model {
    public:
        mesh mesh;
        texture texture;
    };

    namespace Storage {
        class modelStorage {
        public:
            std::map<std::string, model> models;
        };
    }

    class object {
    public:
        model* model;
        Transform transform;
    };

    class camera {
    public:
        float FOV = 80;
        Transform transform;
        bool depth = true;
    };

    class layer {
    public:
        GL::Buffer::VBO<GLfloat> posVBO;
        GL::Buffer::VBO<GLfloat> colVBO;
        GL::Buffer::VBO<GLfloat> objectVBO;
        GL::Buffer::VBO<GLfloat> texVBO;
        GL::Buffer::VBO<GLuint> texIDVBO;
        GL::Buffer::EBO EBO;

        std::map<std::string, object> objects;
        camera camera;

        layer(GL::VAO* VAO);
        ~layer();
        void render(GL::window* window, GL::shaderProgram* shader, GL::VAO* VAO);
    };
}