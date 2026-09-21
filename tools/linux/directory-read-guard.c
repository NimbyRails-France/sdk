/* Optional launcher compatibility for NIMBY's Linux texture reader.
 * Reading a directory through std::ifstream can produce LONG_MAX from tellg;
 * the game then attempts that allocation. Keep this outside the SDK ABI.
 * Enable only for the game process with LD_PRELOAD, never system-wide.
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

typedef FILE *(*open_file)(const char *, const char *);

static FILE *guarded_open(const char *symbol, const char *path, const char *mode) {
    open_file original = (open_file)dlsym(RTLD_NEXT, symbol);
    if (!original) { errno = ENOSYS; return NULL; }
    FILE *file = original(path, mode);
    if (file && mode[0] == 'r' && !strchr(mode, '+')) {
        struct stat info;
        if (fstat(fileno(file), &info) == 0 && S_ISDIR(info.st_mode)) {
            fclose(file);
            errno = EISDIR;
            return NULL;
        }
    }
    return file;
}

FILE *fopen(const char *path, const char *mode) { return guarded_open("fopen", path, mode); }
FILE *fopen64(const char *path, const char *mode) { return guarded_open("fopen64", path, mode); }
