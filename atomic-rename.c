#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

int
posixy_open(const char *filename, int flags)
{
    /* Translate POSIX read mode to Windows access mode */
    DWORD dwDesiredAccess = GENERIC_READ;
    switch (flags & (O_RDONLY | O_RDWR | O_WRONLY)) {
        case O_RDONLY:
            dwDesiredAccess = GENERIC_READ;
            break;
        case O_RDWR:
            dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
            break;
        case O_WRONLY:
            dwDesiredAccess = GENERIC_WRITE;
            break;
    }
    /* Subsequent operations may open this file to read, write or delete it */
    DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;

    /* Translate POSIX creation mode to Windows creation mode */
    DWORD dwCreationDisposition = OPEN_EXISTING;
    switch (flags & (O_CREAT | O_TRUNC | O_EXCL)) {
        case O_EXCL:
            dwCreationDisposition = OPEN_EXISTING;
            break;

        case O_CREAT:
            dwCreationDisposition = OPEN_ALWAYS;
            break;

        case O_CREAT | O_TRUNC:
            dwCreationDisposition = CREATE_ALWAYS;
            break;

        case O_CREAT | O_EXCL:
        case O_CREAT | O_TRUNC | O_EXCL:
            dwCreationDisposition = CREATE_NEW;
            break;

        case O_TRUNC:
        case O_TRUNC | O_EXCL:
            dwCreationDisposition = TRUNCATE_EXISTING;
            break;
    }

    HANDLE handleWin =
        CreateFileA(filename,
                    dwDesiredAccess,
                    dwShareMode,
                    NULL,
                    dwCreationDisposition,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);
    if (handleWin == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "CreateFileA failed (0x%08lx)\n", GetLastError());
        return -1;
    }

    /* Return a standard file descriptor. */
    return _open_osfhandle((intptr_t)handleWin, flags|O_BINARY);
}

int
posixy_rename(const char *from, const char *to)
{
    if (MoveFileEx(from, to, MOVEFILE_REPLACE_EXISTING) != 0) {
        return 0;
    }

    fprintf(stderr, "MoveFileEx failed (0x%08lx)\n", GetLastError());
    return -1;
}

int main() {
    int fd = posixy_open("deletetest", O_WRONLY|O_CREAT);
    if (fd < 0) exit(1);

    int fdr = posixy_open("README.rst", O_RDONLY);
    if (fdr < 0) exit(1);

    if (posixy_rename("atomic-rename.c", "deletetest") < 0) {
        perror("not ok\trename to replace deletetest");
    } else {
        fprintf(stderr, "ok\trename to replace deletetest\n");
    }

    if (posixy_rename("testsigpipe.c", "README.rst") < 0) {
        perror("not ok\trename to replace README.rst");
    } else {
        fprintf(stderr, "ok\trename to replace README.rst\n");
    }

    Sleep(60000);
    int r = _write(fd, "test", 4);
    if (r < 0) {
        perror("_write failed");
        fprintf(stderr, "GetLastError() -> 0x%08lx\n", GetLastError());
        return 1;
    }
    fprintf(stderr, "_write returned %d\n", r);

    char buf[1024];
    r = _read(fdr, buf, sizeof(buf) - 1);
    if (r < 0) {
        perror("_read failed");
        fprintf(stderr, "GetLastError() -> 0x%08lx\n", GetLastError());
        return 1;
    }
    buf[r] = '\0';
    fprintf(stderr, "_read returned %d [%s]\n", r, buf);
    return 0;
}
