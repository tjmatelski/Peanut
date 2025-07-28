// peanut
#include "peanut/RenderBuffer.hpp"
#include "GLDebug.hpp"
#include "peanut/Log.hpp"

// external
#include <glad/glad.h>

namespace PEANUT {

RenderBuffer::RenderBuffer(Config config)
    : config_(config)
    , id_(0)
{
    GLCALL(glGenRenderbuffers(1, &id_));
    LOG_TRACE("Created RenderBuffer [{}] [w: {}, h: {}]", id_, config.width_, config.height_);
    Bind();
    GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, config.width_, config.height_));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

RenderBuffer::~RenderBuffer()
{
    if (id_ != 0) {
        LOG_TRACE("Deleting RenderBuffer [{}]", id_);
        GLCALL(glDeleteRenderbuffers(1, &id_));
        id_ = 0;
    }
}

RenderBuffer::RenderBuffer(RenderBuffer&& other) { *this = std::move(other); }

RenderBuffer& RenderBuffer::operator=(RenderBuffer&& other)
{
    id_ = other.id_;
    other.id_ = 0;
    return *this;
}

void RenderBuffer::Bind() { GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, id_)); }

}
