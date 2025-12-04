#ifndef SERIALIZER_H
#define SERIALIZER_H

#include<string>
#include<filesystem>

using FilePath = std::filesystem::path;

class Serializer
{
private:
    static constexpr char* config_file = ".poly_file_manager";
    
public:
};

#endif // SERIALIZER_H
