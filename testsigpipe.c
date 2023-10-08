#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

// SO_NOSIGPIPE: DragonflyBSD FreeBSD NetBSD macOS (11,12) (not OpenBSD Linux)
// MSG_NOSIGNAL: DragonflyBSD FreeBSD NetBSD macOS (11,12) OpenBSD Linux

int main() {
    {
	int fds[2];
	if (socketpair(PF_UNIX, SOCK_STREAM, 0, fds) < 0) {
	    perror("socketpair");
	    return 1;
	}
	int fd = fds[0];
#ifdef SO_NOSIGPIPE
	int flag = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (char*)&flag, sizeof(flag)) < 0) {
	    perror("setsockopt SO_NOSIGPIPE");
	} else {
	    fprintf(stderr, "Set SO_NOSIGPIPE successfully\n");
	}
#endif
	if (send(fd, "x", 1, 0) < 0) {
	    perror("send 0");
	} else {
	    fprintf(stderr, "send 0 OK\n");
	}
#ifdef MSG_NOSIGNAL
	if (send(fd, "x", 1, MSG_NOSIGNAL) < 0) {
	    perror("send MSG_NOSIGNAL");
	} else {
	    fprintf(stderr, "send MSG_NOSIGNAL OK\n");
	}
#endif
	close(fd);
	close(fds[1]);
    }

    {
	int fds[2];
	if (pipe(fds) < 0) {
	    perror("pipe");
	    return 1;
	}
	int fd = fds[1];
#ifdef SO_NOSIGPIPE
	int flag = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (char*)&flag, sizeof(flag)) < 0) {
	    perror("setsockopt SO_NOSIGPIPE");
	} else {
	    fprintf(stderr, "Set SO_NOSIGPIPE successfully\n");
	}
#endif
	if (send(fd, "x", 1, 0) < 0) {
	    perror("send 0");
	} else {
	    fprintf(stderr, "send 0 OK\n");
	}
#ifdef MSG_NOSIGNAL
	if (send(fd, "x", 1, MSG_NOSIGNAL) < 0) {
	    perror("send MSG_NOSIGNAL");
	} else {
	    fprintf(stderr, "send MSG_NOSIGNAL OK\n");
	}
#endif
	close(fd);
	close(fds[0]);
    }

    return 0;
}
