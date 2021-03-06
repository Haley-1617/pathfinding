
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// In order to load the resources like cute_image.png, you have to set up
// your target scheme:
//
// - Select "Edit Scheme…" in the "Product" menu;
// - Check the box "use custom working directory";
// - Fill the text field with the folder path containing your resources;
//        (e.g. your project folder)
// - Click OK.
//

#include <iostream>
#include <queue>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "Window.h"


struct route {
    sf::Vector2i position;
    route *parent;
    route(int x, int y) : position(x, y), parent(NULL){}
    route() : position(0, 0), parent(NULL){}
};

typedef std::vector<sf::RectangleShape> vectorOfRec;

class FindPath {
private:
    std::vector<route*> visited;
    std::deque<route*> frontier;
    int gridSize;
    enum Direction {UP, RIGHT, DOWN, LEFT};
    sf::Vector2i m_start;
    sf::Vector2i m_target;
    int lengthPath;
    bool pathIsFound;
    
public:
    FindPath(const int &gridSize);
    ~FindPath();
    void Update(vectorOfRec m_obstacle);
    void pathfinding(vectorOfRec m_obstacle);
    bool findPath() {return pathIsFound;}
    bool find(sf::Vector2i cur, vectorOfRec m_obstacle);
    void render(sf::RenderWindow &window);
    bool visitIsEmpty() {return visited.empty();}
    void setStart(sf::Vector2i newStart) {m_start = newStart;}
    void setTarget(sf::Vector2i newTarget) {m_target = newTarget;}
};

FindPath::FindPath(const int &gridSize) {
    this->gridSize = gridSize;
    m_start = sf::Vector2i(-1, -1);
    m_target = sf::Vector2i(-1, -1);
    route startPoint(m_start.x, m_start.y);
    lengthPath = 0;
    pathIsFound = false;
}

FindPath::~FindPath() {
    std::vector<route*>::iterator iterV;
    for (iterV = visited.begin(); iterV != visited.end(); ++iterV)
        delete *iterV;
    visited.clear();
    frontier.clear();
}

void FindPath::Update(vectorOfRec m_obstacle) {
//    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
    pathfinding(m_obstacle);
//    call render
}

void FindPath::pathfinding(vectorOfRec m_obstacle) {
    if (visited.empty()) {
        route *start = new route(m_start.x, m_start.y);
        frontier.push_back(start);
    }
    
    visited.push_back(frontier.front());
    if (visited.back()->position.x == m_target.x &&
        visited.back()->position.y == m_target.y) {
        pathIsFound = true;
        return;
    }
    for (int d = UP; d <= LEFT; d++) {
        route *cur = new route(0, 0);
        switch(d) {
            case UP:
                cur->position.x = frontier.front()->position.x;
                cur->position.y = frontier.front()->position.y - gridSize;
                break;
            case RIGHT:
                cur->position.x = frontier.front()->position.x + gridSize;
                cur->position.y = frontier.front()->position.y;
                break;
            case DOWN:
                cur->position.x = frontier.front()->position.x;
                cur->position.y = frontier.front()->position.y + gridSize;
                break;
            case LEFT:
                cur->position.x = frontier.front()->position.x - gridSize;
                cur->position.y = frontier.front()->position.y;
                break;
        }
        
//        check if out of range
        if (cur->position.x < 0 || cur->position.x >= 1200 ||
            cur->position.y < 0 || cur->position.y >= 1200) {
            delete cur;
            continue;
        }
//        check if the node has been visited
//        or the node is in the frontier already
        else if (find(cur->position, m_obstacle)) {
            delete cur;
            continue;
        }
        else {
            cur->parent = visited.back();
            frontier.push_back(cur);
        }
    }
    frontier.pop_front();
}

bool FindPath::find(sf::Vector2i cur, vectorOfRec m_obstacle) {
//    check if the node has been visited
    std::vector<route*>::iterator iterV;
    for (iterV = visited.begin(); iterV != visited.end(); iterV++)
        if ((*iterV)->position.x == cur.x && (*iterV)->position.y == cur.y) return true;
//    check if the nodes is in frontier
    std::deque<route*>::iterator iterF;
    for (iterF = frontier.begin(); iterF != frontier.end(); iterF++)
        if ((*iterF)->position.x == cur.x && (*iterF)->position.y == cur.y)
            return true;
//    check if the node is obstacle
    vectorOfRec::iterator iterOb;
    for (iterOb = m_obstacle.begin(); iterOb != m_obstacle.end(); iterOb++) {
        if (iterOb->getPosition().x == cur.x &&
            iterOb->getPosition().y == cur.y) return true;
    }
    return false;
}

