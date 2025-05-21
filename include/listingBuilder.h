#pragma once

#include <string.h>

#define LISTING_SIZE 2324 

class listingBuilder {
  public:
    listingBuilder() {
        mSize = 0;
    }

    bool addString(const char* value) {
        uint8_t pathLen = strnlen(value, 255);
        uint16_t sizeToAdd = 1 + pathLen;
        if ((mSize + sizeToAdd + 2) > LISTING_SIZE) {
            return false;
        }
        memcpy(mValuesContainer + mSize, &pathLen, 1);
        memcpy(mValuesContainer + mSize + 1, value, pathLen);
        mSize += sizeToAdd;
        return true;
    }

    bool addTerminator(uint8_t hasNext) {
        if ((mSize + 2) > LISTING_SIZE) {
            return false;
        }
        mValuesContainer[mSize] = 0;
        mValuesContainer[mSize + 1] = hasNext;
        mSize += 2;
        return true;
    }

    uint8_t* getData() { return mValuesContainer; }

    uint32_t size() { return mSize; }

    char* getString(uint16_t index)
    {
        static char result[256];

        uint16_t offset = 0;
        uint16_t currentPos = 0;
        while (offset < LISTING_SIZE)
        {
            if (currentPos == index)
            {
                uint16_t length =mValuesContainer[offset + 1];
                strncpy(result, (char*)&mValuesContainer[offset], length);
                result[length] = '\0';
                return result;
            }
            uint8_t strLen = mValuesContainer[offset];
            if (strLen == 0)
            {
                break;
            }
            offset += strLen + 1;
            currentPos++;
        }
        return nullptr;
    }

    void clear() {
        mSize = 0;
    }

  private:
    uint8_t mValuesContainer[LISTING_SIZE];
    uint32_t mSize;
};