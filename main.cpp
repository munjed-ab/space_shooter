#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>

const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 500;

float shipX = 250.0f;
const float maxVelocity = 5.0f;
bool isFiring = false;
bool isLeftPressed = false;
bool isRightPressed = false;

struct Fireball {
    float x, y;
    Fireball(float x, float y) : x(x), y(y) {}
};

struct Rock {
    float x, y, size, velocity;
    Rock(float x, float size, float velocity) : x(x), y(500.0f), size(size), velocity(velocity) {}
};

struct Star {
    float x, y, size;
    Star(float x, float y, float size) : x(x), y(y), size(size) {}
};

std::vector<Fireball> fireballs;
std::vector<Rock> rocks;
std::vector<Star> stars;

void generateStars() {
    for (size_t i = 0; i < 500; ++i) {
        float x = static_cast<float>(rand()) / RAND_MAX * WINDOW_WIDTH;
        float y = static_cast<float>(rand()) / RAND_MAX * WINDOW_HEIGHT;
        float size = static_cast<float>(rand()) / RAND_MAX * 3 + 1;
        stars.emplace_back(x, y, size);
    }
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    srand(static_cast<unsigned int>(time(nullptr)));
    generateStars();
}

void drawShip() {
    glColor3f(0.5, 0.8, 0.9);
    glBegin(GL_POLYGON);
    glVertex2f(shipX - 20, 20);
    glVertex2f(shipX, 50);
    glVertex2f(shipX + 20, 20);
    glEnd();

    glColor3f(0.5, 0.9, 0.6);
    glBegin(GL_TRIANGLES);
    glVertex2f(shipX - 25, 20);
    glVertex2f(shipX - 20, 35);
    glVertex2f(shipX - 20, 20);
    glEnd();

    glColor3f(0.5, 0.9, 0.6);
    glBegin(GL_TRIANGLES);
    glVertex2f(shipX + 25, 20);
    glVertex2f(shipX + 20, 35);
    glVertex2f(shipX + 20, 20);
    glEnd();

    glColor3f(0.2, 0.6, 0.4);
    glBegin(GL_TRIANGLES);
    glVertex2f(shipX - 35, 25);
    glVertex2f(shipX - 10, 25);
    glVertex2f(shipX - 20, 20);
    glEnd();

    glColor3f(0.2, 0.6, 0.4);
    glBegin(GL_TRIANGLES);
    glVertex2f(shipX + 35, 25);
    glVertex2f(shipX + 10, 25);
    glVertex2f(shipX + 20, 20);
    glEnd();
}

void drawFireballs() {
    glColor3f(1.0f, 0.0f, 0.0f);
    for (const auto& fireball : fireballs) {
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; ++i) {
            float angle = i * 3.14f / 180.0f;
            float x = fireball.x + 5 * cos(angle);
            float y = fireball.y + 5 * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
    }
}

void drawStars() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    for (const auto& star : stars) {
        glVertex2f(star.x, star.y);
    }
    glEnd();
}

void drawRocks() {
    glColor3f(0.5f, 0.5f, 0.5f);
    for (const auto& rock : rocks) {
        glBegin(GL_POLYGON);
        glVertex2f(rock.x - rock.size / 2, rock.y + rock.size / 2);
        glVertex2f(rock.x + rock.size / 2, rock.y + rock.size / 2);
        glVertex2f(rock.x + rock.size / 4, rock.y - rock.size / 2);
        glVertex2f(rock.x - rock.size / 4, rock.y - rock.size / 2);
        glEnd();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    drawStars();
    drawShip();
    drawFireballs();
    drawRocks();
    glfwSwapBuffers(glfwGetCurrentContext());
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_SPACE:
            isFiring = true;
            break;
        case GLFW_KEY_LEFT:
            isLeftPressed = true;
            break;
        case GLFW_KEY_RIGHT:
            isRightPressed = true;
            break;
        }
    }
    if (action == GLFW_RELEASE) {
        switch (key) {
        case GLFW_KEY_SPACE:
            isFiring = false;
            break;
        case GLFW_KEY_LEFT:
            isLeftPressed = false;
            break;
        case GLFW_KEY_RIGHT:
            isRightPressed = false;
            break;
        }
    }
}

void updateShip() {
    if (isLeftPressed && shipX > 35) shipX -= maxVelocity;
    if (isRightPressed&& shipX < WINDOW_WIDTH-35) shipX += maxVelocity;
}

void updateFireballs() {
    for (auto& fireball : fireballs) {
        fireball.y += 20;
    }
    fireballs.erase(std::remove_if(fireballs.begin(), fireballs.end(),
        [](const Fireball& fireball) { return fireball.y > WINDOW_HEIGHT; }),
        fireballs.end());

    for (auto& fireball : fireballs) {
        for (auto& rock : rocks) {
            float dx = fireball.x - rock.x;
            float dy = fireball.y - rock.y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance <= rock.size) {
                if (rock.size == 5) {
                    rocks.erase(std::remove_if(rocks.begin(), rocks.end(),
                        [&](const Rock& r) { return &r == &rock; }),
                        rocks.end());
                }
                else {
                    rock.size--;
                }
                break;
            }
        }
    }
}

void resetGame() {
    fireballs.clear();
    rocks.clear();
    shipX = 250.0f;
}

void updateStars() {
    for (auto& star : stars) {
        star.y -= 1;
    }
    stars.erase(std::remove_if(stars.begin(), stars.end(),
        [](const Star& star) { return star.y < 0; }),
        stars.end());

    while (stars.size() < 500) {
        float x = static_cast<float>(rand()) / RAND_MAX * WINDOW_WIDTH;
        float size = static_cast<float>(rand()) / RAND_MAX * 3 + 1;
        stars.emplace_back(x, WINDOW_HEIGHT, size);
    }
}

void updateRocks() {
    for (auto& rock : rocks) {
        rock.y -= rock.velocity;

        float dx = rock.x - shipX;
        float dy = rock.y - 20;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance <= rock.size + 20) {
            resetGame();
            return;
        }
    }

    rocks.erase(std::remove_if(rocks.begin(), rocks.end(),
        [](const Rock& rock) { return rock.y < 0; }),
        rocks.end());
}

void generateRock() {
    float size = rand() % 20 + 20;
    float x = rand() % WINDOW_WIDTH;
    float velocity = rand() % 5 + 1;
    rocks.emplace_back(x, size, velocity);
}

void timer() {
    updateShip();
    updateFireballs();
    updateRocks();
    updateStars();

    if (rand() % 100 < 10) {
        generateRock();
    }
    if (isFiring) {
        fireballs.emplace_back(shipX - 20, 55);
        fireballs.emplace_back(shipX + 20, 55);
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Space Shooter", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glfwSetKeyCallback(window, keyCallback);

    init();

    while (!glfwWindowShouldClose(window)) {
        display();
        glfwPollEvents();
        timer();
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // approx 60 FPS
    }

    glfwTerminate();
    return 0;
}
