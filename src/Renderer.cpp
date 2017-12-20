
#include "Renderer.h"
#include "Logic.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>

using namespace std;

sf::Vector2i Renderer :: getMousePosition() {
    return sf::Mouse::getPosition(window);
}

void Renderer :: init_polygon(sf::ConvexShape& polygon) {
    polygon.setFillColor(sf::Color::Transparent);
    polygon.setOutlineColor(sf::Color::Red);
    polygon.setOutlineThickness(3.0f);
    polygon.setPointCount(0);
    externPoint.setFillColor(sf::Color::Magenta);
}
void Renderer :: init_triang(sf::ConvexShape& polygon) {
    polygon.setFillColor(sf::Color(100, 100, 100));
    polygon.setOutlineColor(sf::Color::Transparent);
    polygon.setOutlineThickness(3.0f);
    polygon.setPointCount(0);
}
void Renderer :: handleClick() {
    auto position = getMousePosition();
    if (position.x <= 0
    || position.y <= 0
    || position.x >= window.getSize().x
    || position.y >= window.getSize().y) {
        return;
    }
    sf::CircleShape point(Renderer::circleShapeSize);
    point.setPosition(
        position.x - Renderer::circleShapeSize,
        position.y - Renderer::circleShapeSize);
    point.setFillColor(sf::Color::Red);
    points.push_back(point);
}
void Renderer :: handleRightClick() {
    auto position = getMousePosition();
    externPoint.setPosition(sf::Vector2f(
        position.x - 1.5 * Renderer::circleShapeSize,
        position.y - 1.5 * Renderer::circleShapeSize
    ));
}
void Renderer :: clear() {
    points.clear();
}
void Renderer :: update() {
    window.clear();

    sf::ConvexShape polygon;
    init_polygon(polygon);
    polygon.setPointCount(points.size());
    int index = 0;

    for (auto point : points) {
        auto position = point.getPosition();
        polygon.setPoint(index, {
            position.x + Renderer::circleShapeSize,
            position.y + Renderer::circleShapeSize});
        index ++;
    }

    window.draw(polygon);

    for (auto point : points) {
        window.draw(point);
    }

    vector<Point> discrete_polygon;
    for (auto p : points) {
        discrete_polygon.push_back(
            { p.getPosition().x, p.getPosition().y }
        );
    }

    Logic tester1(discrete_polygon);
    Logic tester2(discrete_polygon);
    vector<Triangle> solution;
    if(discrete_polygon.size() > 2) {
        try {
            solution = tester1.get_triangulation();
        } catch (int w) { }
    }

    auto drawLine = [this](Point a, Point b, int col = 1) {
        sf::Vertex line[] = {
            sf::Vertex({ 
                (float) a.x + Renderer::circleShapeSize,
                (float) a.y + Renderer::circleShapeSize }),
            sf::Vertex({
                (float) b.x + Renderer::circleShapeSize,
                (float) b.y + Renderer::circleShapeSize })
        };
        if(col == 1) {
            line[0].color  = sf::Color::Green;
            line[1].color  = sf::Color::Green;
        } else {
            line[0].color  = sf::Color::Yellow;
            line[1].color  = sf::Color::Yellow;
        }
        window.draw(line, 2, sf::Lines);
    };

    for (auto it : solution) {
        drawLine(it.A, it.B);
        drawLine(it.B, it.C);
        drawLine(it.C, it.A);
    }
    
    Point extern_pos = {
        externPoint.getPosition().x + 1.5 * Renderer::circleShapeSize,
        externPoint.getPosition().y + 1.5 * Renderer::circleShapeSize
    };

    int response = -1;
    if(discrete_polygon.size() > 2) {
        try {
            response = tester2.is_point_inside(extern_pos);
        } catch (int w) { }
    }

    if (response == -1) {
        externPoint.setFillColor(sf::Color::Magenta);
    } else {
        externPoint.setFillColor(sf::Color::Green);
        sf::ConvexShape triang;
        init_triang(triang);
        triang.setPointCount(3);
        Triangle t = solution[response];
        triang.setPoint(0, {
            (float) t.A.x + Renderer::circleShapeSize,
            (float) t.A.y + Renderer::circleShapeSize});
        triang.setPoint(1, {
            (float) t.B.x + Renderer::circleShapeSize,
            (float) t.B.y + Renderer::circleShapeSize});
        triang.setPoint(2, {
            (float) t.C.x + Renderer::circleShapeSize,
            (float) t.C.y + Renderer::circleShapeSize});
        window.draw(triang);
    }

    if(discrete_polygon.size() > 2) {
        auto pos_edge = tester2.check_for_edges(extern_pos);
        for(int i=1; i <= 20; i++) {
            double dx = double(rand() % 10);
            double dy = double(rand() % 10);
            drawLine(
                {pos_edge.first.x + dx, pos_edge.first.y + dy},
                {pos_edge.second.x + dx, pos_edge.second.y + dy},
            2);
        }
    }

    window.draw(externPoint);
    window.display();
}

Renderer :: Renderer(sf::RenderWindow & _window) :
    window(_window),
    externPoint(1.5*Renderer::circleShapeSize) { }

void Renderer :: startRenderWindow() {
    bool painterRunning = true;
    thread painter ([this, &painterRunning]() {
        while (painterRunning) {
            update();
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    });
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                painterRunning = false;
                if(painter.joinable()) {
                    painter.join();
                }
                window.close();
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                auto t1 = std::chrono::high_resolution_clock::now();
                handleClick();
                auto t2 = std::chrono::high_resolution_clock::now();
                auto duration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
                this_thread::sleep_for(chrono::milliseconds(200));
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                handleRightClick();
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
            {
                clear();
            }
        }
    }
    painterRunning = false;
    if(painter.joinable()) {
        painter.join();
    }
}