#include "Renderer.h"
#include <SFML/Graphics.hpp>

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 720;

int main() {
    sf::ContextSettings contextSettings;
    contextSettings.antialiasingLevel = 0;
    sf::VideoMode videoMode(WIDTH, HEIGHT);
    sf::RenderWindow window(videoMode, "Proiect GC", 
        sf::Style::Default, contextSettings);
    Renderer renderer(window);
    renderer.startRenderWindow();
}