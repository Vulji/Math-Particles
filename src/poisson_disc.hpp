// poisson_disc.hpp
#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <cmath>
#include "utils.hpp"
#include <glm/gtx/norm.hpp> 

namespace PoissonDisc
{

static bool isValid(const glm::vec2& candidate,
                    const glm::vec2& regionSize,
                    float            cellSize,
                    float            radius,
                    const std::vector<glm::vec2>& points,
                    const std::vector<int>&       grid,
                    int              gridWidth,
                    int              gridHeight)
{
    if (candidate.x < 0 || candidate.x >= regionSize.x ||
        candidate.y < 0 || candidate.y >= regionSize.y)
        return false;

    int cellX = int(candidate.x / cellSize);
    int cellY = int(candidate.y / cellSize);

    int startX = std::max(0, cellX - 2);
    int endX   = std::min(gridWidth  - 1, cellX + 2);
    int startY = std::max(0, cellY - 2);
    int endY   = std::min(gridHeight - 1, cellY + 2);

    for (int x = startX; x <= endX; ++x)
        for (int y = startY; y <= endY; ++y)
        {
            int idx = grid[x + y*gridWidth];
            if (idx != -1)
            {
                float d2 = glm::length2(candidate - points[idx]);
                if (d2 < radius * radius)
                    return false;
            }
        }

    return true;
}


static std::vector<glm::vec2> GeneratePoints(float radius,
                                             const glm::vec2& regionSize,
                                             int numSamplesBeforeRejection = 30)
{
    float cellSize = radius / std::sqrt(2.f);

    int gridWidth  = int(std::ceil(regionSize.x / cellSize));
    int gridHeight = int(std::ceil(regionSize.y / cellSize));

    std::vector<int> grid(gridWidth * gridHeight, -1);

    std::vector<glm::vec2> points;
    points.reserve(64);

    std::vector<glm::vec2> spawnPoints;
    spawnPoints.reserve(64);
    spawnPoints.push_back(regionSize * 0.5f);

    auto getGridIndex = [&](int gx, int gy){ 
        return gx + gy * gridWidth; 
    };

    while (!spawnPoints.empty())
    {
        int spawnIndex = int(utils::rand(0.f, float(spawnPoints.size())));
        glm::vec2 spawnCenter = spawnPoints[spawnIndex];
        bool accepted = false;


        for (int i = 0; i < numSamplesBeforeRejection; ++i)
        {
            float angle = utils::rand(0.f, 2.f * glm::pi<float>());
            glm::vec2 dir(std::cos(angle), std::sin(angle));

            float mag = utils::rand(radius, 2.f * radius);
            glm::vec2 candidate = spawnCenter + dir * mag;

            if (isValid(candidate, regionSize, cellSize, radius,
                        points, grid, gridWidth, gridHeight))
            {
                
                points.push_back(candidate);
                spawnPoints.push_back(candidate);
                int gx = int(candidate.x / cellSize);
                int gy = int(candidate.y / cellSize);
                grid[getGridIndex(gx, gy)] = int(points.size()) - 1;
                accepted = true;
                break;
            }
        }

        
        if (!accepted)
            spawnPoints.erase(spawnPoints.begin() + spawnIndex);
    }

    return points;
}

}
