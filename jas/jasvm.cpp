#include <fstream>
#include <exception>
#include <string>
#include <vector>
#include <format>
#include "jstdint.h"

std::string read_entire_file(const char* path) {
  std::ifstream file(path, std::ios::ate);
  if (!file) throw std::runtime_error("could not open file");

  size_t size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::string content(size, '\0');
  file.read(&content[0], size);

  return content;
}

struct Lexer {
  const char* file_path;
  size_t row;
  const char* current;
  const char* start;
};


#define TOKEN_TYPE_LIST \
  X(IDN)  \
  X(DIR)  \
  X(REG)  \
  X(IMM)  \
  X(PAL)  \
  X(PAR)  \
  X(COM)  \
  X(COL)  \
  X(NONE)

enum TokenType {
#define X(name) name,
    TOKEN_TYPE_LIST
#undef X
    TOKEN_TYPE_COUNT
};

const char* const TOKEN_TYPE_NAMES[] = {
#define X(name) #name,
    TOKEN_TYPE_LIST
#undef X
};

struct Token {
  TokenType type;

  const char* text;
  size_t size;

  const char* file_path;
  size_t row;
};

void print_token(Token* token) {

  printf("%s:%zu: [%s] %*.*s\n", token->file_path, token->row,
         TOKEN_TYPE_NAMES[token->type],
         (int)token->size, (int)token->size, token->text);
}

Token lex_token(Lexer* lex) {
  // for token
  const char* start = NULL;
  const char* end   = NULL;
  TokenType type    = NONE;
  TokenType types[256] = { TOKEN_TYPE_COUNT };
  types[':'] = COL;
  types[','] = COM;
  types['('] = PAL;
  types[')'] = PAR;
  types['.'] = DIR;
  types['%'] = REG;
  types['$'] = IMM;

  for (;;) {
    // finish token if any of these ocurr
    // we do not move
    switch (*lex->current) {
      case '#':
      case ':':
      case ',':
      case '(':
      case ')':
      case '\n':
      case ' ':
      case '\r':
      case '\t':
        if (lex->start) {
          start = lex->start;
          end = lex->current;
          lex->start = NULL;
          return {
            .type = type,
            .text = start,
            .size = (size_t)(end - start),
            .file_path = lex->file_path,
            .row = lex->row,
          };
        }
    }

    // move
    switch (*lex->current) {
      case '#':
        while (*lex->current && *lex->current != '\n')
          lex->current++;
        break;

      case ':':
      case ',':
      case '(':
      case ')':
        start = lex->current;
        lex->start = NULL;
        lex->current++;
        return {
          .type = types[(u8)*start],
          .text = start,
          .size = 1,
          .file_path = lex->file_path,
          .row = lex->row,
        };

      case '.':
      case '%':
      case '$':
        if (lex->start) {
          fprintf(stderr, "ERROR: lex_token: unexpected char '%c' in token", *lex->current);
          abort();
        }
        type = types[(u8)*lex->current];
        lex->start = lex->current++;
        break;

      case '\n':
        lex->row++;
      case ' ':
      case '\r':
      case '\t':
        lex->current++;
        break;

      // all other chars are valid in IDN
      default:
        if (!lex->start) {
          lex->start = lex->current;
          type = IDN;
        }
        lex->current++;
        break;
    }
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "ERROR: usage: %s <file>\n", argv[0]);
    return 1;
  }

  const char* file_path = argv[1];
  std::string content = read_entire_file(file_path);
  size_t size = content.size();
  printf("%s: %zu bytes\n", file_path, size);

  std::vector<Token> tokens;

  Lexer lex = {
    .file_path = file_path,
    .row = 0,
    .current = content.c_str(),
    .start = NULL,
  };

  while (*lex.current) {
    Token token = lex_token(&lex);
    print_token(&token);
    tokens.push_back(token);
  }
}
