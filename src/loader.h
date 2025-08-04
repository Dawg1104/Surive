#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

#include <glm/glm.hpp>

#include "mesh.h"

Mesh LoadOBJ(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::filesystem::path absPath = std::filesystem::absolute(path);
        std::cerr << "Failed to open OBJ file: " << absPath.string() << std::endl;
        return Mesh({}, {}, {});
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            positions.push_back(glm::vec3(x, y, z));
        }
        else if (prefix == "vt")
        {
            float u, v;
            iss >> u >> v;
            texCoords.push_back(glm::vec2(u, v));
        }
        else if (prefix == "vn")
        {
            float x, y, z;
            iss >> x >> y >> z;
            normals.push_back(glm::vec3(x, y, z));
        }

        else if (prefix == "f")
        {
            for (int i = 0; i < 3; ++i) // assumes triangles
            {
                std::string vertexData;
                iss >> vertexData;

                std::istringstream vss(vertexData);
                std::string posIndexStr, texIndexStr, normIndexStr;

                std::getline(vss, posIndexStr, '/');
                std::getline(vss, texIndexStr, '/');
                std::getline(vss, normIndexStr, '/');

                int posIndex = std::stoi(posIndexStr) - 1;
                int texIndex = texIndexStr.empty() ? -1 : std::stoi(texIndexStr) - 1;
                int normIndex = normIndexStr.empty() ? -1 : std::stoi(normIndexStr) - 1;

                Vertex vertex{};
                vertex.Position = (posIndex >= 0 && posIndex < positions.size()) ? positions[posIndex] : glm::vec3(0.0f);
                vertex.TexCoords = (texIndex >= 0 && texIndex < texCoords.size()) ? texCoords[texIndex] : glm::vec2(0.0f);
                vertex.Normal = (normIndex >= 0 && normIndex < normals.size()) ? normals[normIndex] : glm::vec3(0.0f);

                vertices.push_back(vertex);
                indices.push_back(static_cast<unsigned int>(vertices.size() - 1));
            }
        }
    }
	file.close();

    // After parsing the OBJ:

    if (normals.empty())
    {
        // Initialize all vertex normals to zero
        std::vector<glm::vec3> accumulatedNormals(vertices.size(), glm::vec3(0.0f));

        // Accumulate face normals for each triangle
        for (size_t i = 0; i + 2 < indices.size(); i += 3)
        {
            unsigned int i0 = indices[i];
            unsigned int i1 = indices[i + 1];
            unsigned int i2 = indices[i + 2];

            glm::vec3& v0 = vertices[i0].Position;
            glm::vec3& v1 = vertices[i1].Position;
            glm::vec3& v2 = vertices[i2].Position;

            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

            accumulatedNormals[i0] += faceNormal;
            accumulatedNormals[i1] += faceNormal;
            accumulatedNormals[i2] += faceNormal;
        }

        // Normalize the accumulated normals and assign to vertices
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            vertices[i].Normal = glm::normalize(accumulatedNormals[i]);
        }
    }

    return Mesh(vertices, indices, {}); // You can add textures later

}