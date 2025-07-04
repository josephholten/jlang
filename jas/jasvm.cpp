#include <fstream>
#include <string>
#include <vector>
#include <string.h>
#include <frozen/unordered_map.h>
#include <frozen/string.h>
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


#define TOKENTYPE_LIST \
  X(IDN)  \
  X(DIR)  \
  X(REG)  \
  X(IMM)  \
  X(PAL)  \
  X(PAR)  \
  X(COM)  \
  X(COL)  \
  X(STR)  \
  X(NONE)

enum class TokenType : u8 {
#define X(name) name,
    TOKENTYPE_LIST
#undef X
    COUNT
};

const char* const TOKENTYPE_TO_STR[] = {
#define X(name) #name,
    TOKENTYPE_LIST
#undef X
};

struct Token {
  TokenType type;
  std::string_view text;
  const char* file_path;
  size_t row;
};

int tokencmp(Token* t, Token* s) {
  return (t->type == s->type) && (t->text == t->text);
}

Token create_token(TokenType type, std::string_view text) {
  return {
    .type = type,
    .text = text,
    .file_path = NULL,
    .row = 0,
  };
}

void print_token(Token* token) {
  printf("%s:%zu: [%s] %*.*s\n", token->file_path, token->row,
         TOKENTYPE_TO_STR[(u8)token->type],
         (int)token->text.size(), (int)token->text.size(), token->text.data());
}

Token lex_token(Lexer* lex) {
  // for token
  const char* start = NULL;
  const char* end   = NULL;
  TokenType type    = TokenType::NONE;
  TokenType types[256] = { TokenType::NONE };
  types[':'] = TokenType::COL;
  types[','] = TokenType::COM;
  types['('] = TokenType::PAL;
  types[')'] = TokenType::PAR;
  types['.'] = TokenType::DIR;
  types['%'] = TokenType::REG;
  types['$'] = TokenType::IMM;

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
      case '\0':
        if (lex->start || *lex->current == '\0') {
          start = lex->start;
          end = lex->current;
          lex->start = NULL;
          return {
            .type = type,
            .text = { start, (size_t)(end - start)},
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

      case '"':
        lex->current++; // skip '"'
        lex->start = lex->current;

        while (*lex->current && *lex->current != '"')
          lex->current++;

        if (!*lex->current) {
          fprintf(stderr, "ERROR: lex_token: encountered EOF while parsing string starting in line %zu",
                  lex->row);
          abort();
        }

        // token
        start = lex->start;
        end = lex->current;
        lex->start = NULL;
        lex->current++; // skip '"'
        return {
          .type = TokenType::STR,
          .text = { start, (size_t)(end - start)},
          .file_path = lex->file_path,
          .row = lex->row,
        };

      case ':':
      case ',':
      case '(':
      case ')':
        start = lex->current;
        lex->start = NULL;
        lex->current++;
        return {
          .type = types[(u8)*start],
          .text = {start,1},
          .file_path = lex->file_path,
          .row = lex->row,
        };

      case '.':
      case '%':
      case '$':
        if (lex->start) {
          fprintf(stderr, "ERROR: lex_token: unexpected char '%c' in token at line %zu",
                  *lex->current, lex->row);
          abort();
        }
        type = types[(u8)*lex->current];
        lex->start = ++lex->current; // skip initial symbol
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
          type = TokenType::IDN;
        }
        lex->current++;
        break;
    }
  }
}

std::vector<Token> lex_tokens(Lexer* lex) {
  std::vector<Token> tokens;
  while (*lex->current) {
    Token token = lex_token(lex);
    if (token.type != TokenType::NONE)
      tokens.push_back(token);
  }
  return tokens;
}

#define SECTION_LIST \
  X(text)  \
  X(data)  \
  X(bss)

enum class Section : u8 {
#define X(name) name,
    SECTION_LIST
#undef X
    COUNT
};

const char* const SECTION_TO_STR[] = {
#define X(name) #name,
    SECTION_LIST
#undef X
};

static constexpr frozen::unordered_map<
  frozen::string, Section,
  (u8)Section::COUNT
> STR_TO_SECTION = {
#define X(NAME) {#NAME, Section::NAME},
  SECTION_LIST
#undef X
};

#define OPCODE_LIST \
  X(XOR) \
  X(MOVB) \
  X(TEST) \
  X(JE) \
  X(INC) \
  X(JMP) \
  X(RET) \
  X(LEA) \
  X(CALL) \
  X(MOV) \
  X(SYSCALL)

enum class OpCode : u8 {
#define X(name) name,
    OPCODE_LIST
#undef X
    COUNT
};

const char* const OPCODE_TO_STR[] = {
#define X(name) #name,
    OPCODE_LIST
#undef X
};

static constexpr frozen::unordered_map<
  frozen::string, OpCode,
  (u8)OpCode::COUNT
> STR_TO_OPCODE = {
#define X(NAME) {#NAME, OpCode::NAME},
  OPCODE_LIST
#undef X
};



#define REGISTER_LIST \
  X(rax) \
  X(rbx) \
  X(rcx) \
  X(rsp) \
  X(rbp) \
  X(rdi) \
  X(rsi) \
  X(rdx)

enum class Register : u8 {
#define X(name) name,
    REGISTER_LIST
#undef X
    COUNT
};

const char* const REGISTER_NAMES[] = {
#define X(name) #name,
    REGISTER_LIST
#undef X
};

static constexpr frozen::unordered_map<
  frozen::string, Register,
  (u8)Register::COUNT
> STR_TO_REGISTER = {
#define X(NAME) {#NAME, Register::NAME},
  REGISTER_LIST
#undef X
};



int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "ERROR: usage: %s <file>\n", argv[0]);
    return 1;
  }

  const char* file_path = argv[1];
  std::string content = read_entire_file(file_path);
  size_t size = content.size();
  printf("%s: %zu bytes\n", file_path, size);

  Lexer lex = {
    .file_path = file_path,
    .row = 1,
    .current = content.c_str(),
    .start = NULL,
  };

  std::vector<Token> tokens = lex_tokens(&lex);
  for (size_t i = 0; i < tokens.size(); i++)
    print_token(&tokens[i]);
}
