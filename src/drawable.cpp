#include "drawable.h"

Drawable::Drawable(OpenGLContext *context)
    : glContext(context),
      bufferHandles(),
      indexBufferLength(-1)
{}

Drawable::~Drawable()
{
    destroyGPUData();
}

void Drawable::destroyGPUData() {
    for(auto &kvp : bufferHandles) {
        glContext->glDeleteBuffers(1, &kvp.second);
    }
    indexBufferLength = 0;
}

GLenum Drawable::drawMode() {
    return GL_TRIANGLES;
}

void Drawable::generateBuffer(BufferType t) {
    bufferHandles[t] = 0; // placeholder, just inserts a kvp into the map
    glContext->glGenBuffers(1, &(bufferHandles.at(t)));
}

void Drawable::bindBuffer(BufferType t) {
    if(t != BufferType::INDEX) {
        glContext->glBindBuffer(GL_ARRAY_BUFFER, bufferHandles.at(t));
    }
    else {
        glContext->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferHandles.at(t));
    }
}

bool Drawable::hasBuffer(BufferType t) const {
    return bufferHandles.contains(t);
}

int Drawable::getIndexBufferLength() const {
    return indexBufferLength;
}



