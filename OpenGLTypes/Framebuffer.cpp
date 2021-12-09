
#include  "Framebuffer.h"
#include <iostream>

//You'll notice i use 'map::at(const key_type& k)' to get a texture attachment and not 'map::operator[](const key_type& k)'
//that's because the latter inserts a new element with that key and returns a reference to its mapped value
//if 'k' doesn't exist while the former throws an 'out of range' exception is 'k' doesn't exist

Framebuffer::Framebuffer(
    const std::vector<const char*>& AttachmentNames, 
    const FTexImage& _TexImage2dParams, 
    const FTextureParameters& _TextureParameters
) 
{
    glGenFramebuffers(1, &BufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, BufferID);

    for(unsigned int i = 0; i < AttachmentNames.size(); i++)
    {
        GLuint TextureID; 

        glGenTextures(1, &TextureID);
        glBindTexture(_TexImage2dParams.TargetTextureType, TextureID);

        glTexImage2D(
            _TexImage2dParams.TargetTextureType, 
            _TexImage2dParams.LOD, 
            _TexImage2dParams.InternalFormat, 
            _TexImage2dParams.Width, 
            _TexImage2dParams.Height,
            _TexImage2dParams.Border, 
            _TexImage2dParams.Format, 
            _TexImage2dParams.DataType, 
            _TexImage2dParams.Data
        );

        //set texture params
        Texture::StaticSetParameters(
             TextureID, 
            _TexImage2dParams.TargetTextureType, 
            _TextureParameters
        );

        //attach to framebuffer
        glBindTexture(GL_TEXTURE_2D, TextureID);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, _TexImage2dParams.TargetTextureType, TextureID, 0);

        //add to map
        TextureAttachments.insert
        ( 
            {
                AttachmentNames[i],
                {TextureID, GL_COLOR_ATTACHMENT0+i} 
            }
        );

    }    

    TexImage2dParams  = _TexImage2dParams;
    TextureParameters = _TextureParameters;

    if(!IsComplete())
    {
	    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    glBindTexture(GL_TEXTURE_2D, 0);
}

const GLuint& Framebuffer::GetID() const
{
    return BufferID;
}

void Framebuffer::SelectAttachmentsToDrawTo(std::function< bool ( const std::string& ) > Predicate) 
{
    if(!IsComplete()) 
    {
        std::cerr << "LOG::SelectAttachmentsToDrawTo()::Framebuffer is incomplete \n...exiting";
        return;
    }

    
    std::vector<GLuint> DrawableAttachments;
    DrawableAttachments.reserve(TextureAttachments.size());

    std::cout << "\n";
    for (const auto &Tex : TextureAttachments)
    {
        if( Predicate(Tex.first) ) //{ AttachmentName, {TextureID, Framebuffer Attachment point} }
        {
            DrawableAttachments.emplace_back(Tex.second.second);
            std::cout << "Selected attachment'" << Tex.first << "'\t Index / output /layout location = " << DrawableAttachments.size()-1 << "\n";
        }
    }
    std::cout << "\n";


    //Whaever order / index these attachments were placed in the array, those are the same indices to
    //dump appropriate content in your shader.
    //Ex.1 [ COLOR_ATTACHMENT_1 , COLOR_ATTACHMENT_0, COLOR_ATTACHMENT_3 ] are in layout location  [0, 1, 2]
    //Ex.2 [ COLOR_ATTACHMENT_3 , COLOR_ATTACHMENT_2, COLOR_ATTACHMENT_1 ] are in layout location  [0, 1, 2]

    glBindFramebuffer(GL_FRAMEBUFFER, BufferID);
    glDrawBuffers(DrawableAttachments.size(), DrawableAttachments.data()); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::SelectAttachmentsToDrawTo( const std::vector<const char*>& AttachmentNames ) 
{
    std::vector<GLuint> DrawableAttachments;
    DrawableAttachments.reserve(TextureAttachments.size());

    std::cout << "\n";

    for ( auto &Name : AttachmentNames)
    {
        try
        {
            DrawableAttachments.emplace_back( TextureAttachments.at(Name).second );
            std::cout << "Selected attachment'" << Name << "'\t Index / output /layout location = " << DrawableAttachments.size()-1 << "\n";

        }
        catch(const std::exception& e)
        {
            std::cerr << "\nException::" << e.what() << "\t'" << Name << "' doesn't exist\n";
        }
    }
    
    std::cout << "\n";

    glBindFramebuffer(GL_FRAMEBUFFER, BufferID);
    glDrawBuffers(DrawableAttachments.size(), DrawableAttachments.data()); 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool Framebuffer::IsComplete() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, BufferID);
    bool bIsComplete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return  bIsComplete;
}