void FindPath::render(sf::RenderWindow &window) {
//    draw visited node
    std::vector<route*>::iterator iterV;
    sf::RectangleShape temp;
    temp.setSize(sf::Vector2f(gridSize, gridSize));
    temp.setFillColor(sf::Color(187, 236, 237));
    for (iterV = visited.begin(); iterV != visited.end(); iterV++) {
        if (((*iterV)->position.x == m_start.x && (*iterV)->position.y == m_start.y) ||
            ((*iterV)->position.x == m_target.x && (*iterV)->position.y == m_target.y)) continue;
        temp.setPosition((*iterV)->position.x, (*iterV)->position.y);
        window.draw(temp);
    }
//    draw frontier
    std::deque<route*>::iterator iterF;
    temp.setFillColor(sf::Color(175, 247, 162));
    for (iterF = frontier.begin(); iterF != frontier.end(); iterF++) {
        if (((*iterF)->position.x == m_start.x && (*iterF)->position.y == m_start.y) ||
        ((*iterF)->position.x == m_target.x && (*iterF)->position.y == m_target.y)) continue;
        temp.setPosition((*iterF)->position.x, (*iterF)->position.y);
        window.draw(temp);
    }
//    draw path if found
    if (pathIsFound) {
        route *temp = visited.back();
        route *parent = temp->parent;
        sf::RectangleShape line;
        std::string drawFrom = "";
        while (parent) {
//            vertical direction
            if (temp->position.x - parent->position.x == 0) {
                line.setSize(sf::Vector2f(5, abs(temp->position.y - parent->position.y)));
                if (parent->position.y < temp->position.y) drawFrom = "parent";
                else drawFrom = "temp";
            }
//            horizontal direction
            else {
                line.setSize(sf::Vector2f(abs(temp->position.x - parent->position.x), 5));
                if (parent->position.x < temp->position.x) drawFrom = "parent";
                else drawFrom = "temp";
            }
            if (drawFrom == "parent")
                line.setPosition(parent->position.x + gridSize / 2, parent->position.y + gridSize / 2);
            else line.setPosition(temp->position.x + gridSize / 2, temp->position.y + gridSize / 2);
            line.setFillColor(sf::Color::Yellow);
            window.draw(line);
            temp = parent;
            parent = parent->parent;
        }
        
    }
}

class Game {
private:
    Window window;
    FindPath m_findPath;
    vectorOfRec obstacle;
    int gridSize;
    sf::RectangleShape start;
    sf::RectangleShape target;
    bool mousePressed;
    bool startClick;
    bool targetClick;
    bool setObstacle;
    bool removeObstacle;
    bool findingPath;
    void mouseEvent();
    void obstacleEvent();
    sf::Clock m_clock;
    float m_elapsed;
    
public:
    Game();
    Game(sf::Vector2u windowSize);
    ~Game(){};
    void Update();
    void handleInput();
    void render();
    void drawGridLine();
    int getGridSize() {return gridSize;}
    bool windowClosed() {return window.getCloseWindow();}
    sf::Vector2i getstartPos() {return sf::Vector2i(start.getPosition().x,
                                                 start.getPosition().y);}
    sf::Vector2i gettargetPos() {return sf::Vector2i(target.getPosition().x,
                                                  target.getPosition().y);}
    void restartClock() {m_elapsed += m_clock.restart().asSeconds();}
};

void Game::obstacleEvent() {
    if (obstacle.empty()) setObstacle = true;
    else {
        sf::Vector2i mousePos = sf::Mouse::getPosition(*window.getWindow());
        std::vector<sf::RectangleShape>::iterator iter;
        for (iter = obstacle.begin(); iter != obstacle.end(); iter++)
            if (iter->getPosition().x == mousePos.x / gridSize * gridSize &&
                iter->getPosition().y == mousePos.y / gridSize * gridSize) {
                removeObstacle = true;
                return;
            }
        setObstacle = true;
    }
}

Game::Game() : window("pathfinding", sf::Vector2u(1200, 1200)), m_findPath(40) {
    gridSize = 40;
    start.setPosition(window.getWindowSize().x / 2 - gridSize, window.getWindowSize().y / 2);
    start.setSize(sf::Vector2f(gridSize, gridSize));
    start.setFillColor(sf::Color(101, 216, 65));
    target.setPosition(window.getWindowSize().x / 2 + gridSize, window.getWindowSize().y / 2);
    target.setSize(sf::Vector2f(gridSize, gridSize));
    target.setFillColor(sf::Color(221, 81, 37));
    mousePressed = false;
    startClick = false;
    targetClick = false;
    setObstacle = false;
    removeObstacle = false;
    findingPath = false;
    m_clock.restart();
    m_elapsed = 0.0f;
}

void Game::Update() {
    window.clearDraw();
    handleInput();
    mouseEvent();
    render();
    window.display();
}

