#include <vector>
#include <chrono>
#include "ImGui/imgui.h"

struct Particle
{
    ImVec2 position;
    ImVec2 velocity;
    float life;
    float size;
    ImU32 color;
};

std::vector<Particle> particles;
const int MAX_PARTICLES = 50; 
bool enableParticles = true; 
float particleSizeMultiplier = 1.0f; 
float particleSpeedMultiplier = 1.0f; 


static auto lastTime = std::chrono::high_resolution_clock::now();

float CalculateDeltaTime()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = currentTime - lastTime;
    lastTime = currentTime;
    return elapsed.count();
}


void InitializeParticles(const ImVec2& menuSize)
{
    particles.clear(); 
    for (int i = 0; i < MAX_PARTICLES; ++i)
    {
        Particle particle;
        particle.position = ImVec2(rand() % (int)menuSize.x, rand() % (int)menuSize.y);
        particle.velocity = ImVec2(0, -(rand() % 10 / 100.0f + 0.3f)); 
        particle.life = 1.0f;
        particle.size = (rand() % 15 + 15) / 15.0f;
        particle.color = IM_COL32(255, 255, 255, 150); 
        particles.push_back(particle);
    }
}


void UpdateParticles(float deltaTime, const ImVec2& menuSize)
{
    for (int i = 0; i < particles.size(); ++i)
    {
        particles[i].position.y += particles[i].velocity.y * deltaTime * 60.0f * particleSpeedMultiplier; 

 
        if (particles[i].position.y < 0)
        {
            particles[i].position.y = menuSize.y;
            particles[i].position.x = rand() % (int)menuSize.x;
        }
    }
}


void RenderParticles(const ImVec2& menuPos, const ImVec2& menuSize)
{
    for (int i = 0; i < particles.size(); ++i)
    {
        ImVec2 adjustedPos = ImVec2(particles[i].position.x + menuPos.x, particles[i].position.y + menuPos.y);

     
        if (adjustedPos.x >= menuPos.x && adjustedPos.x <= menuPos.x + menuSize.x &&
            adjustedPos.y >= menuPos.y && adjustedPos.y <= menuPos.y + menuSize.y)
        {
            ImGui::GetWindowDrawList()->AddCircleFilled(adjustedPos, particles[i].size * particleSizeMultiplier, particles[i].color);
        }
    }
}