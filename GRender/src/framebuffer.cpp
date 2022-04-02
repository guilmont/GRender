#include "framebuffer.h"

namespace GRender
{
    
    Framebuffer::Framebuffer(uint32_t width, uint32_t height) : size(width, height)
    {
        glGenFramebuffers(1, &bufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, bufferID);

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Assigning texture to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);


        glGenTextures(1, &depthID);
        glBindTexture(GL_TEXTURE_2D, depthID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthID, 0);



        // Testing if it worked properly
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
           pout("ERROR: Framebuffer is incomplete!!");
            exit(-1);
        }

        // binding standard buffer back
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } 

    Framebuffer::~Framebuffer(void)
    {
        glDeleteTextures(1, &depthID);
        glDeleteTextures(1, &textureID);
        glDeleteFramebuffers(1, &bufferID);
    }

    void Framebuffer::bind(void)
    {
        assert(bufferID != 0);
        glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
        glViewport(0, 0, uint32_t(size.x), uint32_t(size.y));
    }

    void Framebuffer::unbind(void)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}