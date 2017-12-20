#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include <vector>

using namespace std;

class Renderer
{
    private:

    const float circleShapeSize = 6.f;
    
    sf::RenderWindow & window;
    vector<sf::CircleShape> points;
    sf::CircleShape externPoint;

    sf::Vector2i getMousePosition();

    void init_polygon(sf::ConvexShape& polygon);
    void init_triang(sf::ConvexShape& polygon);
    void handleClick();
    void handleRightClick();
    void clear();
    void update();

    public:

    Renderer(sf::RenderWindow & _window);
    void startRenderWindow();
};

#endif