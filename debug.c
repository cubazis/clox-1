#include <stdio.h>

#include "debug.h"
#include "value.h"

/* return line number of opcode which is at given offset */
static int get_line(line_array_t *lines, int offset)
{
  int m;
  int s = 0;
  int e = lines->count;
  line_t *data = lines->data;

  /* Works like std::upper_bound() in C++ */
  while (s < e) {
    m = (s + e) / 2;
    if (data[m].offset <= offset) {
      s = m + 1;
    } else {
      e = m;
    }
  }

  /*
   * Since index of first element which is greater than given element
   * is e, return data[e - 1].line
   */
  return data[e - 1].line;
}

static int simple_instruction(const char *name, int offset)
{
  printf("%s\n", name);
  return offset + 1;
}

static int constant_long_instruction(const char *name, chunk_t *chunk,
                                     int offset)
{
  uint32_t constant = (chunk->code[offset]) | 
                      (chunk->code[offset + 1] << 8) |
                      (chunk->code[offset + 2] << 16);
  printf("%-16s %4d '", name, constant);
  print_value(chunk->constants.values[constant]);
  puts("'");
  return offset + 4;
}

static int constant_instruction(const char *name, chunk_t *chunk,
                                int offset)
{
  uint8_t constant = chunk->code[offset + 1];
  printf("%-16s %4d '", name, constant);
  print_value(chunk->constants.values[constant]);
  puts("'");
  return offset + 2;
}

void disassemble_chunk(chunk_t *chunk, const char *name)
{
  printf("== %s ==\n", name);

  for (int i = 0; i < chunk->count; )
    i = disassemble_instruction(chunk, i);
}

int disassemble_instruction(chunk_t *chunk, int offset)
{
  printf("%04d ", offset);

  int line = get_line(&chunk->lines, offset);
  if (offset > 0 && line == get_line(&chunk->lines, offset - 1)) {
    printf("   | ");
  } else {
    printf("%4d ", line);
  }

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
  case OP_CONSTANT:
    return constant_instruction("OP_CONSTANT", chunk, offset);
  case OP_CONSTANT_LONG:
    return constant_long_instruction("OP_CONSTANT_LONG", chunk, offset);
  case OP_ADD:
    return simple_instruction("OP_ADD", offset);
  case OP_SUBTRACT:
    return simple_instruction("OP_SUBTRACT", offset);
  case OP_MULTIPLY:
    return simple_instruction("OP_MULTIPLY", offset);
  case OP_DIVIDE:
    return simple_instruction("OP_DIVIDE", offset);
  case OP_NEGATE:
    return simple_instruction("OP_NEGATE", offset);
  case OP_RETURN:
    return simple_instruction("OP_RETURN", offset);
  default:
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
  }
}
