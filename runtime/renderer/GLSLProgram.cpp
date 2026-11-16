/*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */
 
 /*
 * Copyright 1993-2010 NVIDIA Corporation.  All rights reserved.
 *
 * Please refer to the NVIDIA end user license agreement (EULA) associated
 * with this source code for terms and conditions that govern your use of
 * this software. Any use, reproduction, disclosure, or distribution of
 * this software and related documentation outside the terms of the EULA
 * is strictly prohibited.
 *
 */
 
 #include <stdlib.h>
#include <string>
#include <cstring>
#include "GLSLProgram.h"

static std::string withVersion(const char *src)
{
    if (src && strncmp(src, "#version", 8) != 0)
        return std::string("#version 120\n") + src;
    return src;
}

GLSLProgram::GLSLProgram(const char *vsource, const char *fsource)
{
    mProg = compileProgram(vsource, 0, fsource);
}

GLSLProgram::GLSLProgram(const char *vsource, const char *gsource, const char *fsource,
                         GLenum gsInput, GLenum gsOutput)
{
    mProg = compileProgram(vsource, gsource, fsource, gsInput, gsOutput);
}

GLSLProgram::~GLSLProgram()
{
	if (mProg) {
		glDeleteProgram(mProg);
	}
}

void
GLSLProgram::enable()
{
	glUseProgram(mProg);
}

void
GLSLProgram::disable()
{
	glUseProgram(0);
}

void
GLSLProgram::setUniform1f(const char *name, float value)
{
    GLint loc = glGetUniformLocation(mProg, name);
    if (loc >= 0) {
        glUniform1f(loc, value);
    } else {
#if _DEBUG
        fprintf(stderr, "Error setting parameter '%s'\n", name);
#endif
    }
}

void
GLSLProgram::setUniform2f(const char *name, float x, float y)
{
    GLint loc = glGetUniformLocation(mProg, name);
    if (loc >= 0) {
        glUniform2f(loc, x, y);
    } else {
#if _DEBUG
        fprintf(stderr, "Error setting parameter '%s'\n", name);
#endif
    }
}

void
GLSLProgram::setUniform3f(const char *name, float x, float y, float z)
{
    GLint loc = glGetUniformLocation(mProg, name);
    if (loc >= 0) {
        glUniform3f(loc, x, y, z);
    } else {
#if _DEBUG
        fprintf(stderr, "Error setting parameter '%s'\n", name);
#endif
    }
}

void
GLSLProgram::setUniform4f(const char *name, float x, float y, float z, float w)
{
    GLint loc = glGetUniformLocation(mProg, name);
    if (loc >= 0) {
        glUniform4f(loc, x, y, z, w);
    } else {
#if _DEBUG
        fprintf(stderr, "Error setting parameter '%s'\n", name);
#endif
    }
}

void
GLSLProgram::setUniformMatrix4fv(const GLchar *name, GLfloat *m, bool transpose)
{
    GLint loc = glGetUniformLocation(mProg, name);
    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, transpose, m);
    } else {
#if _DEBUG
        fprintf(stderr, "Error setting parameter '%s'\n", name);
#endif
    }
}

void
GLSLProgram::setUniformfv(const GLchar *name, GLfloat *v, int elementSize, int count)
{
	GLint loc = glGetUniformLocation(mProg, name);
	if (loc == -1) {
#ifdef _DEBUG
        fprintf(stderr, "Error setting parameter '%s'\n", name);
#endif
		return;
	}

    switch (elementSize) {
		case 1:
			glUniform1fv(loc, count, v);
			break;
		case 2:
			glUniform2fv(loc, count, v);
			break;
		case 3:
			glUniform3fv(loc, count, v);
			break;
		case 4:
			glUniform4fv(loc, count, v);
			break;
	}
}

void
GLSLProgram::bindTexture(const char *name, GLuint tex, GLenum target, GLint unit)
{
    GLint loc = glGetUniformLocation(mProg, name);
    if (loc >= 0) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(target, tex);
        glUseProgram(mProg);
        glUniform1i(loc, unit);
        glActiveTexture(GL_TEXTURE0);
    } else {
#if _DEBUG
        fprintf(stderr, "Error binding texture '%s'\n", name);
#endif
    }
}


GLuint
GLSLProgram::compileProgram(const char *vsource, const char *gsource, const char *fsource,
                            GLenum gsInput, GLenum gsOutput)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vsrc = withVersion(vsource);
    std::string fsrc = withVersion(fsource);
    const char *vp = vsrc.c_str();
    const char *fp = fsrc.c_str();

    glShaderSource(vertexShader, 1, &vp, 0);
    glShaderSource(fragmentShader, 1, &fp, 0);
    
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    GLint vok = 0, fok = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vok);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fok);
    if (!vok || !fok) {
        char log[4096];
        if (!vok) {
            glGetShaderInfoLog(vertexShader, sizeof(log), 0, log);
            fprintf(stderr, "Vertex shader compile failed:\n%s\n--- source ---\n%.1500s\n", log, vsource);
        }
        if (!fok) {
            glGetShaderInfoLog(fragmentShader, sizeof(log), 0, log);
            fprintf(stderr, "Fragment shader compile failed:\n%s\n--- source ---\n%.1500s\n", log, fsource);
        }
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    if (gsource) {
        GLuint geomShader = glCreateShader(GL_GEOMETRY_SHADER_EXT);
        glShaderSource(geomShader, 1, &gsource, 0);
        glCompileShader(geomShader);
        glAttachShader(program, geomShader);

        glProgramParameteriEXT(program, GL_GEOMETRY_INPUT_TYPE_EXT, gsInput);
        glProgramParameteriEXT(program, GL_GEOMETRY_OUTPUT_TYPE_EXT, gsOutput); 
        glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, 4); 
    }

    glLinkProgram(program);

    // check if program linked
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char temp[1024];
        glGetProgramInfoLog(program, 1024, 0, temp);
        fprintf(stderr, "Failed to link program:\n%s\n", temp);
        glDeleteProgram(program);
        program = 0;
        exit(-1);
    }

    return program;
}
