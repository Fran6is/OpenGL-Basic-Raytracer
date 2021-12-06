#pragma once

#include <glad\glad.h>
#include <vector>
#include <set>
#include <map>
#include <string>

#include <Texture.h>
#include <functional>

class Framebuffer
{
private:
    GLuint BufferID = -1;
        //{ AttachmentName, {TextureID, Framebuffer Attachment point} }
    std::map< std::string, std::pair< GLuint, GLenum> > TextureAttachments;

    FTextureParameters TextureParameters;
    FTexImage          TexImage2dParams;
    
    void WindowResize(int NewWidth, int NewHeight);

public:
    Framebuffer(const std::vector<const char*>& AttachmentNames, const FTexImage& _TexImage2dParams, const FTextureParameters& _TextureParameters);
    Framebuffer(unsigned int AttachmentCount, unsigned int _Width, unsigned int _Height);
    ~Framebuffer();

    bool IsComplete() const;

    //If 'NAME' doesn't exist, the function returns the largest possible representable value of an 'unsigned int' or 'GLuint' which is same as assigning -1 to an unsigned int
    //or same value as the static constant 'std::string::npos'
    GLuint GetTextureAttachmentID(const char* Name) const;

    const size_t& GetAttachmentsCount() const { return TextureAttachments.size(); }

    const unsigned int& GetWidth() const { return TexImage2dParams.Width; }

    const unsigned int& GetHeight() const { return TexImage2dParams.Height; }
    
    const GLuint& GetID() const;

    void SelectAttachmentsToDrawTo( const std::vector<const char*>& AttachmentNames  );

    void SelectAttachmentsToDrawTo(  std::function< bool ( const std::string& ) > Predicate );

private:
    static std::set<Framebuffer*> FramebufferContainer;

public:
    static void RemoveFramebufferFromWindowResizeCallback(Framebuffer* FrameBufferToRemove);
    static void AddFramebufferForWindowResizeCallback(Framebuffer* FrameBufferToAdd);
    static void OnWindowResize(int NewWidth, int NewHeight);
};





