#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stb/stb_image.h>
#include <generation/Images.h>

#include <iostream>
#include <vector>
#include <random>
#include <cmath>

#include "mesh.h"
#include "loader.h"
#include "Shader.h"
#include "Camera.h"

#include <filesystem>
#include <vector>
#include <string>
#include <iostream>

namespace fs = std::filesystem;

// 1. Replace your model loading with something like this:

std::unordered_map<std::string, Mesh> modelLibrary; // name -> Mesh
std::vector<std::string> modelNames;                // ordered list of names for UI

void LoadModels(const std::string& directory)
{
    for (const auto& entry : fs::directory_iterator(directory))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".obj")
        {
            std::string name = entry.path().stem().string(); // filename without extension
            std::cout << "Loading model: " << entry.path().string() << std::endl;
            Mesh mesh = LoadOBJ(entry.path().string());
            modelLibrary[name] = std::move(mesh);
            modelNames.push_back(name);
        }
    }
}

int SCR_WIDTH = 1280, SCR_HEIGHT = 720;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float blockSize = 16.0f;
const int CHUNK_SIZE = 32;

struct GameObject {
    std::string name;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale = glm::vec3(1.0f);
    Mesh* mesh; // pointer to shared mesh data
};

std::vector<GameObject> objects;


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

std::vector<GLuint64> LoadNecessaryTextures(const std::vector<std::string>& texturePaths)
{
    std::vector<GLuint64> textureHandles;
    textureHandles.reserve(texturePaths.size());

    stbi_set_flip_vertically_on_load(true);

    for (const auto& path : texturePaths)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (!data)
        {
            std::cerr << "Failed to load texture: " << path << std::endl;
            textureHandles.push_back(0);
            continue;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Default texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Bindless handle
        GLuint64 handle = glGetTextureHandleARB(textureID);
        glMakeTextureHandleResidentARB(handle);
        textureHandles.push_back(handle);

        stbi_image_free(data);
    }

    return textureHandles;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Update viewport
    glViewport(0, 0, width, height);

    // Retrieve camera pointer from GLFW user pointer (optional)
    Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (cam)
    {
        cam->SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if (!window)
    {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "GLAD init failed\n";
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3{ 0.0f, 10.0f, 0.0f });

    glfwSetWindowUserPointer(window, &camera);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader shader("../../assets/shaders/default.vert", "../../assets/shaders/default.frag");

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    glEnable(GL_DEPTH_TEST);

    bool wires = false;

    int seed = 1;

    int worldWidth = SCR_WIDTH;
    int worldHeight = SCR_HEIGHT;

    LoadModels("../../assets/models");
    int currentModelIndex = 0;

    glm::mat4 model{ 1.0f };
    glm::vec3 rotAxis{ 0.0f, 1.0f, 0.0f };

    glEnable(GL_CULL_FACE);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        for (auto& obj : objects) {
            obj.rotation.y += 45.0f * deltaTime; // Rotate 45 degrees per second
        }
        
        glClearColor(0.0f, 0.737f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        for (auto& obj : objects)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), obj.position);
            model = glm::rotate(model, glm::radians(obj.rotation.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(obj.rotation.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(obj.rotation.z), glm::vec3(0, 0, 1));
            model = glm::scale(model, obj.scale);

            shader.setMat4("model", model);
            obj.mesh->Draw(shader);
        }
        
        if (!io.WantCaptureMouse)
            camera.Inputs(window, deltaTime);
        camera.Matrix(60.0f, 0.001f, 1000.0f ,shader, "camMatrix");

        // ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Debug");
        ImGui::Text("Entity Count: %d", objects.size());
        ImGui::Text("Camera Pos: %.2f, %.2f, %.2f", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Checkbox("Wireframe Mode", &wires);
		glPolygonMode(GL_FRONT_AND_BACK, wires ? GL_LINE : GL_FILL);
        static int selectedModel = 0;

        if (!modelNames.empty())
        {
            ImGui::Combo("Model", &selectedModel,
                [](void* data, int idx, const char** out_text) {
                    auto& names = *static_cast<std::vector<std::string>*>(data);
                    *out_text = names[idx].c_str();
                    return true;
                },
                &modelNames, static_cast<int>(modelNames.size()));
            
            if (ImGui::Button("Spawn"))
            {
                const std::string& name = modelNames[selectedModel];
                GameObject obj;
                obj.name = name;
                obj.position = glm::vec3(0.0f);
                obj.rotation = glm::vec3(0.0f);
                obj.scale = glm::vec3(1.0f);
                obj.mesh = &modelLibrary[name];
                objects.push_back(obj);
            }
        }
        else
        {
            ImGui::Text("No models loaded");
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}