#ifndef __POLY_FILE_MANAGER_ERROR__
#define  __POLY_FILE_MANAGER_ERROR__

enum class PolyFileManagerError
{
    OK = 0,
    PathNotAbsolute,
    PathNotExistsInSystem,
    PathAlreadyAdded,
    PathNotToFile,
    PermissionDenied
};

#endif //__POLY_FILE_MANAGER_ERROR__