#ifndef __FILE_META_H__
#define __FILE_META_H__

#include <string>
#include <set>
#include <filesystem>
#include "poly_file_manager_error.h"


class FileMeta
{
private:
    std::filesystem::path file_path;
    std::set<std::string> tags;

public:
    FileMeta(std::filesystem::path file_path);

    static PolyFileManagerError isValidPath(std::filesystem::path file_path); //Is absolute path to existing file?

    void setFilePath(std::filesystem::path file_path);

    int getTagCount();

    std::set<std::string> getTags();

    bool hasTag(std::string tag);

    void insertTag(std::string tag);
    
    void removeTag(std::string tag);

    
};

typedef FileMeta *soft_link;

#endif //__FILE_META_H__