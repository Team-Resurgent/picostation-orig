#include "directory_listing.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#include "f_util.h"
#include "ff.h"

#if DEBUG_I2S
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) while (0)
#endif

namespace picostation {

DirectoryListing::PathItem DirectoryListing::createPathItem(const char* path) {
    uint32_t len = strlen(path);
    if (len > c_maxFilePathLength) {
        len = c_maxFilePathLength;
    }
    PathItem pathItem;
    memset(&pathItem, 0, sizeof(pathItem));
    memcpy(pathItem.path, path, len);
    pathItem.path[len] = '\0';
    return pathItem;
}

void DirectoryListing::getExtension(const PathItem& filePath, PathItem& extension) {
    const char* last_dot = strrchr(filePath.path, '.');
    if (last_dot != nullptr && last_dot != filePath.path) {
        size_t ext_len = strlen(filePath.path) - (last_dot - filePath.path);
        memcpy(extension.path, last_dot, ext_len);
        extension.path[ext_len] = '\0';
    } else {
        extension.path[0] = '\0';
    }
}

void DirectoryListing::getPathWithoutExtension(const PathItem& filePath, PathItem& newPath) {
    PathItem extension;
    memset(&extension, 0, sizeof(extension));
    DirectoryListing::getExtension(filePath, extension);
    size_t extension_pos = strlen(filePath.path) - strlen(extension.path);
    if (extension_pos > 0) {
        memcpy(newPath.path, filePath.path, extension_pos);
        newPath.path[extension_pos] = '\0';
    } else {
        newPath.path[0] = '\0';
    }
}

bool DirectoryListing::pathContainsFilter(const PathItem& filePath, const char* filter) {
    if (strlen(filter) == 0) {
        return true;
    }
    PathItem pathWithoutExtension;
    memset(&pathWithoutExtension, 0, sizeof(pathWithoutExtension));
    getPathWithoutExtension(filePath, pathWithoutExtension);
    return strstr(pathWithoutExtension.path, filter) != nullptr;
}

bool DirectoryListing::getDirectoryEntries(const PathItem& filePath, const char* filter, const uint32_t page, DirectoryDetails& directoryDetails) {
    DIR dir;
    FILINFO currentEntry;
    FILINFO nextEntry;
    FRESULT res = f_opendir(&dir, filePath.path);
    if (res != FR_OK) {
        DEBUG_PRINT("f_opendir error: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
    }

    uint32_t filesProcessed = 0;
    directoryDetails.hasNext = 1;
    directoryDetails.fileEntryCount = 0;

    res = f_readdir(&dir, &currentEntry);
    if (res == FR_OK && currentEntry.fname[0] != '\0') {
        res = f_readdir(&dir, &nextEntry);
        bool hasNext = (res == FR_OK && nextEntry.fname[0] != '\0');
        while (true) {
            if (directoryDetails.fileEntryCount == c_maxFileEntriesPerPage) {
                break;
            }
            if (!(currentEntry.fattrib & AM_HID)) {
                PathItem pathItem = createPathItem(currentEntry.fname);
                if (pathContainsFilter(pathItem, filter)) {
                    if ((filesProcessed / c_maxFileEntriesPerPage) >= page) {
                        directoryDetails.fileEntries[directoryDetails.fileEntryCount].isDirectory = currentEntry.fattrib & AM_DIR ? 1 : 0;
                        directoryDetails.fileEntries[directoryDetails.fileEntryCount].filePath = pathItem;
                        directoryDetails.fileEntryCount++;
                    }
                    filesProcessed++;
                }
            }
            if (!hasNext) {
                directoryDetails.hasNext = 0;
                break;
            }
            currentEntry = nextEntry;
            res = f_readdir(&dir, &nextEntry);
            hasNext = (res == FR_OK && nextEntry.fname[0] != '\0');
        }
    }
    f_closedir(&dir);
    return true;
}

}  // namespace picostation