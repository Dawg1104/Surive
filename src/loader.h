#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "mesh.h"

Mesh LoadOBJ(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        // Return empty mesh
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

                // OBJ format: posIndex/texIndex/normIndex (some parts optional)
                std::getline(vss, posIndexStr, '/');
                std::getline(vss, texIndexStr, '/');
                std::getline(vss, normIndexStr, '/');

                int posIndex = std::stoi(posIndexStr) - 1; // OBJ is 1-based
                int texIndex = texIndexStr.empty() ? -1 : std::stoi(texIndexStr) - 1;
                int normIndex = normIndexStr.empty() ? -1 : std::stoi(normIndexStr) - 1;

                Vertex vertex{};

                if (posIndex >= 0 && posIndex < positions.size())
                    vertex.Position = positions[posIndex];
                else
                    vertex.Position = glm::vec3(0.0f);

                if (texIndex >= 0 && texIndex < texCoords.size())
                    vertex.TexCoords = texCoords[texIndex];
                else
                    vertex.TexCoords = glm::vec2(0.0f);

                if (normIndex >= 0 && normIndex < normals.size())
                    vertex.Normal = normals[normIndex];
                else
                    vertex.Normal = glm::vec3(0.0f);

                vertices.push_back(vertex);
                indices.push_back(static_cast<unsigned int>(vertices.size() - 1));
            }
        }
    }
	file.close();

    return Mesh(vertices, indices, {}); // You can add textures later

}