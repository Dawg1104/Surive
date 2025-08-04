#pragma once

#include "Data.h"

#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <algorithm> // For std::clamp
#include <limits>    // For numeric_limits
#include <vector>

class Images
{
public:
    static void Write2DVectorToFile(std::vector<float> image, std::string name, const int width, const int height)
    {
        const int bytesPerPixel = 3; // 24-bit BMP
        const int rowStride = ((width * bytesPerPixel + 3) / 4) * 4; // Rows are padded to multiples of 4 bytes
        const int imageSize = rowStride * height;
        const int fileSize = 54 + imageSize;

        std::ofstream file = makeBMPImage(name, width, height);

        // Write pixel data (BMP stores pixels bottom-up, BGR order)
        std::vector<uint8_t> row(rowStride);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                const float& pixel = image[y * width + x];
                row[x * 3 + 0] = static_cast<uint8_t>(std::clamp(pixel, 0.0f, 1.0f) * 255.0f); // Blue
                row[x * 3 + 1] = static_cast<uint8_t>(std::clamp(pixel, 0.0f, 1.0f) * 255.0f); // Green
                row[x * 3 + 2] = static_cast<uint8_t>(std::clamp(pixel, 0.0f, 1.0f) * 255.0f); // Red
            }
            file.write(reinterpret_cast<char*>(row.data()), rowStride);
        }
    }

    static void WriteVectorToFile(std::vector<float> image, std::string name, const int width, const int height)
    {
        const int bytesPerPixel = 3; // 24-bit BMP
        const int rowStride = ((width * bytesPerPixel + 3) / 4) * 4; // Rows are padded to multiples of 4 bytes
        const int imageSize = rowStride * height;
        const int fileSize = 54 + imageSize;

        std::ofstream file = makeBMPImage(name, width, height); // Assumes this function writes BMP header

        if (!file)
        {
            std::cerr << "Failed to open file for writing: " << name << std::endl;
            return;
        }

        std::vector<uint8_t> row(rowStride, 0);

        for (int y = height - 1; y >= 0; y--) // BMP stores rows bottom-up
        {
            for (int x = 0; x < width; x++)
            {
                float pixel = std::clamp(image[y * width + x], 0.0f, 1.0f);
                uint8_t value = static_cast<uint8_t>(pixel * 255.0f);

                int offset = x * bytesPerPixel;
                row[offset + 0] = value; // Blue
                row[offset + 1] = value; // Green
                row[offset + 2] = value; // Red
            }
            file.write(reinterpret_cast<char*>(row.data()), rowStride);
        }

        file.close();
    }

    static void StitchChunksToImage(
        const std::string& outputName,
        int chunkSize,
        int gridWidth, int gridHeight,
        const std::string& prefix = "test")
    {
        const int bytesPerPixel = 3;
        const int rowStride = ((chunkSize * bytesPerPixel + 3) / 4) * 4;
        const int stitchedWidth = chunkSize * gridWidth;
        const int stitchedHeight = chunkSize * gridHeight;
        const int stitchedStride = ((stitchedWidth * bytesPerPixel + 3) / 4) * 4;

        std::vector<uint8_t> finalImage(stitchedStride * stitchedHeight, 0);

        for (int chunkY = 0; chunkY < gridHeight; ++chunkY)
        {
            for (int chunkX = 0; chunkX < gridWidth; ++chunkX)
            {
                std::string filename = prefix + std::to_string(chunkX) + ", " + std::to_string(chunkY) + ".bmp";

                std::ifstream file(filename, std::ios::binary);
                if (!file.is_open())
                {
                    std::cerr << "Failed to open: " << filename << "\n";
                    break;
                }

                file.seekg(54); // Skip BMP header
                std::vector<uint8_t> chunkPixels(rowStride * chunkSize);
                file.read(reinterpret_cast<char*>(chunkPixels.data()), chunkPixels.size());

                for (int y = 0; y < chunkSize; ++y)
                {
                    int srcY = y;
                    int dstY = stitchedHeight - 1 - (chunkY * chunkSize + srcY);
                    int dstIndex = dstY * stitchedStride + chunkX * chunkSize * bytesPerPixel;
                    int srcIndex = srcY * rowStride;

                    std::copy_n(
                        &chunkPixels[srcIndex],
                        chunkSize * bytesPerPixel,
                        &finalImage[dstIndex]
                    );
                }
            }
        }

        // Write final stitched image
        std::ofstream outFile = makeBMPImage(outputName, stitchedWidth, stitchedHeight);
        if (!outFile)
        {
            std::cerr << "Failed to write: " << outputName << "\n";
            return;
        }

        outFile.write(reinterpret_cast<const char*>(finalImage.data()), stitchedStride * stitchedHeight);
        outFile.close();
        std::cout << "Saved: " << outputName << ".bmp\n";
    }

private:
    static std::ofstream makeBMPImage(std::string name, const int width, const int height)
    {
        const int bytesPerPixel = 3; // 24-bit BMP
        const int rowStride = ((width * bytesPerPixel + 3) / 4) * 4; // Rows are padded to multiples of 4 bytes
        const int imageSize = rowStride * height;
        const int fileSize = 54 + imageSize;

        std::ofstream file(name + ".bmp", std::ios::binary);

        // BMP Header
        uint8_t bmpHeader[54] = {
            'B', 'M',           // Signature
            0,0,0,0,            // File size
            0,0, 0,0,           // Reserved
            54,0,0,0,           // Data offset
            40,0,0,0,           // DIB header size
            0,0,0,0,            // Width
            0,0,0,0,            // Height
            1,0,                // Planes
            24,0,               // Bits per pixel
            0,0,0,0,            // Compression (none)
            0,0,0,0,            // Image size (can be 0 for BI_RGB)
            0,0,0,0,            // X pixels per meter
            0,0,0,0,            // Y pixels per meter
            0,0,0,0,            // Total colors
            0,0,0,0             // Important colors
        };

        // File size
        bmpHeader[2] = (uint8_t)(fileSize);
        bmpHeader[3] = (uint8_t)(fileSize >> 8);
        bmpHeader[4] = (uint8_t)(fileSize >> 16);
        bmpHeader[5] = (uint8_t)(fileSize >> 24);

        // Width
        bmpHeader[18] = (uint8_t)(width);
        bmpHeader[19] = (uint8_t)(width >> 8);
        bmpHeader[20] = (uint8_t)(width >> 16);
        bmpHeader[21] = (uint8_t)(width >> 24);

        // Height
        bmpHeader[22] = (uint8_t)(height);
        bmpHeader[23] = (uint8_t)(height >> 8);
        bmpHeader[24] = (uint8_t)(height >> 16);
        bmpHeader[25] = (uint8_t)(height >> 24);

        // Image size
        bmpHeader[34] = (uint8_t)(imageSize);
        bmpHeader[35] = (uint8_t)(imageSize >> 8);
        bmpHeader[36] = (uint8_t)(imageSize >> 16);
        bmpHeader[37] = (uint8_t)(imageSize >> 24);

        file.write(reinterpret_cast<char*>(bmpHeader), 54);

        return file;
    }
};