#include "Include.h"
#include "framework.h"

GL::window window(800, 600, false, "image test");
GL::VAO VAO;

int main() {
    Element::Storage::modelStorage modelStorage;

    VAO.bind();
    Element::layer layer(&VAO);
    VAO.unbind();

    VAO.configure(&layer.posVBO, 0, 3, 3, 0);
    VAO.configure(&layer.colVBO, 1, 4, 4, 0);
    VAO.configure(&layer.objectVBO, 2, 3, 10, 0);
    VAO.configure(&layer.objectVBO, 3, 4, 10, 3);
    VAO.configure(&layer.objectVBO, 4, 3, 10, 7);

    GL::shaderProgram shader;
    shader.addShader(GL_VERTEX_SHADER, "Vertex.txt");
    shader.addShader(GL_FRAGMENT_SHADER, "Fragment.txt");
    shader.addShader(GL_GEOMETRY_SHADER, "Geometry.txt");
    shader.compile();

    modelStorage.models["cubes"].mesh.cube(glm::vec3(-5, -5, 20), glm::vec4(0, 0, 0, 0), glm::vec3(10, 10, 10), glm::vec4(1, 0, 0, 1));
    modelStorage.models["cubes"].mesh.cube(glm::vec3(20, -5, -5), glm::vec4(0, 0, 0, 0), glm::vec3(10, 10, 10), glm::vec4(0, 1, 0, 1));
    modelStorage.models["cubes"].mesh.cube(glm::vec3(-5, 20, -5), glm::vec4(0, 0, 0, 0), glm::vec3(10, 10, 10), glm::vec4(0, 0, 1, 1));
    modelStorage.models["test"].mesh.circle(glm::vec3(20, 20, 20), glm::vec4(0, 0, 0, 0), 5, 20, glm::vec4(1, 1, 1, 0.5));
    modelStorage.models["cubes"].mesh.sphere(glm::vec3(-20, -20, -20), 5, 20, glm::vec3(1, 1, 1), glm::vec4(0, 0, 1, 1));

    layer.objects["cubes"].model = &modelStorage.models["cubes"];
    layer.objects["test"].model = &modelStorage.models["test"];

    modelStorage.models["cubes"].mesh.debug(true);

    glm::vec2 lastCursor;
    window.getMouse(&lastCursor);

    float c = 0;
    while (!glfwWindowShouldClose(window.ID)) {
        bool validQuat = (layer.camera.transform.rotation.x || layer.camera.transform.rotation.y || layer.camera.transform.rotation.z);
        glm::quat camera = glm::angleAxis(
            glm::radians((validQuat) ? layer.camera.transform.rotation.w : 0),
            glm::vec3(
                (validQuat) ? layer.camera.transform.rotation.x : 1,
                layer.camera.transform.rotation.y,
                layer.camera.transform.rotation.z));
        glm::vec3 forward = glm::rotate(camera, glm::vec3(0, 0, 1));
        glm::vec3 right = glm::rotate(camera, glm::vec3(1, 0, 0));
        glm::vec3 up = glm::rotate(camera, glm::vec3(0, 1, 0));

        const float speed = 0.1;
        if (glfwGetKey(window.ID, GLFW_KEY_W) == GLFW_PRESS) {
            layer.camera.transform.position += forward * speed;
        }
        if (glfwGetKey(window.ID, GLFW_KEY_S) == GLFW_PRESS) {
            layer.camera.transform.position -= forward * speed;
        }
        if (glfwGetKey(window.ID, GLFW_KEY_D) == GLFW_PRESS) {
            layer.camera.transform.position += right * speed;
        }
        if (glfwGetKey(window.ID, GLFW_KEY_A) == GLFW_PRESS) {
            layer.camera.transform.position -= right * speed;
        }
        if (glfwGetKey(window.ID, GLFW_KEY_SPACE) == GLFW_PRESS) {
            layer.camera.transform.position += up * speed;
        }
        if (glfwGetKey(window.ID, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            layer.camera.transform.position -= up * speed;
        }

        glm::vec2 currentCursor;
        window.getMouse(&currentCursor);
        glm::vec2 deltaCursor = currentCursor - lastCursor;
        lastCursor = currentCursor;

        if (glfwGetMouseButton(window.ID, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            const float sensitivity = 0.5;

            bool validQuat = (layer.camera.transform.rotation.x || layer.camera.transform.rotation.y || layer.camera.transform.rotation.z);

            glm::quat camera = glm::angleAxis(
                glm::radians((validQuat) ? layer.camera.transform.rotation.w : 0),
                glm::vec3(
                    (validQuat) ? layer.camera.transform.rotation.x : 1, 
                    layer.camera.transform.rotation.y,
                    layer.camera.transform.rotation.z));
            glm::quat rotX = glm::angleAxis(glm::radians(deltaCursor.x * sensitivity), glm::vec3(0, 1, 0));
            glm::quat rotY = glm::angleAxis(glm::radians(deltaCursor.y * sensitivity), glm::vec3(1, 0, 0));

            camera = glm::normalize(camera);
            rotX = glm::normalize(rotX);
            rotY = glm::normalize(rotY);

            camera = rotX * camera;
            camera = camera * rotY;

            layer.camera.transform.rotation = glm::vec4(glm::axis(camera), glm::degrees(glm::angle(camera)));
        }

        window.setView(glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(1, 1));
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        layer.render(&window, &shader, &VAO);

        glfwPollEvents();
        glfwSwapBuffers(window.ID);
    }
    return 0;
}