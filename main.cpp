
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <GLFW/glfw3.h>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdlib>     /* srand, rand */
#include <ctime>
#include <queue>
#include <utility>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 640;
const unsigned int SCR_HEIGHT = 640;

const unsigned int GRID_WIDTH = 10;
const unsigned int GRID_HEIGHT = 10;
const float cellWidth = 50.0f;
const float cellHeight = 50.0f;

enum CellState {
    HIDDEN,
    REVEALED,
    MEME,
    FLAGGED
};

struct Cell {
    CellState state = CellState::HIDDEN;
    bool isMeme = false;  // Whether this cell contains a meme
    int neighboringMemeCount = 0;  // Number of memes in adjacent cells
};

Cell grid[GRID_HEIGHT][GRID_WIDTH];
bool firstClick = false;

void calculateMemeCounts() {
    // Directions for the 8 neighboring cells
    const int dx[] = {-1, -1, -1,  0, 0, 1, 1, 1};
    const int dy[] = {-1,  0,  1, -1, 1, -1, 0, 1};

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            // Skip cells that are memes
            if (grid[y][x].isMeme) continue;

            // Check the 8 neighboring cells
            for (int i = 0; i < 8; ++i) {
                int nx = x + dx[i];
                int ny = y + dy[i];

                // Ensure the neighbor is within the grid bounds
                if (nx >= 0 && nx < GRID_WIDTH && ny >= 0 && ny < GRID_HEIGHT) {
                    if (grid[ny][nx].isMeme) {
                        grid[y][x].neighboringMemeCount++;
                    }
                }
            }
        }
    }
}

void placeMemes(int numMemes) {
    int placedMemes = 0;
    std::srand(std::time(nullptr));
    while (placedMemes < numMemes) {
        int x = std::rand() % GRID_WIDTH;
        int y = std::rand() % GRID_HEIGHT;

        // If there's no meme at this position, place one
        if (!grid[y][x].isMeme) {
            grid[y][x].isMeme = true;
            placedMemes++;
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
     if (action == GLFW_PRESS) {
        double xpos, ypos;
        // Get the mouse position
        glfwGetCursorPos(window, &xpos, &ypos);

        // Define constants
        int cell_size = 50;          // Actual cell size
        int grid_size = 500;         // 10x10 grid with 50x50 cells
        int window_size = 640;       // Window size
        int offset = (window_size - grid_size) / 2;  // Offset for centering (70 pixels)

        // Invert the y-coordinate
        ypos = window_size - ypos;

        // Adjust for the offset
        xpos -= offset;
        ypos -= offset;

        // Calculate the grid cell based on mouse position
        int grid_x = static_cast<int>(std::round(xpos)) / cell_size;
        int grid_y = static_cast<int>(std::round(ypos)) / cell_size;
        
        // The grid is assumed to be a 2D array of Cells, where each Cell has a boolean isRevealed and isMeme.

        

        if(!firstClick){
            // Use a flood-fill algorithm to reveal a "safe" area
            
            // Check if the starting position is safe
            if (grid[grid_y][grid_x].isMeme) {
                // In some versions of Minesweeper, the game would reposition the meme if you click on one first
                // You can handle this however you'd like.
                return; // Or reposition the meme at this point
            }

            // Use a queue for breadth-first search (BFS) to reveal neighboring squares
            std::queue<std::pair<int, int>> toReveal;
            toReveal.push({grid_x, grid_y});

            while (!toReveal.empty()) {
                auto [x, y] = toReveal.front();
                toReveal.pop();

                // Check bounds
                if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT)
                    continue;

                // If the cell is already revealed or is a meme, skip it
                if (grid[y][x].state == CellState::REVEALED || grid[y][x].isMeme)
                    continue;

                // Reveal the cell
                grid[y][x].state = CellState::REVEALED;

                // Count neighboring memes
                int memeCount = grid[y][x].neighboringMemeCount;

                // If no adjacent memes, reveal neighbors
                if (memeCount == 0) {
                    // Add neighboring cells to the queue
                    for (int offsetY = -1; offsetY <= 1; ++offsetY) {
                        for (int offsetX = -1; offsetX <= 1; ++offsetX) {
                            if (offsetX == 0 && offsetY == 0) continue;  // Skip the current cell
                            toReveal.push({x + offsetX, y + offsetY});
                        }
                    }

                    firstClick = true;

                }
            }
            
            

        } else {
            // Ensure the click is within the grid bounds
            if (xpos >= 0 && xpos < grid_size && ypos >= 0 && ypos < grid_size) {
                if (button == GLFW_MOUSE_BUTTON_LEFT) {
                    std::cout << "Left mouse button pressed at (" << xpos << ", " << ypos << ")" << std::endl;
                    grid[grid_y][grid_x].state = CellState::FLAGGED;
                }
                if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                    std::cout << "Right mouse button pressed at (" << xpos << ", " << ypos << ")" << std::endl;
                    if(grid[grid_y][grid_x].isMeme)
                        grid[grid_y][grid_x].state = CellState::MEME;
                    else if (!grid[grid_y][grid_x].isMeme)
                        grid[grid_y][grid_x].state = CellState::REVEALED;
                }
            } else {
                std::cout << "Click was outside the grid." << std::endl;
            }
        }

    }
}

