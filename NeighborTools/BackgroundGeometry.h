#pragma once
#include "includes.h"
#include "globals.h"
#include <vector>




const int NUM_POINTS = 250;
const float MAX_DISTANCE = 80.0f;
const float WINDOW_WIDTH = 800.0f;
const float WINDOW_HEIGHT = 600.0f;

struct Point {
    ImVec2 pos;
    ImVec2 velocity;
};

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

std::vector<Point> initializePoints(int numPoints, float width, float height) {
    std::vector<Point> points;
    for (int i = 0; i < numPoints; ++i) {
        Point p;
        p.pos = ImVec2(randomFloat(0, width), randomFloat(0, height));
        p.velocity = ImVec2(randomFloat(-0.5f, 0.5f), randomFloat(-0.5f, 0.5f));
        points.push_back(p);
    }
    return points;
}

void updatePoints(std::vector<Point>& points, float width, float height) {
    for (auto& p : points) {
        p.pos.x += p.velocity.x;
        p.pos.y += p.velocity.y;

        if (p.pos.x <= 0 || p.pos.x >= width) p.velocity.x *= -1;
        if (p.pos.y <= 0 || p.pos.y >= height) p.velocity.y *= -1;
    }
}

ImU32 getRGBColor(float speed = 0.1f) {
    float time = ImGui::GetTime() * speed;
    int r = static_cast<int>((sin(time) * 127.5f + 127.5f));
    int g = static_cast<int>((sin(time + 2.0f) * 127.5f + 127.5f));
    int b = static_cast<int>((sin(time + 4.0f) * 127.5f + 127.5f));
    return IM_COL32(r, g, b, 255);
}

void drawNetwork(ImDrawList* drawList, const std::vector<Point>& points, ImU32 color, float pointSize, float lineThickness) {
    for (const auto& p : points) {
        drawList->AddCircleFilled(p.pos, pointSize, color); // Punkte
    }

    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            float distance = sqrtf(pow(points[i].pos.x - points[j].pos.x, 2) + pow(points[i].pos.y - points[j].pos.y, 2));
            if (distance < MAX_DISTANCE) {
                int alpha = static_cast<int>(255 * (1.0f - distance / MAX_DISTANCE));
                drawList->AddLine(points[i].pos, points[j].pos, IM_COL32(color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF, alpha), lineThickness);
            }
        }
    }
}