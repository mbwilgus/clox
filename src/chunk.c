#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk)
{
    chunk->count          = 0;
    chunk->capacity       = 0;
    chunk->code           = NULL;
    chunk->lines.index    = 0;
    chunk->lines.capacity = 0;
    chunk->lines.runs     = NULL;
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines.runs, chunk->lines.capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

static void setLine(Chunk* chunk, int line)
{
    Lines* lines = &chunk->lines;
    int cursor   = lines->index;
    int current  = cursor + 1;

    bool sameLine = lines->capacity > 0 ? line == lines->runs[current] : false;

    if (sameLine) {
        lines->runs[cursor]++;
    } else {
        if (lines->capacity > 0) {
            cursor +=2;
            current = cursor + 1;
            lines->index += 2;
        }

        if (lines->capacity < current) {
            int oldCapacity = lines->capacity;
            lines->capacity = GROW_CAPACITY(oldCapacity);
            lines->runs =
                GROW_ARRAY(int, lines->runs, oldCapacity, lines->capacity);
        }

        lines->runs[cursor]  = 1;
        lines->runs[current] = line;
    }
}

void writeChunk(Chunk* chunk, uint8_t byte, int line)
{
    int cursor = chunk->count;

    if (chunk->capacity < cursor + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code =
            GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    chunk->code[cursor] = byte;
    chunk->count++;

    setLine(chunk, line);
}

static void writeChunkLong(Chunk* chunk, int index, int line)
{
    for (int shift = 16; shift >= 0; shift -= 8) {
        writeChunk(chunk, (index >> shift) & 0xFF, line);
    }
}

void writeConstant(Chunk* chunk, Value value, int line)
{
    int index = addConstant(chunk, value);
    if (index < 256) {
        writeChunk(chunk, OP_CONSTANT, line);
        writeChunk(chunk, index, line);
    } else {
        writeChunk(chunk, OP_CONSTANT_LONG, line);
        writeChunkLong(chunk, index, line);
    }
}

int addConstant(Chunk* chunk, Value value)
{
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}

int getLine(Chunk* chunk, int offset)
{
    int accumulator = 0;
    int cursor      = 0;
    while (accumulator < offset) {
        accumulator += chunk->lines.runs[cursor];
        if (accumulator <= offset) cursor += 2;
    }

    return chunk->lines.runs[cursor + 1];
}
