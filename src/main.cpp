#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <string>
#include <fstream>

#define numVAOs 1
SDL_Window* sdl_window = nullptr;
SDL_GLContext context;

const int win_width = 800;
const int win_height = 600;

/* Demo program of basic use of OpenGL 4.3 with SDL2 */

void printShaderLog(GLuint shader) {
    int len = 0;
    int chWrittn = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        char buffer[512];
        glGetShaderInfoLog(shader, len, &chWrittn, buffer);
        std::cout << "Shader Info Log: " << buffer << std::endl;
    }
}

void printProgramLog(int prog) {
    int len = 0;
    int chWrittn = 0;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        char buffer[512];
        glGetProgramInfoLog(prog, len, &chWrittn, buffer);
        std::cout << "Program Info Log: " << buffer << std::endl;
    }
}

bool checkOpenGLError() {
    bool foundError = false;
    int glErr = glGetError();
    while (glErr != GL_NO_ERROR) {
        std::cerr << "glError: " << glErr << std::endl;
        foundError = true;
        glErr = glGetError();
    }
    return foundError;
}

int init(const char* title, unsigned int win_flags) {
    // Return 0 for success and -1 for failure
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "Failed to init SDL video, error: "
                  << SDL_GetError()
                  << std::endl;
        return -1;
    }

    sdl_window = nullptr;
    sdl_window = SDL_CreateWindow(title,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  win_width,
                                  win_height,
                                  win_flags);
    if (sdl_window == nullptr) {
        std::cerr << "Could not create window: "
                  << SDL_GetError()
                  << std::endl;
        return -1;
    }

    // Initialize OpenGL necessities (OpenGL 4.3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    context = SDL_GL_CreateContext(sdl_window);
    // glewExperimental = GL_TRUE;
    glewInit();

    return 0;
}

std::string readShaderSource(const char* filePath) {
    std::string content;
    std::ifstream fin(filePath, std::ios::in);
    std::string line = "";
    while (std::getline(fin, line)) {
        content.append(line + "\n");
    }
    fin.close();
    return content;
}

GLuint createShaderProgram() {
    GLint vertCompiled;
    GLint fragCompiled;
    GLint linked;
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    const std::string vertShaderString = readShaderSource("src/vertShader.glsl");
    const std::string fragShaderString = readShaderSource("src/fragShader.glsl");
    const char* vShaderSource = vertShaderString.c_str();
    const char* fShaderSource = fragShaderString.c_str();

    // load GLSL code from the strings into shader objects
    glShaderSource(vShader, 1, &vShaderSource, NULL);
    glShaderSource(fShader, 1, &fShaderSource, NULL);
    // compile them and check for errors
    glCompileShader(vShader);
    checkOpenGLError();
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
    if (vertCompiled != 1) {
        std::cerr << "Vertex compilation failed" << std::endl;
        printShaderLog(vShader);
    }

    glCompileShader(fShader);
    checkOpenGLError();
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled != 1) {
        std::cerr << "Fragment compilation failed" << std::endl;
        printShaderLog(fShader);
    }
    // create and link a program with the shaders, and check for errors
    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    glLinkProgram(vfProgram);
    checkOpenGLError();
    glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
    if (linked != 1) {
        std::cerr << "Linking failed" << std::endl;
        printProgramLog(vfProgram);
    }

    return vfProgram;
}

int main() {
    GLuint vao[numVAOs];
    GLuint renderingProgram;

    // Init, exit if it fails
    if (init("sdl2opengl", SDL_WINDOW_OPENGL) == -1) {
        return -1;
    }

    renderingProgram = createShaderProgram();
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);

    glUseProgram(renderingProgram); // load shader program into gpu
    glPointSize(30.0f);
    glDrawArrays(GL_POINTS, 0, 1);

    bool isRunning = true;
    SDL_Event sdl_event;
    while (isRunning)
    {
        while (SDL_PollEvent(&sdl_event) != 0)
        {
            if (sdl_event.type == SDL_QUIT)
            {
                isRunning = false;
            }
            else if (sdl_event.type == SDL_KEYDOWN)
            {
                switch (sdl_event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    isRunning = false;
                    break;
                }
            }

            SDL_GL_SwapWindow(sdl_window);
        }
    }

    // clean up
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();

    return 0;
}