void Game::handleInput() {
    sf::Event e;
    window.clearDraw();
    while(window.getWindow()->pollEvent(e)) {
        if (e.type == sf::Event::Closed) window.setCloseWindow();
        if (e.type == sf::Event::MouseButtonPressed) {
            mousePressed = true;
            if (sf::Mouse::getPosition(*window.getWindow()).x / gridSize * gridSize ==
                start.getPosition().x &&
                sf::Mouse::getPosition(*window.getWindow()).y / gridSize * gridSize ==
                start.getPosition().y) startClick = true;
            else if (sf::Mouse::getPosition(*window.getWindow()).x / gridSize * gridSize ==
            target.getPosition().x &&
            sf::Mouse::getPosition(*window.getWindow()).y / gridSize * gridSize ==
            target.getPosition().y) targetClick = true;
            else obstacleEvent();
        }
        if (e.type == sf::Event::MouseButtonReleased) {
            mousePressed = false;
            startClick = false;
            targetClick = false;
            if (setObstacle) setObstacle = false;
            else removeObstacle = false;
        }
        if (e.type == sf::Event::KeyPressed)
            if (e.key.code == sf::Keyboard::Enter) findingPath = true;
    }
}

void Game::mouseEvent() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(*window.getWindow());
    float timestamp = 1.0 / 60;
    
//    dragging start point
    if (mousePressed && startClick) start.setPosition(mousePos.x / gridSize * gridSize,
                                                      mousePos.y / gridSize * gridSize);
//    dragging target point
    else if (mousePressed && targetClick) target.setPosition(mousePos.x / gridSize * gridSize,
                                                             mousePos.y / gridSize * gridSize);
    
//    set obstacle
    else if (mousePressed && setObstacle) {
        if (!obstacle.empty() &&
            obstacle.back().getPosition().x == mousePos.x / gridSize * gridSize &&
            obstacle.back().getPosition().y == mousePos.y / gridSize * gridSize) return;
//        out of window range
        else if (mousePos.x < 0 || mousePos.x > window.getWindowSize().x ||
                 mousePos.y < 0 || mousePos.y > window.getWindowSize().y) return;
        sf::RectangleShape wall;
        wall.setSize(sf::Vector2f(gridSize, gridSize));
        wall.setFillColor(sf::Color(128, 128, 128));
        wall.setPosition(mousePos.x / gridSize * gridSize, mousePos.y / gridSize * gridSize);
        obstacle.push_back(wall);
    }
    
//    remove obstacle
    else if (mousePressed && removeObstacle) {
        std::vector<sf::RectangleShape>::iterator iter;
        for (iter = obstacle.begin(); iter != obstacle.end(); iter++) {
            if (iter->getPosition().x == mousePos.x / gridSize * gridSize &&
                iter->getPosition().y == mousePos.y / gridSize * gridSize) {
                obstacle.erase(iter);
                return;
            }
        }
    }
    
    else if (findingPath) {
        if (m_findPath.visitIsEmpty()) {
            m_findPath.setStart(sf::Vector2i(start.getPosition().x,
                                             start.getPosition().y));
            m_findPath.setTarget(sf::Vector2i(target.getPosition().x,
                                              target.getPosition().y));
        }
//        if (m_elapsed >= timestamp) {
            m_findPath.Update(obstacle);
            m_elapsed -= timestamp;
            if (m_findPath.findPath()) findingPath = false;
//        }
    }
}

void Game::render() {
    if (!obstacle.empty()) {
        std::vector<sf::RectangleShape>::iterator iter;
        for (iter = obstacle.begin(); iter != obstacle.end(); iter++)
            window.draw(*iter);
    }
    window.draw(start);
    window.draw(target);
    m_findPath.render(*window.getWindow());
    drawGridLine();
}

void Game::drawGridLine() {
    for (int y = 1; y <= window.getWindowSize().y / gridSize; y++) {
        sf::VertexArray line(sf::Lines, 2);
        line[0].position = sf::Vector2f(0, y * gridSize);
        line[0].color = sf::Color(168, 168, 168);
        line[1].position = sf::Vector2f(window.getWindowSize().x, y * gridSize);
        line[1].color = sf::Color(168, 168, 168);
        window.draw(line);
    }
    for (int x = 1; x <= window.getWindowSize().x / gridSize; x++) {
        sf::VertexArray line(sf::Lines, 2);
        line[0].position = sf::Vector2f(x * gridSize, 0);
        line[0].color = sf::Color(168, 168, 168);
        line[1].position = sf::Vector2f(x * gridSize, window.getWindowSize().y);
        line[1].color = sf::Color(168, 168, 168);
        window.draw(line);
    }
}



int main(int argc, char const** argv) {
//    Window window("Defualt", sf::Vector2u(1200, 1200));
    Game game;
    while (!game.windowClosed()) {
        game.Update();
        game.restartClock();
    }
    return EXIT_SUCCESS;
}
