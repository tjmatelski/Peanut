// peanut
#include "../Util.hpp"
#include "GLDebug.hpp"
#include "peanut/RenderBuffer.hpp"
#include "peanut/Texture.hpp"
#include <peanut/FrameBuffer.hpp>
#include <peanut/Log.hpp>

// external
#include <glad/glad.h>
#include <variant>

namespace PEANUT {

namespace {
    struct BufferVisitor {
        void operator()(const RenderBuffer* p_buf)
        {
            LOG_TRACE("Binding RenderBuffer: [{}]", p_buf->GetID());
            GLCALL(glFramebufferRenderbuffer(
                GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, p_buf->GetID()));
        }

        void operator()(const Texture* p_tex)
        {
            LOG_TRACE("Binding Texture: [{}]", p_tex->GetID());
            p_tex->Bind();
            if (p_tex->GetConfig().type_ == Texture::Type::COLOR) {
                GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, p_tex->GetID(), 0));
            }
            if (p_tex->GetConfig().type_ == Texture::Type::DEPTH) {
                GLCALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, p_tex->GetID(), 0));
            }
        }
    };
}

FrameBuffer::FrameBuffer(const Buffers& bufs, Config config)
    : m_id(0)
{
    if (bufs.empty()) {
        return;
    }

    // Create framebuffer
    GLCALL(glGenFramebuffers(1, &m_id));
    LOG_TRACE("FrameBuffer Created: id [{}]", m_id);
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_id));

    // Bind buffers to framebuffer
    for (const auto& buf : bufs) {
        std::visit(BufferVisitor {}, buf);
        if (!config.bind_draw_buf_) {
            GLCALL(glDrawBuffer(GL_NONE));
        }
        if (!config.bind_read_buf_) {
            GLCALL(glReadBuffer(GL_NONE));
        }
    }

    // Validate framebuffer
    PN_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) { *this = std::move(other); }

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other)
{
    m_id = other.m_id;
    other.m_id = 0;
    return *this;
}

FrameBuffer::~FrameBuffer()
{
    if (m_id != 0) {
        LOG_TRACE("FrameBuffer Destroyed: id [{}]", m_id);
        GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GLCALL(glDeleteFramebuffers(1, &m_id));
    }
}

void FrameBuffer::Bind() { GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, m_id)); }

void FrameBuffer::Unbind() { GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0)); }

}