#pragma once
#include <iostream>



class QuadBuffer
{

    uint32_t VAO{};
    uint32_t VBO{};
    uint32_t EBO{};

    uint32_t EBO_Count{};
    bool bPreparedBuffer = false;

private:
    friend void PrepareQuadBuffer(QuadBuffer& OutData);

public:

    void Prepare();
    void BindVAO() const;
    
    const uint32_t& GetVAO()const{ return VAO;}
    const uint32_t& GetVBO()const{ return VBO;}
    const uint32_t& GetEBO()const{ return EBO;}
    const uint32_t& Get_EBO_Count()const{ return EBO_Count;}

    ~QuadBuffer();
};

void PrepareQuadBuffer(QuadBuffer& OutData);

//when binding, 'GPositionTex' will be 'GL_COLOR_ATTACHMENT0', 'GNormalTex' is 'GL_COLOR_ATTACHMENT1'
void PrepareFramebufferWithTwoTextureAttachments(size_t& GFramebuffer, size_t& GPositionTex, size_t& GNormalTex);
