#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

// SO_NOSIGPIPE: DragonflyBSD FreeBSD NetBSD macOS (11,12,14,15,26)
// not defined: Linux OpenBSD

// MSG_NOSIGNAL: DragonflyBSD FreeBSD NetBSD macOS (11,12,14,15,26) OpenBSD Linux

int main() {
    {
	int fds[2];
	if (socketpair(PF_UNIX, SOCK_STREAM, 0, fds) < 0) {
	    perror("not ok\tsocketpair");
	    return 1;
	}
	fprintf(stderr, "ok\tsocketpair\n");
	int fd = fds[0];
#ifdef SO_NOSIGPIPE
	int flag = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (char*)&flag, sizeof(flag)) < 0) {
	    perror("not ok\tsetsockopt SO_NOSIGPIPE");
	} else {
	    fprintf(stderr, "ok\tSet SO_NOSIGPIPE successfully\n");
	}
#endif
	if (send(fd, "x", 1, 0) < 0) {
	    perror("not ok\tsend 0");
	} else {
	    fprintf(stderr, "ok\tsend 0\n");
	}
#ifdef MSG_NOSIGNAL
	if (send(fd, "x", 1, MSG_NOSIGNAL) < 0) {
	    perror("not ok\tsend MSG_NOSIGNAL");
	} else {
	    fprintf(stderr, "ok\tsend MSG_NOSIGNAL\n");
	}
#endif
	close(fd);
	close(fds[1]);
    }

    {
	int fds[2];
	if (pipe(fds) < 0) {
	    perror("not ok\tpipe");
	    return 1;
	}
	fprintf(stderr, "ok\tpipe\n");
	int fd = fds[1];
#ifdef SO_NOSIGPIPE
	int flag = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (char*)&flag, sizeof(flag)) < 0) {
	    if (errno == ((fd == 3) ? EBADF : ENOTSOCK)) {
		fprintf(stderr, "ok\tSet SO_NOSIGPIPE on pipe fd %d failed with expected error: %s\n", fd, strerror(errno));
	    } else {
		fprintf(stderr, "not ok\tSet SO_NOSIGPIPE on pipe fd %d failed with unexpected error: %s\n", fd, strerror(errno));
	    }
	} else {
	    fprintf(stderr, "not ok\tSet SO_NOSIGPIPE on pipe fd %d unexpectedly successful\n", fd);
	}
#endif
	if (send(fd, "x", 1, 0) < 0) {
	    if (errno == ((fd == 3) ? EBADF : ENOTSOCK)) {
		fprintf(stderr, "ok\tsend 0 on pipe fd %d failed with expected error: %s\n", fd, strerror(errno));
	    } else {
		fprintf(stderr, "not ok\tsend 0 on pipe fd %d failed with unexpected error: %s\n", fd, strerror(errno));
	    }
	} else {
	    fprintf(stderr, "not ok\tsend 0 on pipe fd %d unexpectedly successful\n", fd);
	}
#ifdef MSG_NOSIGNAL
	if (send(fd, "x", 1, MSG_NOSIGNAL) < 0) {
	    if (errno == ((fd == 3) ? EBADF : ENOTSOCK)) {
		fprintf(stderr, "ok\tsend MSG_NOSIGNAL on pipe fd %d failed with expected error: %s\n", fd, strerror(errno));
	    } else {
		fprintf(stderr, "not ok\tsend MSG_NOSIGNAL on pipe fd %d failed with unexpected error: %s\n", fd, strerror(errno));
	    }
	} else {
	    fprintf(stderr, "not ok\tsend MSG_NOSIGNAL on pipe fd %d unexpectedly successful\n", fd);
	}
#endif
	close(fd);
	close(fds[0]);
    }

    // Open fd 3 on a file.
    int fd = open("tmpfile", O_WRONLY|O_CREAT, 0666);
    if (fd < 0) {
	perror("not ok\topen tmpfile");
	return 1;
    }
    fprintf(stderr, "ok\topen tmpfile\n");
    if (fd == 3) {
	fprintf(stderr, "ok\topen returned fd 3\n");
    } else if (dup2(fd, 3) < 0) {
	perror("not ok\tdup2");
	return 1;
    } else {
	fprintf(stderr, "ok\tdup2\n");
    }

    close(4); // Ensure fd 4 not open.
    for (fd = 0; fd <= 4; ++fd) {
        static const char* fd_desc[] = { "stdin", "stdout", "stderr", "regular file", "closed fd" };
#ifdef SO_NOSIGPIPE
	int flag = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (char*)&flag, sizeof(flag)) < 0) {
	    if (errno == ((fd == 4) ? EBADF : ENOTSOCK)) {
		fprintf(stderr, "ok\tSet SO_NOSIGPIPE on %s failed with expected error: %s\n", fd_desc[fd], strerror(errno));
	    } else {
		fprintf(stderr, "not ok\tSet SO_NOSIGPIPE on %s failed with unexpected error: %s\n", fd_desc[fd], strerror(errno));
	    }
	} else {
	    fprintf(stderr, "not ok\tSet SO_NOSIGPIPE on %s unexpectedly successful\n", fd_desc[fd]);
	}
#endif
	if (send(fd, "x", 1, 0) < 0) {
	    if (errno == ((fd == 4) ? EBADF : ENOTSOCK)) {
		fprintf(stderr, "ok\tsend 0 on %s failed with expected error: %s\n", fd_desc[fd], strerror(errno));
	    } else {
		fprintf(stderr, "not ok\tsend 0 on %s failed with unexpected error: %s\n", fd_desc[fd], strerror(errno));
	    }
	} else {
	    fprintf(stderr, "not ok\tsend 0 on %s unexpectedly successful\n", fd_desc[fd]);
	}
#ifdef MSG_NOSIGNAL
	if (send(fd, "x", 1, MSG_NOSIGNAL) < 0) {
	    if (errno == ((fd == 4) ? EBADF : ENOTSOCK)) {
		fprintf(stderr, "ok\tsend MSG_NOSIGNAL on %s failed with expected error: %s\n", fd_desc[fd], strerror(errno));
	    } else {
		fprintf(stderr, "not ok\tsend MSG_NOSIGNAL on %s failed with unexpected error: %s\n", fd_desc[fd], strerror(errno));
	    }
	} else {
	    fprintf(stderr, "not ok\tsend MSG_NOSIGNAL on %s unexpectedly successful\n", fd_desc[fd]);
	}
#endif
    }

    return 0;
}
