#pragma once

#include <stdint.h>
#include <stddef.h>

#include "global.h"
#include "ff.h"

const size_t c_fileNameLength = MAX_LENGTH;

class DirectoryListing {
  public:
    enum Enum { IDLE, GETDIRECTORY };

    struct Status {
        uint8_t currentState;
        uint16_t totalItems;
        char cwd[c_fileNameLength];
    };

    void init();
    int getNumberofFileEntries(const char *dir);
    void readDirectoryToBuffer(void *buffer, const char *path, const size_t offset, const unsigned int bufferSize = 2324);
    void setDirectory(const char *dir);
    
  private:
    int m_fileCount = -1;
    char m_cwdbuf[c_fileNameLength] = "/";

    Status m_status;
};