#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <string>

enum class SaveDataType
{
    String,
    Float,
    Vector3
};

struct SaveBase
{
private:
    bool GetFloat(const char* SlotName, const char* VariableName,   float& Out_Float) const;
    bool GetVector3(const char* SlotName, const char* VariableName, glm::vec3& Out_Vector) const;
    bool GetString(const char* SlotName, const char* VariableName,  std::string& Out_String) const;
//What if we learn more about regular expressions and use it to easily parse the file
//that way we can easily determine what data type exist on each line

//regex to filter for type scalar, vector3, and string
// variableName = 10
// variableName = (111 222 333) or (111, 222, 333)
// variableName = "some string"

//and 'variableName' must be 1 word not two or more like 'variable Name'


//Idea:
//what if you load and save to a cache; like a set
//Let the child class call load to initialize the cache with data
//then on subsequent requests just read from the cache

//let the cache be the interface between the actual file and the application

//Q: The problem might that may arise is when saving the cache data to a file. It has to be in the
//order they initially existed on the file
//when initially reading the file use a variable (like i) to keep track. 
//If a data was successfully read increment i and store that as it's index on the file

//Solution: when reading each data from the file, store each variable in a struct like so
/*
SaveItem{ 
    FileLineNumber, 
    pair("key", value),
    DataType
};
*/
//Then when saving back to a file create a vector of save items equal to item count in cache
//loop through the cache getting the line number of each save item and then inserting them 
//in the array at that index. Then finally writing it the file

//Q: How do i determine the data type of each content?
//Solution:
//1. well can iss read it without failing, does it have a begining and an ending quotation mark (for a string)
//   can iss read exactly 3 floats without failing
//2. Time to review REGEX (easier solution)



public:
    virtual bool Save(const char* SlotName) = 0;
    virtual bool Load(const char* SlotName) = 0;
    virtual ~SaveBase(){};
};

