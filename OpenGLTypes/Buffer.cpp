
#include <Buffer.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

ObjectBuffer::~ObjectBuffer() 
{
    std::cout << "Quad buffer deleted! \n";
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void ObjectBuffer::Prepare() 
{
    if(!bPreparedBuffer) 
        PrepareQuadBuffer(*this);
}

void ObjectBuffer::BindVAO() const
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

static void PrepareQuadBuffer(ObjectBuffer& OutData)
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

void PrepareFramebufferWithTwoTextureAttachment(uint32_t& FramebufferID, uint32_t& Texture1, uint32_t& Texture2) 
{
    
}



