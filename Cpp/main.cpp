#include "../Header/DisplayWin32.h"
#include "../Header/Model.h"
#include "random"
#include "../Header/CharacterBall.h"
#include "../Header/Pong.h"
#include "../Header/Sphere.h"

float getRandomCoordinate(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}
bool checkOverlap(DirectX::XMFLOAT3 pos1, DirectX::XMFLOAT3 pos2) {
    float distance = sqrt(pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2) + pow(pos2.z - pos1.z, 2));
    return distance < 10.0f;
}
void PongGame()
{
    DisplayWin32 display(1000, 1000);
    Pong* pong = new Pong();
    pong->Initialize(display.GetHInstance(), display.GetWindow(), display.GetInputDevice(),nullptr);
    pong->InitializeGame();
    pong->Run();
}
void Planets()
{
    DisplayWin32 display(1280, 720);
    Game* game = new Game();
    Sphere* TheSun = new Sphere(game, 6, 50, 50, nullptr, { 0, 0, 0 }, 0, L"Models/PlanetsTextures/Sun.jpg");
    Sphere* Mercure = new Sphere(game, 2, 50, 50, TheSun, { -15, 0, 0 }, 40, L"Models/PlanetsTextures/Mercure.jpg");
    Sphere* Venus = new Sphere(game, 3, 50, 50, TheSun, { -22, 0, 0 }, 30, L"Models/PlanetsTextures/Venus.jpg");
    Sphere* Earth = new Sphere(game, 3, 50, 50, TheSun, { -29, 0, 0 }, 24, L"Models/PlanetsTextures/Earth.jpg");
    Sphere* Mars = new Sphere(game, 2, 50, 50, TheSun, { -36, 0, 0 }, 16, L"Models/PlanetsTextures/Mars.jpg");
    Sphere* Jupiter = new Sphere(game, 4, 50, 50, TheSun, { -45, 0, 0 }, 12, L"Models/PlanetsTextures/Jupiter.jpg");
    Sphere* Saturn = new Sphere(game, 4, 50, 50, TheSun, { -55,0,0 }, 8, L"Models/PlanetsTextures/Saturn.jpg");
    Sphere* Uranus = new Sphere(game, 2, 50, 50, TheSun, { -65,0,0 }, 4, L"Models/PlanetsTextures/Uranus.jpg");
    Sphere* Neptune = new Sphere(game, 2, 50, 50, TheSun, { -70,0,0 }, 3, L"Models/PlanetsTextures/Neptune.jpg");
    Sphere* Pluto = new Sphere(game, 1, 50, 50, TheSun, { -75,0,0 }, 1, L"Models/PlanetsTextures/Pluto.jpg");

    Sphere* Moon = new Sphere(game, 1, 50, 50, Earth, { -34,0,0 }, 100, L"Models/PlanetsTextures/TheMoon.jpg");
    Sphere* Fobos = new Sphere(game, 1, 50, 50, Mars, { -40,0,0 }, 45, L"Models/PlanetsTextures/TheMoon.jpg");
    Sphere* Deymos = new Sphere(game, 1, 50, 50, Mars, { -43,0,0 }, 40, L"Models/PlanetsTextures/TheMoon.jpg");

    game->PushGameComponents(TheSun);
    game->PushGameComponents(Mercure);
    game->PushGameComponents(Venus);
    game->PushGameComponents(Earth);
    game->PushGameComponents(Mars);
    game->PushGameComponents(Jupiter);
    game->PushGameComponents(Saturn);
    game->PushGameComponents(Uranus);
    game->PushGameComponents(Neptune);
    game->PushGameComponents(Moon);
    game->PushGameComponents(Fobos);
    game->PushGameComponents(Deymos);
    game->PushGameComponents(Pluto);

    game->Initialize(display.GetHInstance(), display.GetWindow(), display.GetInputDevice(), nullptr);
    game->Run();
}
void Katamari()
{
    DisplayWin32 display(1280, 720);
    Game* game = new Game();

    for (int i = 0; i < 5; ++i) {
        DirectX::XMFLOAT3 BigelPos(getRandomCoordinate(-50.0f, 50.0f), 0, getRandomCoordinate(-50.0f, 50.0f));
        DirectX::XMFLOAT3 SpritzPos(getRandomCoordinate(-50.0f, 50.0f), 0, getRandomCoordinate(-50.0f, 50.0f));

        while (checkOverlap(BigelPos, SpritzPos)) {
            SpritzPos.x += getRandomCoordinate(-5.0f, 5.0f);
            SpritzPos.z += getRandomCoordinate(-5.0f, 5.0f);
        }

        Model* Bigel = new Model(game, "Models/Beagle/13041_Beagle_v1_L1.obj", BigelPos, DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f), DirectX::XMFLOAT3(-80.0f, -90.0f, 0.0f));
        Model* Spritz = new Model(game, "Models/Spritz/Pomeranian-bl.obj", SpritzPos, DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));

        game->PushGameComponents(Bigel);
        game->PushGameComponents(Spritz);
        game->PushCollisions(Bigel);
        game->PushCollisions(Spritz);
    }
    Model* Road = new Model(game, "Models/Road/detailed_road_03_obj.obj", DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.05f, 0.05f, 0.05f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
    game->PushGameComponents(Road);

    game->Initialize(display.GetHInstance(), display.GetWindow(), display.GetInputDevice(), new CharacterBall(game, 2, 100, 100, L"Models/PlanetsTextures/Pluto.jpg", DirectX::XMFLOAT3(0, 2, 0), DirectX::XMFLOAT3(1, 1, 1), DirectX::XMFLOAT3(0, 0, 0)));
    game->Run();
}

int main()
{
    Katamari();
}


