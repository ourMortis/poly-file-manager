#ifndef __POLY_FILE_MANAGER_ERROR__
#define  __POLY_FILE_MANAGER_ERROR__

#include <string>


enum class PolyFileManagerError
{
    OK = 0,
    PathNotAbsolute,
    PathNotExistsInSystem,
    PathAlreadyAdded,
    PathNotToFile,
    PermissionDenied
};

std::string ecode2string(PolyFileManagerError error_code);

#endif //__POLY_FILE_MANAGER_ERROR__