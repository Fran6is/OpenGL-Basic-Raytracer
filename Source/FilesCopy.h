#pragma once
#include <iostream>
#include <vector>

//'DestinationFilePath' and 'SourceFilePath' should be relative to project root directory

bool CopyPasteFileSection(
    const std::string& DestinationFilePath,
    const std::string& SourceFilePath, 
    std::pair<std::string, std::string> SourceCopyTags, 
    const std::string& DestinationPasteTag, 
    bool bTagDestFileWithUPDATEDwhenDone = true,
    bool bRemovePasteTagFromDest = true
    );


