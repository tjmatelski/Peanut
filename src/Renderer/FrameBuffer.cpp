#include "../Util.hpp"
#include "GLDebug.hpp"
#include <peanut/FrameBuffer.hpp>
#include <peanut/Log.hpp>

// external
#include <glad/glad.h>

namespace PEANUT {

FrameBuffer::FrameBuffer(const FrameBufferConfig& config)
    : m_config(config)
{
    Create();
}

FrameBuffer::~FrameBuffer() { Destroy(); }
void FrameBuffer::Bind() { GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_id)); }

void FrameBuffer::Unbind() { GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0)); }

void FrameBuffer::Resize(int width, int height)
{
    LOG_DEBUG("Resizing Framebuffer to [{}, {}]", width, height);
    m_config.width = width;
    m_config.height = height;
    Destroy();
    Create();
}

void FrameBuffer::Create()
{
    GLCALL(glGenFramebuffers(1, &m_id));
    LOG_DEBUG("FrameBuffer Created: id [{}] type [{}] width [{}] height [{}]", m_id, int(m_config.type), m_config.width,
        m_config.height);
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));

    GLCALL(glGenTextures(1, &m_textureID));
    GLCALL(glBindTexture(GL_TEXTURE_2D, m_textureID));
    if (m_config.type == FrameBufferConfig::Type::COLOR) {
        GLCALL(glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB, m_config.width, m_config.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    }
    if (m_config.type == FrameBufferConfig::Type::DEPTH) {
        GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_config.width, m_config.height, 0,
            GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
    // GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));

    if (m_config.type == FrameBufferConfig::Type::COLOR) {
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureID, 0));
        GLCALL(glGenRenderbuffers(1, &m_renderBufferID));
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferID));
        GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_config.width, m_config.height));
        GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
        GLCALL(
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBufferID));
    }
    if (m_config.type == FrameBufferConfig::Type::DEPTH) {
        GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_textureID, 0));
        GLCALL(glDrawBuffer(GL_NONE));
        GLCALL(glReadBuffer(GL_NONE));
    }

    // GLCALL(glGenRenderbuffers(1, &m_renderBufferID));
    // GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferID));
    // GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_config.width, m_config.height));
    // GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    // GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
    // m_renderBufferID));

    PN_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::Destroy()
{
    LOG_DEBUG("FrameBuffer Destroyed: id = {}", m_id);
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCALL(glDeleteFramebuffers(1, &m_id));
}

}