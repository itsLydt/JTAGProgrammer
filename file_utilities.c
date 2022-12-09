#include <file_utilities.h>

bool dirExists(char* dirName){
    DIR* dir = opendir(dirName);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
        return true;
    } else if (ENOENT == errno) {
        /* Directory does not exist. */
        return false;
    } else {
        /* opendir() failed for some other reason. */
        printf("opendir failed with error code %d\r\n", errno);
        return false;
    }
}