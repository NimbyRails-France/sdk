#define _GNU_SOURCE
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
    errno = 0;
    assert(fopen("/", "rb") == NULL && errno == EISDIR);
    errno = 0;
    assert(fopen64("/", "rb") == NULL && errno == EISDIR);
    char path[] = "/tmp/nrf-file-guard-XXXXXX";
    int fd = mkstemp(path);
    assert(fd >= 0);
    assert(write(fd, "texture", 7) == 7);
    close(fd);
    FILE *file = fopen(path, "rb");
    assert(file);
    char content[8] = {0};
    assert(fread(content, 1, 7, file) == 7);
    assert(strcmp(content, "texture") == 0);
    fclose(file);
    file = fopen64(path, "ab");
    assert(file && fwrite("!", 1, 1, file) == 1);
    fclose(file);
    assert(unlink(path) == 0);
    errno = 0;
    assert(fopen(path, "rb") == NULL && errno == ENOENT);
    puts("directory read guard: passed");
}
