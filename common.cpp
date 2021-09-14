#include "common.h"





void memDump(const uint8_t *array, uint32_t size)
{
#if 1
    uint32_t i;
    char *str = (char *)calloc(size * 3 + 1, 1);
    if(!str) {
        kLOG_ERROR("calloc ...");
        return;
    }
    for (i = 0; i < size - 1; i ++) {
        sprintf(&str[i * 3], "%02X ", array[i]);
    }
    sprintf(&str[i * 3], "%02X", array[size - 1]);
    kLOG_DEBUG("Dump --> [%s]\r\n", str);
    free(str);

#else

    int i;
    char str[1050]={0};

    for (i = 0; i < size - 1; i ++) {
        sprintf(&str[i * 3], "%02X ", array[i]);
    }
    sprintf(&str[i * 3], "%02X", array[size - 1]);
    kLOG_DEBUG("Dump --> [%s]\r\n", str);
#endif
}
