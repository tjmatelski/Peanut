#include "GLDebug.h"
#include "Util.h"
#include <Renderer/FrameBuffer.h>

#include <glad/glad.h>

namespace PEANUT {

FrameBuffer::FrameBuffer()
{
    GLCALL(glGenFramebuffers(1, &m_id));
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));

    GLCALL(glGenTextures(1, &m_colorBufferID));
    GLCALL(glBindTexture(GL_TEXTURE_2D, m_colorBufferID));
    GLCALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCALL(glBindTexture(GL_TEXTURE_2D, 0));

    GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBufferID, 0));

    GLCALL(glGenRenderbuffers(1, &m_renderBufferID));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferID));
    GLCALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600));
    GLCALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

    GLCALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderBufferID));

    PN_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void FrameBuffer::Bind()
{
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));
}

void FrameBuffer::Unbind()
{
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

FrameBuffer::~FrameBuffer()
{
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GLCALL(glDeleteFramebuffers(1, &m_id));
}

}