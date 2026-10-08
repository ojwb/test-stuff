#include <zlib.h>

int main() {
    z_stream* zstream = new z_stream;
    zstream->zalloc = static_cast<alloc_func>(nullptr);
    zstream->zfree = static_cast<free_func>(nullptr);
    zstream->opaque = static_cast<voidpf>(nullptr);
    delete zstream;
}
