export module Graphics.Buffer;

import std;
import OpenGL;

export class Buffer {
public:
    Buffer(): _ID(0), _elementsCount(0), _size(0), _batched(false) {}

    ~Buffer() {
        glDeleteBuffers(1, &_ID);
    }

    void allocate(const uint32_t size, const void* data, const GLuint elementsCount, const bool batched = false,
                  const GLenum   type = GL_STATIC_DRAW) {
        _elementsCount = elementsCount;
        _size          = size;
        _batched       = batched;

        glCreateBuffers(1, &_ID);
        glNamedBufferData(_ID, size, data, type);
    }

    void write(const uint32_t offset, const int32_t size, const void* data) {
        if (offset + size > _size || !_ID) {
            return;
        }
        glNamedBufferSubData(_ID, offset, size, data);
    }

    GLuint getID() const { return _ID; }
    GLuint getElementsCount() const { return _elementsCount; }
    GLuint getSize() const { return _size; }
    bool   isBatched() const { return _batched; }

private:
    GLuint _ID;
    GLuint _elementsCount;
    GLuint _size;
    bool   _batched;
};
