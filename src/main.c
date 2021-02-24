#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
    Chunk chunk;
    initChunk(&chunk);

    for (int i = 0; i < 256; i++) {
        writeConstant(&chunk, 1.2, 123);
    }

    writeConstant(&chunk, 3.14, 125);

    writeChunk(&chunk, OP_RETURN, 126);

    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
    return 0;
}
