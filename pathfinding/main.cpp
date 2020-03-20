
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

class Window {
private:
    sf::RenderWindow window;
    sf::Vector2u windowSize;
    std::string title;
    bool closeWindow;
    bool fullScreen;
    void clear() {window.close();}
    void setup(const std::string &title, const sf::Vector2u &size);
    void create();
    
public:
//    Window() {setup("Default", sf::Vector2u(600, 800));}
    Window(std::string title, sf::Vector2u windowSize) {setup(title, windowSize);}
    ~Window() {clear();}
    void clearDraw() {window.clear(sf::Color::White);}
    void draw(sf::Drawable &drawable) {window.draw(drawable);}
    void display() {window.display();}
    void Update();
    void setCloseWindow() {closeWindow = true;}
    bool getCloseWindow() {return closeWindow;}
    bool getFullScreen() {return fullScreen;}
    sf::Vector2u getWindowSize() {return windowSize;}
    sf::RenderWindow* getWindow() {return &window;}
    void toggleFullScreen();
};

void Window::setup(const std::string &title, const sf::Vector2u &size) {
    this->title = title;
    this->windowSize = size;
    closeWindow = false;
    fullScreen = false;
    create();
}

void Window::create() {
    auto style = (fullScreen ? sf::Style::Fullscreen : sf::Style::Default);
    window.create({windowSize.x, windowSize.y, 32}, title, style);
}

void Window::Update() {
    sf::Event e;
    while(window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) closeWindow = true;
    }
}

void Window::toggleFullScreen() {
    fullScreen = !fullScreen;
    clear();
    create();
}

struct route {
    sf::Vector2i position;
    route *parent;
    route(int x, int y) : position(x, y), parent(NULL){}
    route() : position(0, 0), parent(NULL){}
};

class FindPath {
private:
    std::vector<route> visited;
    std::deque<route> frontier;
    int gridSize;
    enum Direction {UP, RIGHT, DOWN, LEFT};
    sf::Vector2i m_start;
    sf::Vector2i m_target;
    int lengthPath;
    
public:
    FindPath(const int &gridSize);
    ~FindPath(){};
    void Update();
    void pathfinding();
    bool findPath() {return frontier.empty();}
    bool find(const sf::Vector2i &cur);
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
}

void FindPath::Update() {
//    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
    pathfinding();
//    call render
}

void FindPath::pathfinding() {
    if (visited.empty()) {
        route start(m_start.x, m_start.y);
        frontier.push_back(start);
    }
    visited.push_back(frontier.front());
    if (visited.back().position.x == m_target.x &&
        visited.back().position.y == m_target.y) {
        frontier.clear();
        return;
    }
    int curIndex = visited.size() - 1;
    std::cout << "curIndex: " << curIndex << std::endl;
    for (int d = UP; d <= LEFT; d++) {
        route cur;
        switch(d) {
            case UP:
                cur.position.x = frontier.front().position.x;
                cur.position.y = frontier.front().position.y - gridSize;
                break;
            case RIGHT:
                cur.position.x = frontier.front().position.x + gridSize;
                cur.position.y = frontier.front().position.y;
                break;
            case DOWN:
                cur.position.x = frontier.front().position.x;
                cur.position.y = frontier.front().position.y + gridSize;
                break;
            case LEFT:
                cur.position.x = frontier.front().position.x - gridSize;
                cur.position.y = frontier.front().position.y;
                break;
        }
        std::cout << "curX: " << cur.position.x << std::endl;
        std::cout << "curY: " << cur.position.y << std::endl;
        
//        check if out of range
        if (cur.position.x < 0 || cur.position.x > 1200 ||
            cur.position.y < 0 || cur.position.y > 1200) continue;
//        check if the node has been visited
//        or the node is in the frontier already
        else if(!find(cur.position)) {
            cur.parent = &visited[curIndex];
            frontier.push_back(cur);
        }
    }
    frontier.pop_front();
}

bool FindPath::find(const sf::Vector2i &cur) {
    std::vector<route>::iterator iterV;
    for (iterV = visited.begin(); iterV != visited.end(); iterV++)
        if (iterV->position.x == cur.x && iterV->position.y == cur.y) return true;
    std::deque<route>::iterator iterF;
    for (iterF = frontier.begin(); iterF != frontier.end(); iterF++)
        if (iterF->position.x == cur.x && iterF->position.y == cur.y)
            return true;
    return false;
}

void FindPath::render(sf::RenderWindow &window) {
    std::vector<route>::iterator iterV;
    sf::RectangleShape temp;
    temp.setSize(sf::Vector2f(gridSize, gridSize));
    temp.setFillColor(sf::Color(187, 236, 237));
    for (iterV = visited.begin(); iterV != visited.end(); iterV++) {
        if ((iterV->position.x == m_start.x && iterV->position.y == m_start.y) ||
            (iterV->position.x == m_target.x && iterV->position.y == m_target.y)) continue;
        temp.setPosition(iterV->position.x, iterV->position.y);
        window.draw(temp);
    }
    std::deque<route>::iterator iterF;
    temp.setFillColor(sf::Color(175, 247, 162));
    for (iterF = frontier.begin(); iterF != frontier.end(); iterF++) {
        if ((iterF->position.x == m_start.x && iterF->position.y == m_start.y) ||
        (iterF->position.x == m_target.x && iterF->position.y == m_target.y)) continue;
        temp.setPosition(iterF->position.x, iterF->position.y);
        window.draw(temp);
    }
}

class Game {
private:
    Window window;
    FindPath m_findPath;
    std::vector<sf::RectangleShape> obstacle;
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
//        if (e.type == sf::Event::KeyReleased)
//            findingPath = false;
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
            m_findPath.Update();
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
//    Window window;
    while (!game.windowClosed()) {
        game.Update();
        game.restartClock();
    }
    return EXIT_SUCCESS;
}