GLuint Framebuffer::GetTextureAttachmentID(const char* Name) const
{
    try
    {
        return TextureAttachments.at(Name).first;
    }
    catch(const std::exception& e) //catch out of bound exception if element doesn't exist
    {
        std::cerr << "\nException::" << e.what() << "\t'" << Name << "' doesn't exist\n";
        return -1;
    }
}

Framebuffer::~Framebuffer() 
{
    std::cout << "\nCustom FBO and all it's attachments deleted\n";
    glDeleteFramebuffers(1, &BufferID);

    auto It = TextureAttachments.cbegin();
    
    while(It != TextureAttachments.cend())
    {
        glDeleteTextures(1, &It->second.first);
        It++;
    }
    
    Framebuffer::RemoveFramebufferFromWindowResizeCallback(this);
}

void Framebuffer::WindowResize(int NewWidth, int NewHeight)
{
    auto It = TextureAttachments.cbegin();

    //Why doesn't my framebuffer texture attachments automatically scale with window resolution?
    //https://stackoverflow.com/questions/23362497/how-can-i-resize-existing-texture-attachments-at-my-framebuffer
    
    while(It != TextureAttachments.cend())
    {
        
        glBindTexture(TexImage2dParams.TargetTextureType,  (It->second.first));
        glTexImage2D(
            TexImage2dParams.TargetTextureType, 
            TexImage2dParams.LOD, 
            TexImage2dParams.InternalFormat, 
            NewWidth, 
            NewHeight,
            TexImage2dParams.Border, 
            TexImage2dParams.Format, 
            TexImage2dParams.DataType, 
            TexImage2dParams.Data

        );

            // glBindTexture(GL_TEXTURE_2D,   (It->second.first));
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, NewWidth, NewHeight, 0, GL_RGBA, GL_FLOAT, NULL);

        std::cout << "LOG::Framebuffer::WindowResize:: Resized '" << It->first << "' from " << "[ " << TexImage2dParams.Width << " x " << TexImage2dParams.Height << " ]"
        << "to [ " << NewWidth << " x " << NewHeight << " ] \n";

        It++;
    }

    TexImage2dParams.Width  = NewWidth;
    TexImage2dParams.Height = NewHeight;

    glBindTexture(TexImage2dParams.TargetTextureType,  0);
}

//statics
std::set<Framebuffer*> Framebuffer::FramebufferContainer;
void Framebuffer::RemoveFramebufferFromWindowResizeCallback(Framebuffer* FrameBufferToRemove) 
{
    FramebufferContainer.erase(FrameBufferToRemove);
}

void Framebuffer::AddFramebufferForWindowResizeCallback(Framebuffer* FrameBufferToAdd)
{
    FramebufferContainer.insert(FrameBufferToAdd);
}

void Framebuffer::OnWindowResize(int NewWidth, int NewHeight) 
{
    auto It = FramebufferContainer.begin();
    while(It != FramebufferContainer.end())
    {
        if( *It )
        {
            (*It)->WindowResize(NewWidth, NewHeight);
        }
        It++;
    }
}

