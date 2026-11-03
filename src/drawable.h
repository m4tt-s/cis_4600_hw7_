#pragma once

#include "openglcontext.h"
#include <la.h>

enum BufferType {
    POSITION, NORMAL, COLOR,
    INDEX
};

class Drawable {
protected:
    // See MyGL's `glContext` member for more info
    OpenGLContext *glContext;

    std::unordered_map<BufferType, GLuint> bufferHandles;

    // We will store the number of indices that we send to our
    // index buffer. For example, if the index buffer was
    // {0, 1, 2, 0, 2, 3}, then indexBufferLength would be 6.
    // We need this information in order to tell
    // the glDrawElements function how many indices to read when
    // drawing our geometry.
    int indexBufferLength;

public:
    Drawable(OpenGLContext*);
    virtual ~Drawable();

    // Generate vertex position and triangle index data
    // for your mesh, then pass it to the GPU to be stored
    // in buffers.
    virtual void initializeAndBufferGeometryData() = 0;
    void destroyGPUData();

    // Called in ShaderProgram::draw, this function
    // tells glDrawElements what kind of primitives to
    // draw. The default implementation returns GL_TRIANGLES.
    // You can write subclasses that return GL_LINES or GL_POINTS.
    virtual GLenum drawMode();

    void generateBuffer(BufferType t);
    void bindBuffer(BufferType t);
    bool hasBuffer(BufferType t) const;

    template<class T>
    void bufferData(BufferType t, const std::vector<T> &data) {
        GLenum target = (t == INDEX ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER);
        glContext->glBufferData(target, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    }


    int getIndexBufferLength() const;
};
