#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>

#define numVAOs 1
SDL_Window *sdl_window = nullptr;
SDL_GLContext context;

const int win_width = 800;
const int win_height = 600;

/* Demo program to draw a red square using OpenGL 4.3 with SDL2 */

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

GLuint createShaderProgram() {
    const char* vshaderSource = R"glsl(
        #version 430
        void main(void)
        { gl_Position = vec4(0.0, 0.0, 0.0, 1.0); }
    )glsl";
    const char* fshaderSource = R"glsl(
        #version 430
        out vec4 color;
        void main(void)
        { color = vec4(1.0, 0.0, 0.0, 1.0); }
    )glsl";
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    // load GLSL code from the strings into shader objects
    glShaderSource(vShader, 1, &vshaderSource, NULL);
    glShaderSource(fShader, 1, &fshaderSource, NULL);
    // compile them
    glCompileShader(vShader);
    glCompileShader(fShader);
    // create and link a program with the shaders
    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    glLinkProgram(vfProgram);

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
