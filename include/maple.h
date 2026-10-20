#ifndef __MAPLE_H
#define __MAPLE_H
#include "pine/pine.h"
#include <cglm/types.h>

typedef _Bool MpBool;
#define MP_TRUE 1
#define MP_FALSE 0

typedef struct {
    float x, y, w, h;
} MpRect;

typedef struct {
    float r, g, b, a;
} MpColor;

typedef struct {
    unsigned int shader_hndl;

    int uViewProjectionLocation;
    int uTransformLocation;
    int uColorLocation;
    int uRoundingLocation;
    int uRectLocation;

    mat4 uViewProjection;
    MpColor uColor;
} MpElementShader;

typedef struct {
    MpRect rect;
    MpColor color;
    float rounding;
    MpBool draggable;
    int layer;

    MpBool isDragging;
    int dragging_offset_x;
    int dragging_offset_y;

    mat4 transform;
    unsigned int VAO, VBO;
    MpElementShader shader;
} MpElement;

typedef struct {
    MpElement *elements;
    unsigned int num_elements;
} MpLayout;

MpElement MpCreateElement(MpRect rect, MpColor color, float rounding, MpBool draggable);
void MpUpdateElementTransform(MpElement *element);
void MpRenderElement(MpElement element);
void MpUpdateElement(MpElement *element);
unsigned int MpShaderCompile(const char * restrict frag_src, const char * restrict vert_src);
MpBool MpIsInsideElement(unsigned int x, unsigned int y, MpElement element);
void MpElementHandleEvent(MpElement *element, PxEvent event);

MpLayout MpCreateLayout(void);
void MpAppendElement(MpLayout *layout, MpElement *element);

#endif //__MAPLE_H
