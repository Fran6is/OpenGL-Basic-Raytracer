#pragma once
#include <iostream>
#include <vector>

//'DestinationFilePath' and 'SourceFilePath' should be relative to project root directory
bool CopyPasteFileSection(
    const char* DestinationFilePath, 
    const char* SourceFilePath, 
    std::pair<const char*, const char*> SourceCopyTags, 
    const char* DestinationPasteTag, 
    bool bTagDestFileWithUPDATEDwhenDone = true,
    bool bRemovePasteTagFromDest = true
    );


