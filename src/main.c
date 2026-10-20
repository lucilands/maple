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

    MpElement element = MpCreateElement((MpRect){0, 0, 100, 100}, (MpColor){0.3f, 0.4f, 0.3f, 1}, 10.0f, MP_TRUE);
    glm_ortho(0, window_info.width, window_info.height, 0, -1, 1, element.shader.uViewProjection);

    int *param = NULL;
    PxUint32 mouse_x = 0, mouse_y = 0;
    int x_offset = 0, y_offset = 0;
    while (running) {
        while (PxPollEvents(window, &event)) {
            switch (event.type) {
                case PX_EVENT_CLOSE:
                    running = 0;
                    break;
                case PX_EVENT_KEYDOWN:
                    switch (event.keycode) {
                        case PX_KEY_Q:
                            running = 0;
                            break;
                    }
                    break;
                case PX_EVENT_RESIZE:
                    param = (int*)PxGetWindowParam(context, window, PX_PARAM_SIZE);
                    glViewport(0, 0, param[0], param[1]);
                    glm_ortho(0, param[0], param[1], 0, -1, 1, element.shader.uViewProjection);
                    break;
                case PX_EVENT_MOUSE_DOWN:
                    if (MpIsInsideElement(mouse_x, mouse_y, element)) {
                        element.isDragging = MP_TRUE;
                        x_offset = mouse_x - element.rect.x;
                        y_offset = mouse_y - element.rect.y;
                    }
                    break;
                case PX_EVENT_MOUSE_UP:
                    element.isDragging = MP_FALSE;
                    break;
                case PX_EVENT_MOUSE_MOVE:
                    mouse_x = event.mouse_x;
                    mouse_y = event.mouse_y;

                    
                    if (element.isDragging && element.draggable) {
                        element.rect.x = mouse_x - x_offset;
                        element.rect.y = mouse_y - y_offset;
                    }

                    break;
                default:
                    break;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
   
        MpUpdateElement(&element);
        MpRenderElement(element);

        PxSwapBuffers(window);
    }

    PxDestroyWindow(window);
    PxDestroyContext(context);
    return 0;
}
