#include "element_shader.h"
#include <maple.h>
#include <pine/pine.h>
#include <cglm/cglm.h>

#include <stddef.h>
#include <stdio.h>



const float g_MpSquareVertices[] = {
     // First Triangle
      0.5f,  0.5f, // Top Right
      0.5f, -0.5f, // Bottom Right
     -0.5f,  0.5f, // Top Left
     // Second Triangle
      0.5f, -0.5f, // Bottom Right
     -0.5f, -0.5f, // Bottom Left
     -0.5f,  0.5f, // Top Left
};

static unsigned int MpShaderCompileStage(GLenum type, const char * src) {
    unsigned int shader = glCreateShader(type);
    if (shader == 0) {
        fprintf(stderr, "maple: glCreateShader failed (0x%x)\n", glGetError());
        return 0;
    }

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        fprintf(stderr, "maple: %s shader compilation failed:\n%s\n",
                type == GL_VERTEX_SHADER ? "vertex" : "fragment", log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

unsigned int MpShaderCompile(const char * frag_src, const char * vert_src) {
    unsigned int vert = MpShaderCompileStage(GL_VERTEX_SHADER, vert_src);
    if (vert == 0) {
        return 0;
    }

    unsigned int frag = MpShaderCompileStage(GL_FRAGMENT_SHADER, frag_src);
    if (frag == 0) {
        glDeleteShader(vert);
        return 0;
    }

    unsigned int program = glCreateProgram();
    if (program == 0) {
        fprintf(stderr, "maple: glCreateProgram failed (0x%x)\n", glGetError());
        glDeleteShader(vert);
        glDeleteShader(frag);
        return 0;
    }

    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    glDetachShader(program, vert);
    glDetachShader(program, frag);
    glDeleteShader(vert);
    glDeleteShader(frag);

    int success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        fprintf(stderr, "maple: shader program linking failed:\n%s\n", log);
        glDeleteProgram(program);
        return 0;
    }

    return program;
}

MpElementShader MpElementShaderInit(void) {
    MpElementShader ret = {0};
    ret.shader_hndl = MpShaderCompile(element_fragment_shader_source, element_vertex_shader_source);

    ret.uColorLocation = glGetUniformLocation(ret.shader_hndl, "uColor");
    ret.uViewProjectionLocation = glGetUniformLocation(ret.shader_hndl, "uViewProjection");
    ret.uTransformLocation = glGetUniformLocation(ret.shader_hndl, "uTransform");
    ret.uRoundingLocation = glGetUniformLocation(ret.shader_hndl, "uRounding");
    ret.uRectLocation = glGetUniformLocation(ret.shader_hndl, "uRect");
    return ret;
}


MpElement MpCreateElement(MpRect rect, MpColor color, float rounding, MpBool draggable) {
    MpElement element = {
        .rect = rect,
        .color = color,
        .rounding = rounding,
        .draggable = draggable,
        .VAO = 0,
        .VBO = 0
    };

    glGenVertexArrays(1, &element.VAO);
    glGenBuffers(1, &element.VBO);

    glBindVertexArray(element.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, element.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_MpSquareVertices), g_MpSquareVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    element.shader = MpElementShaderInit();
    MpUpdateElementTransform(&element);
    return element;
}

void MpUpdateElementTransform(MpElement *element) {
    MpRect r = element->rect;

    glm_mat4_identity(element->transform);
    glm_translate(element->transform, (vec3){r.x + r.w * 0.5f, r.y + r.h * 0.5f, 0.0f});
    glm_scale(element->transform, (vec3){r.w, r.h, 1.0f});
}

MpBool MpIsInsideElement(unsigned int x, unsigned int y, MpElement element) {
    return (x >= element.rect.x &&
            x <= element.rect.x + element.rect.w &&
            y >= element.rect.y &&
            y <= element.rect.y + element.rect.h);
}

void MpUpdateElement(MpElement *element) {
    MpUpdateElementTransform(element);
}

void MpRenderElement(MpElement element) {
    glUseProgram(element.shader.shader_hndl);

    glUniform4f(element.shader.uColorLocation, 
            element.color.r, element.color.g, element.color.b, element.color.a);
    glUniform4f(element.shader.uRectLocation, element.rect.x, element.rect.y, element.rect.w, element.rect.h);
    glUniformMatrix4fv(element.shader.uViewProjectionLocation, 1, GL_FALSE, element.shader.uViewProjection[0]);
    glUniformMatrix4fv(element.shader.uTransformLocation, 1, GL_FALSE, element.transform[0]);
    glUniform1f(element.shader.uRoundingLocation, element.rounding);

    glBindVertexArray(element.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

