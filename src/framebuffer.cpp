#include "framebuffer.h"

namespace GRender
{
    
    Framebuffer::Framebuffer(uint32_t width, uint32_t height) : size(width, height)
    {
        glad_glGenFramebuffers(1, &bufferID);
        glad_glBindFramebuffer(GL_FRAMEBUFFER, bufferID);

        glad_glGenTextures(1, &textureID);
        glad_glBindTexture(GL_TEXTURE_2D, textureID);
        glad_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glad_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Assigning texture to framebuffer
        glad_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

        // Testing if it worked properly
        if (glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
           gr_pout("ERROR: Framebuffer is incomplete!!");
            exit(-1);
        }

        // binding standard buffer back
        glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);

    } // constructor

    Framebuffer::~Framebuffer(void)
    {
        glad_glDeleteFramebuffers(1, &bufferID);
        glad_glDeleteTextures(1, &textureID);
    } // destructor

    void Framebuffer::bind(void)
    {
        glad_glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
        glad_glViewport(0, 0, int(size.x), int(size.y));
    }

    void Framebuffer::unbind(void)
    {
        glad_glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}