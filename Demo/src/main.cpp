#include <string>
#include "../dependente/glew/glew.h"
#include "../dependente/glfw/glfw3.h"

#include "../dependente/glm/glm.hpp"
#include "../dependente/glm/gtc/matrix_transform.hpp"
#include "../dependente/glm/gtc/type_ptr.hpp"
#include "../dependente/freeglut/freeglut.h"

#include "../include/shader.hpp"

#include "../dependente/SFML-2.6.1/include/SFML/Audio.hpp"

bool canPress = true;
float enemySpeed = 0.005f;

bool checkCollision(const float x1, const float y1, const float size1, const float x2, const float y2, const float size2) noexcept;
void window_callback(GLFWwindow* window, int new_width, int new_height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void renderText(const float x, const float y, const std::string& text);
void render();
void init();
void window_callback(GLFWwindow* window, int new_width, int new_height);


int main(int argc, char**argv)
{
    bool wPressed = false;
    bool aPressed = false;
    bool sPressed = false;
    bool dPressed = false;

    bool canColide = true;


    constexpr int cooldownDuration = 5;
    constexpr int invulnerability = 3; 
    constexpr int width = 1024, height = 1024;  
    int healh = 4;

    float squareX = 0.0f;
    float squareY = 0.0f;

    float a = +0.5f;
    float b = -0.5f;

    float speed = 0.01f;
    float rotation = 1;
    
    double lastKeyPressTime = 0.0;
    double lastHitTime = 0;
    glm::mat4 trans(1.0f);
    glm::mat4 trans2(1.0f);
    glm::mat4 healtDisplay(1.0f);


    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glutInit(&argc, argv);
    init();
    GLFWwindow* window = glfwCreateWindow(width, height, "Demo", NULL, NULL);
    
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window.");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = true; 
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const GLuint programID = LoadShaders("../shaders/SimpleVertexShader.vertexshader", "../shaders/SimpleFragmentShader.fragmentshader");

    constexpr GLfloat vertices[] = {
        0.05f,  0.05f, 0.0f,  
        0.05f, -0.05f, 0.0f,  
        -0.05f, -0.05f, 0.0f,  
        -0.05f,  0.05f, 0.0f   
    };

    constexpr GLuint indices[] = { 
        0, 3, 1, 
        1, 3, 2  
    };

    GLuint vbo, vao, ibo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    glfwSetFramebufferSizeCallback(window, window_callback);
    glfwSetKeyCallback(window, keyCallback);
    
    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("../assets/Johnny Silverhand Theme(CelloViolin version) Cyberpunk 2077.mp3")) {
        return -1;
    }
    backgroundMusic.setVolume(10);
    backgroundMusic.play();
    backgroundMusic.setLoop(true);
    while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS )
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(programID);
        glBindVertexArray(vao);

        if(!healh)
        {   
            backgroundMusic.pause();
            trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(0.0f, 0.0f, 0.0f));

            unsigned int transformLoc = glGetUniformLocation(programID, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            render();
            
            if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            {
                healh = 4;
                backgroundMusic.play();
                backgroundMusic.play();
            }

        }
        if(healh)
        {   
            
            if (a > squareX) {
                a -= enemySpeed;
            } else {
                a += enemySpeed;
            }

            if (b > squareY) {
                b -= enemySpeed;
            } else  {
                b += enemySpeed;
            }

            if ((wPressed || sPressed) && (aPressed || dPressed)) {
                speed = 0.007f;
            }
            else
                speed = 0.01f;

            wPressed = false;
            aPressed = false;
            sPressed = false;
            dPressed = false;

            if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            {
                squareY = squareY + speed;
                wPressed = true;
            }
            if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            {
                sPressed = true;
                squareY = squareY - speed;
            }
            if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            {
                squareX = squareX - speed;
                aPressed = true;
            }
            if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            {
                squareX = squareX + speed;
                dPressed = true;
            }
            
            double currentTime = glfwGetTime();

            if (currentTime - lastKeyPressTime >= cooldownDuration && !canPress)
            {
                canPress = true;
                enemySpeed = enemySpeed + enemySpeed;
                lastKeyPressTime = currentTime;
            }

            if (currentTime - lastHitTime >= invulnerability && !canColide)
            {
                canColide = true;
            }

            if (checkCollision(squareX, squareY, 0.1f, a, b, 0.1f) && canColide)
            {
                --healh;
                canColide = false;
                lastHitTime = currentTime;
            }

            trans = glm::mat4(1.0f);
            trans = glm::translate(trans, glm::vec3(squareX, squareY, 0.0f));

            unsigned int transformLoc = glGetUniformLocation(programID, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
 

            trans2 = glm::translate(glm::mat4(1.0f), glm::vec3(a, b, 0.0f));
            trans2 = glm::rotate(trans2, rotation+=5, glm::vec3(0.0f, 0.0f, 1.0f));
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans2));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            
        

            for(short i = 0; i < healh; ++i)
            {
                healtDisplay = glm::translate(glm::mat4(1.0f), glm::vec3(-0.9f + i * 0.15f, 0.9f, 0.0f));
                unsigned int transforationHealth = glGetUniformLocation(programID, "transform");
                glUniformMatrix4fv(transforationHealth, 1, GL_FALSE, glm::value_ptr(healtDisplay));
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        
            // Color
            unsigned int transformLoc2 = glGetUniformLocation(programID, "color");
            glm::vec4 color = glm::vec4(0.5f, 0, 0.5f, 1.0);
            glUniform4fv(transformLoc2, 1, glm::value_ptr(color));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            
            if(rotation > 360)
                rotation = 0;
        }
    }

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(programID);
    backgroundMusic.stop();
    glfwTerminate();

    return 0;
}

bool checkCollision(const float x1, const float y1, const float size1, const float x2, const float y2, const float size2) noexcept
{
    const float halfSize1 = size1 / 2.0f;
    const float halfSize2 = size2 / 2.0f;

    //Centrul e la mijlocul elementului. NU MAI SCHIMBATI FUNCTIA!
    const float centerX1 = x1 + halfSize1;
    const float centerY1 = y1 - halfSize1;
    const float centerX2 = x2 + halfSize2;
    const float centerY2 = y2 - halfSize2;

    const bool collisionX = std::abs(centerX1 - centerX2) <= (halfSize1 + halfSize2);
    const bool collisionY = std::abs(centerY1 - centerY2) <= (halfSize1 + halfSize2);

    return collisionX && collisionY;
}

void renderText(const float x, const float y, const std::string& text)
{
    glRasterPos2f(x, y);

    for (const char &c : text) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0); 

    renderText(-0.3f, 0.0f, "You died! Press Space to Try Again!");
}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 400.0, 0.0, 300.0);
}


void window_callback(GLFWwindow* window, int new_width, int new_height)
{
    glViewport(0, 0, new_width, new_height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_R && canPress)
    {
        enemySpeed = enemySpeed/2;
        canPress = false;
    }
}