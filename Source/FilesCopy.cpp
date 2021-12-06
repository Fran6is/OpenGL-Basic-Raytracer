#include <FilesCopy.h>
#include <fstream>
#include <sstream>
#include <Path.h>
#include <vector>

#define EXPECTED_TAG_INDEX_IN_STRING 0

bool CopyPasteFileSection(const char* DestinationFilePath, 
    const char* SourceFilePath, 
    std::pair<const char*, const char*> SourceCopyTags, 
    const char* DestinationPasteTag, 
    bool bTagDestFileWithUPDATEDwhenDone,
    bool bRemovePasteTagFromDest) 
{
    if( !(DestinationFilePath && SourceFilePath && DestinationPasteTag && SourceCopyTags.first && SourceCopyTags.second) )
    {
        std::cerr << "LOG::CopyPasteFileSection()::One or more of function's 'const char*' argument is null. Exiting... \n";
        return false;
    }


    //Destination file
    std::fstream DestinationFile{ GetFullPath(DestinationFilePath) };
    if (!DestinationFile.is_open())
    {
        std::cerr << "LOG::CopyPasteFileSection()::Unable to open or find destination file '" << DestinationFilePath << "'\n";
    }

    //Source file
    std::ifstream SourceFile{ GetFullPath(SourceFilePath) };
    if (!SourceFile.is_open())
    {
        std::cerr << "LOG::CopyPasteFileSection()::Unable to open or find source file '" << SourceFilePath << "'\n";
    }

    if( !DestinationFile.is_open() || !SourceFile.is_open() )
    {
        return false;
    } 

    std::string CurrentLine_Dest;

    std::getline( DestinationFile, CurrentLine_Dest);
    if( CurrentLine_Dest.find("//UPDATED") == EXPECTED_TAG_INDEX_IN_STRING ) 
    { 
        std::cerr << "CopyPasteFileSection::'//UPDATED' tag found at top of destination file '" << DestinationFilePath << "'. exiting\n";
        DestinationFile.close(); 
        SourceFile.close();  
        return false; 
    }

    
    std::vector<std::string> DestinationFileContent;
    DestinationFileContent.reserve(50);
    DestinationFileContent.emplace_back( bTagDestFileWithUPDATEDwhenDone ? "//DONT" : CurrentLine_Dest  );
    
    bool bFoundFirstPasteTag = false;
    bool bAlreadyCopiedPasteContent = false;

    size_t Index_PasteContent_Start = 0;
    size_t Index_PasteContent_End   = 0;

    size_t PasteTagCount = 0;
    
    DestinationFile.seekg(0, std::ios::beg);
    while( std::getline( DestinationFile, CurrentLine_Dest) )
    {
        size_t FoundTagIndex = CurrentLine_Dest.find(DestinationPasteTag);
        PasteTagCount += (FoundTagIndex == EXPECTED_TAG_INDEX_IN_STRING) ? 1 : 0;

        //First time we encounter a paste tag. we copy the required section from source and add it to our destination array
        //And store the starting and ending index of the copied content; Then for subsequent paste tag encountered we use that 
        //those indices to paste
        if( !bFoundFirstPasteTag && (FoundTagIndex == EXPECTED_TAG_INDEX_IN_STRING)  ) 
        {
            std::cout << "Found first paste tag '" << DestinationPasteTag << "' in destination file '" << DestinationFilePath << "\n";
            bFoundFirstPasteTag = true;

            if(!bRemovePasteTagFromDest) //include paste tag in new file content
            {
                DestinationFileContent.emplace_back( CurrentLine_Dest );
            }

            bool bFoundCopyStartTag = false;
            bool bFoundCopyEndTag   = false;

            std::string CurrentLine_Source;
            while ( std::getline( SourceFile, CurrentLine_Source) )
            {      
                FoundTagIndex = CurrentLine_Source.find(SourceCopyTags.first);

                if( !bFoundCopyStartTag && (FoundTagIndex == EXPECTED_TAG_INDEX_IN_STRING)   )
                {
                    std::cout << "Found copy start tag '" << SourceCopyTags.first << "' in source file '" 
                            << SourceFilePath << "' pos = " << CurrentLine_Source.find(SourceCopyTags.first) << "\n";;

                    bFoundCopyStartTag = true;
                    Index_PasteContent_Start = DestinationFileContent.size();
                    continue;
                }

                if (bFoundCopyStartTag)
                {
                    FoundTagIndex = CurrentLine_Source.find(SourceCopyTags.second);
                    if( FoundTagIndex == EXPECTED_TAG_INDEX_IN_STRING )
                    {
                        std::cout << "Found copy end tag '" << SourceCopyTags.second << "' in source file '" 
                            << SourceFilePath << "' \t" << "pos = " << CurrentLine_Source.find(SourceCopyTags.second) << "\n";

                        bFoundCopyEndTag = true;
                        Index_PasteContent_End = DestinationFileContent.size();
                        break;
                    }

                    //std::cout << CurrentLine_Source << "\n";
                    DestinationFileContent.emplace_back(CurrentLine_Source);
                }
            }

            //if we found both tags (copy and end), then we successfully copied that section of the source file
            //if not, either one or both tags don't exist. So there's no point copying the remaining content from 
            //destination

            if(bFoundCopyStartTag && bFoundCopyEndTag)
            {
                std::cout << "Found both copy tags. Continuing with rest of file \n";
                bAlreadyCopiedPasteContent = true;
                continue;
            }
            else
            {
                DestinationFile.close();
                SourceFile.close();
                std::cerr << "LOG::CopyPasteFileSection:: source copy start tag '"<< SourceCopyTags.first << "' "
                     << "and / or end tag '" << SourceCopyTags.second  <<"' doesn't exist in '" << SourceFilePath << "'. Aborting operation. Destination file is unchanged \n";
                return false;
            }
            
        }
        else if( (FoundTagIndex == EXPECTED_TAG_INDEX_IN_STRING) && bAlreadyCopiedPasteContent )
        {
            if(!bRemovePasteTagFromDest) 
            {
                DestinationFileContent.emplace_back( CurrentLine_Dest );
            }

            for (size_t Index = Index_PasteContent_Start; Index < Index_PasteContent_End; Index++)
            {
                DestinationFileContent.emplace_back( DestinationFileContent.at(Index) );
            }
            
        }
        else
        {
            //std::cout << CurrentLine_Dest << "\n";
            DestinationFileContent.emplace_back(CurrentLine_Dest);
        }
    }

    //If we made it this line, then the paste tag was found and we have successfully replaced it with the grabbed section
    //from the source file in the destination file. Now we just need to replace the destination file with this content
    if(bFoundFirstPasteTag) // or check if greater than 0
    {
        SourceFile.close();
        DestinationFile.close();

        DestinationFile.open(GetFullPath(DestinationFilePath), std::ios::out);
        if(!DestinationFile.is_open())
        {
            std::cout << "LOG::CopyPasteFileSection()::In the final stage of pasting copied content back to destination file. But file is not open \n";
            return false;
        }

        for ( const auto &CurrentLine : DestinationFileContent)
        {
            //std::cout << CurrentLine << "\n";
            DestinationFile << CurrentLine << "\n";
        }
        DestinationFile.close();
        
        std::cout << "LOG::CopyPasteFileSection()::Successfully copied!. Total paste tags found = " << PasteTagCount << "\n";
        return true;
    }

    DestinationFile.close();
    SourceFile.close();
    std::cout << "LOG::CopyPasteFileSection()::No paste tag '" << DestinationPasteTag << "' found in '" << DestinationFilePath << "'. File is unchanged. \n";
    return false;
}

