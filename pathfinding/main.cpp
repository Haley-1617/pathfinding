
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
    Window() {setup("Default", sf::Vector2u(600, 800));}
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

class Grid {
private:
    Window window;
    sf::RectangleShape grid[120][120];
    std::vector<sf::RectangleShape> obstacle;
    int gridRow;
    int gridCol;
    int gridSize;
    sf::RectangleShape start;
    sf::RectangleShape target;
    bool mousePressed;
    bool startClick;
    bool targetClick;
    bool setObstacle;
    bool removeObstacle;
    void obstacleEvent();
    
public:
    Grid();
    Grid(sf::Vector2u windowSize);
    ~Grid(){};
    void Update();
    void handleInput();
    void findingpath();
    void mouseEvent();
    void render();
    void drawGridLine();
    bool closeWindow(){return window.getCloseWindow();}
};

void Grid::obstacleEvent() {
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

Grid::Grid() : window("pathfinding", sf::Vector2u(1200, 1200)){
    gridRow = 30;
    gridCol = gridRow;
    gridSize = 40;
    memset((void*)grid, 0, sizeof(int) * gridRow * gridCol);
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
}

void Grid::Update() {
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
    }
    mouseEvent();
    drawGridLine();
    render();
    window.display();
}

void Grid::mouseEvent() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(*window.getWindow());
    if (mousePressed && startClick) start.setPosition(mousePos.x / gridSize * gridSize,
                                                      mousePos.y / gridSize * gridSize);
    else if (mousePressed && targetClick) target.setPosition(mousePos.x / gridSize * gridSize,
                                                             mousePos.y / gridSize * gridSize);
    else if (mousePressed && setObstacle) {
        if (!obstacle.empty() &&
            obstacle.back().getPosition().x == mousePos.x / gridSize * gridSize &&
            obstacle.back().getPosition().y == mousePos.y / gridSize * gridSize) return;
        sf::RectangleShape wall;
        wall.setSize(sf::Vector2f(gridSize, gridSize));
        wall.setFillColor(sf::Color(128, 128, 128));
        wall.setPosition(mousePos.x / gridSize * gridSize, mousePos.y / gridSize * gridSize);
        obstacle.push_back(wall);
        std::cout << "num of obstacles: " << obstacle.size() << std::endl;
    }
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
}

void Grid::render() {
    if (!obstacle.empty()) {
        std::vector<sf::RectangleShape>::iterator iter;
        for (iter = obstacle.begin(); iter != obstacle.end(); iter++)
            window.draw(*iter);
    }
    window.draw(start);
    window.draw(target);
}

void Grid::drawGridLine() {
    for (int y = 1; y <= gridRow; y++) {
        sf::VertexArray line(sf::Lines, 2);
        line[0].position = sf::Vector2f(0, y * gridSize);
        line[0].color = sf::Color(168, 168, 168);
        line[1].position = sf::Vector2f(window.getWindowSize().x, y * gridSize);
        line[1].color = sf::Color(168, 168, 168);
        window.draw(line);
    }
    for (int x = 1; x <= gridCol; x++) {
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
    Grid grid;
    while (!grid.closeWindow()) {
        grid.Update();
    }
    return EXIT_SUCCESS;
}
