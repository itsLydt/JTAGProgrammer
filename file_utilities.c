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

int getFileList(char* pattern, glob_t* pglob){
    glob(pattern, GLOB_ERR, NULL, pglob);      

    printf("Found %d matches\n", pglob->gl_pathc);
    if(pglob->gl_pathc > 0)
        printf("First match: %s\n", pglob->gl_pathv[0]);

    return pglob->gl_pathc;
}