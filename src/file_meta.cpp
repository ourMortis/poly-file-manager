#include "../include/file_meta.h"

FileMeta::FileMeta(std::filesystem::path path)
{
    file_path = path;
}

PolyFileManagerError FileMeta::isValidPath(std::filesystem::path file_path)
{
    if(!file_path.is_absolute())
    {
        return PolyFileManagerError::PathNotAbsolute;
    }
    if(!std::filesystem::exists(file_path))
    {
        return PolyFileManagerError::PathNotExistsInSystem;
    }
    if(!std::filesystem::is_regular_file(file_path))
    {
        return PolyFileManagerError::PathNotToFile;
    }

    return PolyFileManagerError::OK;
}

void FileMeta::changeFilePath(std::filesystem::path new_path)
{
    if(isValidPath(new_path) == PolyFileManagerError::OK)
    {
        file_path = new_path;
    }
}

int FileMeta::getTagCount()
{
    return tags.size();
}
bool FileMeta::hasTag(std::string &tag)
{
    return tags.find(tag) != tags.end() ? true : false;
}
void FileMeta::insertTag(std::string &tag)
{
    if(hasTag(tag))
    {
        tags.insert(tag);
    }
}

void FileMeta::removeTag(std::string &tag)
{
    if(hasTag(tag))
    {
        tags.erase(tag);
    }
}

