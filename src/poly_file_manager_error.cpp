#include "poly_file_manager_error.h"

std::string ecode2string(PolyFileManagerError error_code)
{
    std::string str;
    switch (error_code)
    {
    case PolyFileManagerError::OK:
        str = "OK";
        break;
    case PolyFileManagerError::PathNotAbsolute:
        str = "Path is not absolute";
        break;
    case PolyFileManagerError::PathNotExistsInSystem:
        str = "Path not exists in system";
        break;
    case PolyFileManagerError::PathAlreadyAdded:
        str = "Path already added";
        break;
    case PolyFileManagerError::PathNotToFile:
        str = "Path not points to file";
        break;
    case PolyFileManagerError::PermissionDenied:
        str = "Permission denied";
        break;
    default:
        str = "Case not exists";
        break;
    }
    return str+'\n';
}