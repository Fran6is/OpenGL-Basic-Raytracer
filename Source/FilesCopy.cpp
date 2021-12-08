#include <FilesCopy.h>
#include <fstream>
#include <sstream>
#include <Path.h>
#include <vector>

#define EXPECTED_TAG_INDEX_IN_STRING 0

bool CopyPasteFileSection(
    const std::string& DestinationFilePath,
    const std::string& SourceFilePath, 
    std::pair<std::string, std::string> SourceCopyTags, 
    const std::string& DestinationPasteTag, 
    bool bTagDestFileWithUPDATEDwhenDone,
    bool bRemovePasteTagFromDest
    )
{

    //Destination file
    std::fstream DestinationFile{ GetFullPath(DestinationFilePath) };

    if (!DestinationFile.is_open())
    {
        std::cerr << "LOG::CopyPasteFileSection()::Unable to open or find destination file '" 
        << DestinationFilePath << "'. Creating a new file\n";

        DestinationFile.open(GetFullPath(DestinationFilePath), std::ios::out);
        DestinationFile << DestinationPasteTag;
        DestinationFile.close();
        DestinationFile.open( GetFullPath(DestinationFilePath) );
    }

    //Source file
    std::ifstream SourceFile{ GetFullPath(SourceFilePath) };
    if (!SourceFile.is_open())
    {
        std::cerr << "LOG::CopyPasteFileSection()::Unable to open or find source file '" << SourceFilePath << "'\n";

        DestinationFile.close();
        SourceFile.close();
    }

    if( !DestinationFile.is_open() || !SourceFile.is_open() )
    {
        DestinationFile.close();
        SourceFile.close();
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

    if( CurrentLine_Dest.find(DestinationPasteTag) == std::string::npos )
    {
        DestinationFileContent.emplace_back( bTagDestFileWithUPDATEDwhenDone ? "//UPDATED" : CurrentLine_Dest  );
    }
    
    bool bFoundFirstPasteTag = false;
    bool bAlreadyCopiedPasteContent = false;

    size_t Index_PasteContent_Start = 0;
    size_t Index_PasteContent_End   = 0;

    size_t PasteTagCount = 0;
    
    
    DestinationFile.seekg(0, std::ios::beg); //go back to the beginning of the file after you've determined the first line content not to be "//UPDATED" tag
    

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

            bool bTheresAnOpenStartTag = false;

            std::string CurrentLine_Source;
            std::vector<std::string>* TmpBuffer = nullptr; 

            //we must be able to copy at least one full section enclosed within the copy tags
            //for 'bAlreadyCopiedPasteContent' to be true
            int SuccessfullCopies = 0; 
            while ( std::getline( SourceFile, CurrentLine_Source) )
            {      
                bool bJustFoundAStartTag = CurrentLine_Source.find(SourceCopyTags.first)  == EXPECTED_TAG_INDEX_IN_STRING;
                bool bJustFoundAnEndTag  = CurrentLine_Source.find(SourceCopyTags.second) == EXPECTED_TAG_INDEX_IN_STRING;
                
                if( !bTheresAnOpenStartTag && bJustFoundAStartTag  )
                {
                    std::cout << "Found copy start tag '" << SourceCopyTags.first << "' in source file '" 
                            << SourceFilePath << "' pos = " << CurrentLine_Source.find(SourceCopyTags.first) << "\n";;

                    bTheresAnOpenStartTag = true;
                    TmpBuffer = new std::vector<std::string>();

                    continue; //don't include the tag 
                }

                if (bTheresAnOpenStartTag)
                {
                    Index_PasteContent_Start = Index_PasteContent_Start < 0 ? DestinationFileContent.size() - 1 : Index_PasteContent_Start;

                    //If we found another start tag while the last one hasn't been closed by an end tag
                    //delete the temporary content we were collecting
                    if( bJustFoundAStartTag ) 
                    {
                        delete TmpBuffer;
                        TmpBuffer = new std::vector<std::string>();
                    }
                    else if( bJustFoundAnEndTag ) //empty the temporary buffer's content into destination file buffer
                    {
                        std::cout << "Found copy end tag '" << SourceCopyTags.second << "' in source file '" 
                            << SourceFilePath << "' pos = " << CurrentLine_Source.find(SourceCopyTags.second) << "\n";;


                        for (const auto &Content : *TmpBuffer)
                        {
                            DestinationFileContent.emplace_back(Content);
                        }

                        Index_PasteContent_End = DestinationFileContent.size();

                        delete TmpBuffer;
                        TmpBuffer = nullptr;

                        bTheresAnOpenStartTag = false;
                        SuccessfullCopies += 1;
                    }
                    else //temporary store file content until a potential end tag is encountered!
                    {
                        TmpBuffer->emplace_back(CurrentLine_Source);
                    }  
                }
            }
            delete TmpBuffer; //delete buffer content, if any

            if(SuccessfullCopies > 0)
            {
                bAlreadyCopiedPasteContent = true;
                continue;
            }
            else
            {
                std::cout << "No start copy tag found in file! \n";

                DestinationFile.close();
                SourceFile.close();

                return false;
            }
            
            std::cout << "Successful copies count = " << SuccessfullCopies   << "\n";
            std::cout << "Any unclosed start tag? = " << bTheresAnOpenStartTag << "\n";
            
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

