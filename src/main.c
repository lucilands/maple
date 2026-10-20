#include "pine/pxutils.h"
#include <cglm/mat3.h>
#include <pine/pine.h>
#include <cglm/cglm.h>
#include <maple.h>

#include <stddef.h>
#include <stdio.h>


const PxWindowInfo window_info = {
    .width = 1080,
    .height = 720,
    .x = 0, .y = 0,
    .title = "Maple"
};

int main(void) {
    PxResult result = 0;
    PxContext *context = PxCreateContext(&result);

    PxWindow *window = PxCreateWindow(context, window_info, NULL);
    PxLoadOpenGL(context, window, 4, 3);
    if (result) {
        fprintf(stderr, "[ERROR]: Failed to load OpenGL: %s\n", PxErrorToString(result));
        return result;
    }
    printf("[INFO]: Initialized OpenGL 4.3\n");

    bool running = 1;
    PxEvent event = {0};

    MpElement element1 = MpCreateElement((MpRect){0, 0, 100, 100}, (MpColor){0.3f, 0.4f, 0.3f, 1}, 10.0f, MP_TRUE);
    MpElement element2 = MpCreateElement((MpRect){20, 60, 50, 100}, (MpColor){0.4f, 0.3f, 0.3f, 1}, 10.0f, MP_TRUE);
    MpElement element3 = MpCreateElement((MpRect){100, 300, 100, 200}, (MpColor){0.3f, 0.3f, 0.4f, 1}, 0.0f, MP_TRUE);
    
    glm_ortho(0, window_info.width, window_info.height, 0, -1, 1, element1.shader.uViewProjection);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int *param = NULL;
    MpBool wireframe = MP_FALSE;
    while (running) {
        while (PxPollEvents(window, &event)) {
            MpElementHandleEvent(&element1, event);
            MpElementHandleEvent(&element2, event);
            MpElementHandleEvent(&element3, event);
            switch (event.type) {
                case PX_EVENT_CLOSE:
                    running = 0;
                    break;
                case PX_EVENT_KEYDOWN:
                    switch (event.keycode) {
                        case PX_KEY_Q:
                            running = 0;
                            break;
                        case PX_KEY_W:
                            if (wireframe) {
                                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                                wireframe = MP_FALSE;
                            } else {
                                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                                wireframe = MP_TRUE;
                            }
                            break;
                    }
                    break;
                case PX_EVENT_RESIZE:
                    param = (int*)PxGetWindowParam(context, window, PX_PARAM_SIZE);
                    glViewport(0, 0, param[0], param[1]);
                    glm_ortho(0, param[0], param[1], 0, -1, 1, element1.shader.uViewProjection);
                    glm_ortho(0, param[0], param[1], 0, -1, 1, element2.shader.uViewProjection);
                    glm_ortho(0, param[0], param[1], 0, -1, 1, element3.shader.uViewProjection);
                    break;
                default:
                    break;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
   
        MpUpdateElement(&element1);
        MpRenderElement(element1);
        MpUpdateElement(&element2);
        MpRenderElement(element2);
        MpUpdateElement(&element3);
        MpRenderElement(element3);

        PxSwapBuffers(window);
    }

    PxDestroyWindow(window);
    PxDestroyContext(context);
    return 0;
}