struct Vertex {
    glm::vec2 position;
    glm::vec2 texCoords;  // Assuming you have texture coordinates too
    float textureID;
};

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Set the mouse button callback
    //glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    {

        
        //placeMemes(10);
        //calculateMemeCounts();

        for (size_t y = 0; y < GRID_HEIGHT; y++)
        {
            for (size_t x = 0; x < GRID_WIDTH; x++)
            {
                std::cout << " grid cell [ " << y << ", " << x << " ] " << grid[y][x].neighboringMemeCount << ", "<< grid[y][x].isMeme << std::endl;
            }
            
        }
        
        // float vertices[] = {
        //     -250.0f,  -250.0f,   0.0f, 0.0f, // left  
        //     -200.0f,  -250.0f,   1.0f, 0.0f, // right 
        //     -200.0f,  -200.0f,   1.0f, 1.0f, // top  
        //     -250.0f,  -200.0f,   0.0f, 1.0f
        // };
        

        // unsigned int indices[]= {
        //     0, 1, 2,
        //     2, 3, 0
        // };

        glm::vec2 quadVertices[] = {
            {-250.0f,  -250.0f},  // Bottom-left
            {-200.0f,  -250.0f},  // Bottom-right
            {-200.0f,  -200.0f},  // Top-right
            {-250.0f,  -200.0f}  // Top-left
        };

         glm::vec2 quadTextureIndices[] = {
            {0.0f, 0.0f},  // Bottom-left
            {1.0f, 0.0f},  // Bottom-right
            {1.0f, 1.0f},  // Top-right
            {0.0f, 1.0f}  // Top-left
        };

        glm::vec2 offsets[GRID_WIDTH * GRID_HEIGHT];
        
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                offsets[y * GRID_WIDTH + x] = glm::vec2(x * cellWidth, y * cellHeight);  // Position each cell in the grid
            }
        }

        std::vector<Vertex> finalVertexBuffer;

        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                // Get the offset for the current grid cell
                glm::vec2 gridOffset = offsets[y * GRID_WIDTH + x];

                // For each quad vertex, add the offset to the local quad position
                for (int i = 0; i < 4; ++i) {  // Assuming 4 vertices per quad
                    Vertex v;
                    v.position = quadVertices[i] + gridOffset;  // Add offset to quad vertex
                    v.texCoords = quadTextureIndices[i]; /* Set appropriate texture coordinates here */
                    v.textureID = 9.0f;
                    finalVertexBuffer.push_back(v);
                }
            }
        }

        std::vector<unsigned int> indices;
        int quadIndexOffset = 0;

        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                // Add indices for each quad
                indices.push_back(quadIndexOffset + 0);
                indices.push_back(quadIndexOffset + 1);
                indices.push_back(quadIndexOffset + 2);
                indices.push_back(quadIndexOffset + 2);
                indices.push_back(quadIndexOffset + 3);
                indices.push_back(quadIndexOffset + 0);

                // Move the offset to the next set of vertices (each quad has 4 vertices)
                quadIndexOffset += 4;
            }
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //Create the vertex Array and vertex Buffer
        VertexArray va;
        //VertexBuffer vb (vertices, 4 * 4 * sizeof(float));
        //VertexBuffer vb( &finalVertexBuffer, ( 4 * 4 + 4 ) * GRID_WIDTH * GRID_HEIGHT  * sizeof(float), true );
        VertexBuffer vb( &finalVertexBuffer[0], 5 * finalVertexBuffer.size()  * sizeof(float), false );
        
        // Create the vertex array layout and bind the buffer and the layout
        VertexBufferLayout layout;
        //layout.push(2, VALUETYPE::FLOAT);
        //layout.push(2, VALUETYPE::FLOAT);
        layout.Push<float>(2);
        layout.Push<float>(2);
        layout.Push<float>(1);
        va.AddBuffer(vb, layout);

        //IndexBuffer ibo( indices.data(), GRID_HEIGHT * GRID_WIDTH * 6);
        IndexBuffer ibo( &indices[0], indices.size());
       
        //glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f);
        glm::mat4 proj = glm::ortho(-320.0f, 320.0f, -320.0f, 320.0f, -1.0f, 1.0f);

        Shader shader("basic.shader");
        //shader.SetUniform4f("u_Color", 1.0f, 0.5f, 0.2f, 1.0f);
        

        
        

        
        // Texture zero("res/zero.png"); // For 0 adjacent memes
        // Texture one("res/one.png");  // For 1 adjacent meme
        // Texture two("res/two.png");  // For 2 adjacent memes
        // Texture three("res/three.png");  // For 3 adjacent memes
        // Texture four("res/four.png");  // For 4 adjacent memes
        // Texture five("res/five.png");  // For 5 adjacent memes
        // Texture six("res/six.png");  // For 6 adjacent memes
        // Texture seven("res/seven.png");  // For 7 adjacent memes
        // Texture eight("res/eight.png");   // For 8 adjacent memes
    

        Texture hidden ("res/hidden.png");
        hidden.Bind();
        // Texture flag ("res/flag.png");
        // Texture mine ("res/mine.png");
        


        // VertexBuffer instancedVBO ( offsets, GRID_HEIGHT * GRID_WIDTH * 2 * sizeof(float) );
        // // Now, bind it to the quad VAO
        // glEnableVertexAttribArray(2);
        // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        // glVertexAttribDivisor(2, 1); // Tell OpenGL this is an instanced attribute
        // glBindVertexArray(0);

        shader.Bind();
        shader.SetUniformMat4f("u_MVP", proj);
        shader.Bind();
        shader.SetUniform1i("u_Texture", 0);
        // int sampler[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        // shader.SetUniform1iVec("u_Textures", sampler);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        // va.Unbind();
        // vb.Unbind();
        // shader.Unbind();
        Renderer renderer;
        // uncomment this call to draw in wireframe polygons.
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // render loop
        // -----------
        while (!glfwWindowShouldClose(window))
        {
            // input
            // -----
            //processInput(window);
            // render
            // ------
            renderer.Clear();
            
            shader.Bind();

        
            // zero.Bind(0);
            // one.Bind(1);
            // two.Bind(2);
            // three.Bind(3);
            // four.Bind(4);
            // five.Bind(5);
            // six.Bind(6);
            // seven.Bind(7);
            // eight.Bind(8);

            hidden.Bind();
            // flag.Bind(10);
            // mine.Bind(11);
            

            //shader.SetUniform4f("u_Color", r, 0.5f, 0.2f, 1.0f);
            // draw our first triangle
            
            renderer.Draw(va, ibo, shader);

            // if (r > 1.0f)
            //     increment = -0.05f;
            // else if (r < 0.0f)
            //     increment = 0.05f;

            // r += increment;
            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            // -------------------------------------------------------------------------------
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // optional: de-allocate all resources once they've outlived their purpose:
        // ------------------------------------------------------------------------
        // glfw: terminate, clearing all previously allocated GLFW resources.
        // ------------------------------------------------------------------
    }
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
// void processInput(GLFWwindow *window)
// {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

