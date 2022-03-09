#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"

int main()
{
    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i < 30; i++) {
        lseek(fd, i, SEEK_SET);
        long long sz = write(fd, NULL, 0);
        printf("calculate f(%d) cost %lld\n", i, sz);
    }

    close(fd);
    return 0;
}
