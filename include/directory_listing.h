#pragma once

#include <stddef.h>
#include <stdint.h>

#include <vector>

#include "ff.h"
#include "global.h"

namespace picostation {
class DirectoryListing {
  public:
    enum Enum { IDLE, GETDIRECTORY };

    struct PathItem {
        char path[c_maxFilePathLength + 1];
    };

    struct FileEntry {
        uint8_t isDirectory;
        PathItem filePath;
    };

    struct DirectoryDetails {
        uint8_t hasNext;
        uint16_t fileEntryCount;
        FileEntry fileEntries[c_maxFileEntriesPerSector];
    };

    static PathItem createPathItem(const char* path);
    static void getExtension(const PathItem& filePath, PathItem& extension);
    static void getPathWithoutExtension(const PathItem& filePath, PathItem& newPath);
    static bool pathContainsFilter(const PathItem& filePath, const char* filter);
    static bool getDirectoryEntries(const PathItem& filePath, const char* filter, const uint32_t page, DirectoryDetails& directoryDetails);
    
    // void setDirectory(const char *dir);
};
}  // namespace picostation