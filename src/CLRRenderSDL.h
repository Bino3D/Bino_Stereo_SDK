#ifndef CLRRENDERSDL_H_
#define CLRRENDERSDL_H_
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include <glm/mat4x4.hpp>
class CLRRenderSDL{
public:
    CLRRenderSDL();
    ~CLRRenderSDL();

    void init();
    bool render(float *outQ, unsigned int *keys);
    void getQ(float *a_q, unsigned int *button);
    bool renderByInputQ(float *inQ);
private:
    void renderr();
    SDL_Window *m_window;
    SDL_GLContext m_glContext;
    glm::mat4 projection;
    glm::mat4 modelview;
    glm::vec4 viewport;
    unsigned char *background_image;
    SDL_Event e;
    //CLRMoveBlueToothTest *bt;
    float sys_q[4*3];
    bool reset_q;
    float reset_q_array[4];
    float frame_q_array[4];
    //Tracker &m_tracker;
};




#endif /* CLRRENDERSDL_H_ */
