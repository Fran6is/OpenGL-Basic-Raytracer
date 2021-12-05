
#include <Buffer.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

QuadBuffer::~QuadBuffer() 
{
    std::cout << "Quad buffer deleted! \n";
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void QuadBuffer::Prepare() 
{
    if(!bPreparedBuffer) 
        PrepareQuadBuffer(*this);
}

void QuadBuffer::BindVAO() const
{
    if(bPreparedBuffer) 
        glBindVertexArray(VAO);
}

const glm::vec2 QuadVertexData[]
{
    //Positions             //Texture co-ordinates
    glm::vec2(-1.f, +1.f), glm::vec2(0.f, 1.f),
    glm::vec2(-1.f, -1.f), glm::vec2(0.f, 0.f),
    glm::vec2(+1.f, -1.f), glm::vec2(1.f, 0.f),
    glm::vec2(+1.f, +1.f), glm::vec2(1.f, 1.f)
};

const unsigned int QuadDrawIndices[]
{
    0, 1, 2,
    2, 3, 0
};

void PrepareQuadBuffer(QuadBuffer& OutData)
{
    
    //Quad
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertexData), QuadVertexData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(QuadDrawIndices), QuadDrawIndices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2  * sizeof(QuadVertexData[0]), (void*)0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(QuadVertexData[0]), (void*)(sizeof(QuadVertexData[0])));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    OutData.VAO = VAO;
    OutData.VBO = VBO;
    OutData.EBO = EBO;
    OutData.EBO_Count = 6;
    OutData.bPreparedBuffer = true;

    std::cout << "Quad buffer prepared! \n";
}

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
void PrepareFramebufferWithTwoTextureAttachments(size_t& GFramebuffer, size_t& GPositionTex, size_t& GNormalTex) 
{
    SCR_WIDTH = 800;
    SCR_HEIGHT = 600;
    glGenFramebuffers(1, &GFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, GFramebuffer);
    
    // - Position color buffer
    glGenTextures(1, &GPositionTex);
    glBindTexture(GL_TEXTURE_2D, GPositionTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GPositionTex, 0); //attach
    
    // - Normal color buffer
    glGenTextures(1, &GNormalTex);
    glBindTexture(GL_TEXTURE_2D, GNormalTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, GNormalTex, 0); //attach
    

    // - Specify attahment to draw to
    size_t Attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, Attachments); 

    // unsigned int rboDepth;
    // glGenRenderbuffers(1, &rboDepth);
    // glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
	    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }   

    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    glBindTexture(GL_TEXTURE_2D, 0);
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
}



