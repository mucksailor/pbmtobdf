#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_INIT {0, 0, NULL}

#define VEC_INIT(S) {S, 0, 0, NULL}

#define A_INIT {0, 0, VEC_INIT(sizeof(Pair)), "pbm2.bdf", 0, "", "", "medium", UPRIGHT, "normal", "", 0, 75, 75, CHAR_CELL, "iso10646", 1}

// TODO
#define HELP \
  "usage: pbm2bdf [-options] [width]x[height] [<block>],[<block>],[...]\n" \
  "  -h, --help : this output\n" \
  "  -b, --blocks : list available unicode blocks\n" \
  "  -o, --output : <file> : output file\n" \
  "  -d, --descent <number> : descent\n" \
  "  -f, --foundry <string> : foundry\n" \
  "  -a, --family <string> : family\n" \
  "  -w, --weight <string> : weight\n" \
  "  -s, --slant : <slant> : slant (r = roman, i = italic, o = oblique, ri = reverse italic, ro = reverse oblique, ot = other)\n" \
  "  -e, --setwidth <string> : setwidth\n" \
  "  -t, --style <string> : style\n" \
  "  -p, --point_size <number> : point size\n" \
  "  -x, --resolution_x <number> : horizontal resolution\n" \
  "  -y, --resolution_y <number> : vertical resolution\n" \
  "  -c, --spacing <spacing> : spacing (m = monospaced, c = char cell)\n" \
  "  -r, --registry <string> : charset registry\n" \
  "  -e, --encoding <number> : charset encoding\n" \
  "    (<block> ::= [unicode block name]=[file path])\n"
  // TODO: not sure how intuitive this is ^

#define BLOCKS \
  "unicode blocks:\n" \
  "  basic_latin\n" \
  "  latin-1_supplement\n" \
  "  latin_extended-a\n" \
  "  latin_extended-b\n" \
  "  latin_extended_additional\n" \
  "  general_punctuation\n" \
  "  superscripts_and_subscripts\n" \
  "  currency_symbols\n" \
  "  letterlike_symbols\n" \
  "  number_forms\n" \
  "  arrows\n" \
  "  mathematical_operators\n" \
  "  miscellaneous_technical\n" \
  "  enclosed_alphanumerics\n" \
  "  box_drawing\n" \
  "  block_elements\n" \
  "  geometric_shapes\n"

#define HASH__H 0x59707a
#define HASH___HELP 0x65274403b28
#define HASH__B 0x597074
#define HASH___BLOCKS 0x1ae4c077bcd8fd
#define HASH__O 0x597081
#define HASH___OUTPUT 0x1ae4c096b5bc30
#define HASH__D 0x597076
#define HASH___DESCENT 0x3777ccff9409e05
#define HASH__F 0x597078
#define HASH___FOUNDRY 0x3777cd0aab2e346
#define HASH__A 0x597073
#define HASH___FAMILY 0x1ae4c080496ee1
#define HASH__W 0x597089
#define HASH___WEIGHT 0x1ae4c0a8374907
#define HASH__S 0x597085
#define HASH___SLANT 0xd0a0fd1255c1
#define HASH__E 0x597077
#define HASH___SETWIDTH 0x72671763fcf0f1eb
#define HASH__T 0x597086
#define HASH___STYLE 0xd0a0fd171e90
#define HASH__P 0x597082
#define HASH___POINT_SIZE 0xa88a0e0fbb5f5343
#define HASH__X 0x59708a
#define HASH___RESOLUTION_X 0xf4782df41770d78a
#define HASH__Y 0x59708b
#define HASH___RESOLUTION_Y 0xf4782df41770d78b
#define HASH__C 0x597075
#define HASH___SPACING 0x3777cd4944d4424
#define HASH__R 0x597084
#define HASH___REGISTRY 0x72671759f1647578
#define HASH__N 0x597080
#define HASH___ENCODING 0x726716db9de85046

#define HASH_R 0x2b617
#define HASH_I 0x2b60e
#define HASH_O 0x2b614
#define HASH_RI 0x597960
#define HASH_RO 0x597966
#define HASH_OT 0x597908

#define HASH_M 0x2b612
#define HASH_C 0x2b608

#define HASH_BASIC_LATIN 0xc072fa08660850fe
#define HASH_LATIN_1_SUPPLEMENT 0x2d490358556bfac7
#define HASH_LATIN_EXTENDED_A 0x102eed33564737bb
#define HASH_LATIN_EXTENDED_B 0x102eed33564737bc
#define HASH_LATIN_EXTENDED_ADDITIONAL 0x9efc26a60e5fed05
#define HASH_GENERAL_PUNCTUATION 0xb387b29a0d32dc
#define HASH_SUPERSCRIPTS_AND_SUBSCRIPTS 0x14064611fb35a6df
#define HASH_CURRENCY_SYMBOLS 0x7c70da5670af06b8
#define HASH_LETTERLIKE_SYMBOLS 0xfe6bc23b5f484da2
#define HASH_NUMBER_FORMS 0xd7a8c58f072d7bb4
#define HASH_ARROWS 0x652f272be63
#define HASH_MATHEMATICAL_OPERATORS 0xa74b141b17cc9e52
#define HASH_MISCELLANEOUS_TECHNICAL 0xec56c9c4191ddb03
#define HASH_ENCLOSED_ALPHANUMERICS 0x4d3f4a764b1332bd
#define HASH_BOX_DRAWING 0xc0754eff23093a59
#define HASH_BLOCK_ELEMENTS 0xc7fca0f7911d936c
#define HASH_GEOMETRIC_SHAPES 0xda8c0eebe2953347

#define PBM_INIT(W, H, L) {W, H, L, NULL}

#define GLYPH_INIT {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL}

#define BLOCK_INIT(N) {N, (block_ranges[(N)].end - block_ranges[(N)].start), NULL}

#define BDF_INIT {"2.1", STR_INIT, 0, 75, 75, 0, 0, 0, 0, 2, 0, 0, 0, VEC_INIT(sizeof(Block))}

typedef struct {
  size_t size;
  size_t cap;
  char *str;
} Str;

typedef struct {
  size_t size;
  size_t len;
  size_t cap;
  char *data;
} Vec;

typedef enum {
  OK,
  ERR,
  STOP,
} State;

typedef enum {
  BASIC_LATIN,
  LATIN_1_SUPPLEMENT,
  LATIN_EXTENDED_A,
  LATIN_EXTENDED_B,
  LATIN_EXTENDED_ADDITIONAL,
  GENERAL_PUNCTUATION,
  SUPERSCRIPTS_AND_SUBSCRIPTS,
  CURRENCY_SYMBOLS,
  LETTERLIKE_SYMBOLS,
  NUMBER_FORMS,
  ARROWS,
  MATHEMATICAL_OPERATORS,
  MISCELLANEOUS_TECHNICAL,
  ENCLOSED_ALPHANUMERICS,
  BOX_DRAWING,
  BLOCK_ELEMENTS,
  GEOMETRIC_SHAPES,
  SIZE_BLOCKS
} BlockName;

typedef struct {
  BlockName name;
  Str path;
} Pair;

typedef enum {
  UPRIGHT,
  ITALIC,
  OBLIQUE,
  REVERSE_ITALIC,
  REVERSE_OBLIQUE,
  OTHER
} Slant;

typedef enum {
  MONOSPACED,
  CHAR_CELL
} Spacing;

typedef struct {
  unsigned w, h;
  Vec pairs;
  char *output;
  unsigned descent;
  char *foundry;
  char *family;
  char *weight;
  Slant slant;
  char *setwidth;
  char *style;
  unsigned point_size;
  unsigned resolution_x, resolution_y;
  Spacing spacing;
  char *charset_registry;
  unsigned charset_encoding;
} Args;

typedef struct {
  size_t w, h, len;
  unsigned char **bitmap;
} Pbm;

typedef struct {
  unsigned encoding;
  unsigned swidth_x, swidth_y;
  unsigned dwidth_x, dwidth_y;
  unsigned bbx_w, bbx_h;
  int bbx_x, bbx_y;
  size_t w, h, len;
  unsigned char **bitmap;
} Glyph;

typedef struct {
  unsigned start;
  unsigned end;
} Range;

typedef struct {
  BlockName name;
  size_t size;
  Glyph *glyphs;
} Block;

typedef struct {
  char *startfont;
  Str font;
  unsigned size_pt, size_xdpi, size_ydpi;
  unsigned fontboundingbox_w, fontboundingbox_h;
  int fontboundingbox_x, fontboundingbox_y;
  unsigned properties; // TODO
  unsigned font_ascent, font_descent; // TODO
  unsigned chars;
  Vec blocks;
} Bdf;

static Args A = A_INIT;

// NOTE: exclusive upper bound
static const Range block_ranges[SIZE_BLOCKS] = {
  [BASIC_LATIN] = {0x0000, 0x0080},
  [LATIN_1_SUPPLEMENT] = {0x0080, 0x0100},
  [LATIN_EXTENDED_A] = {0x0100, 0x0180},
  [LATIN_EXTENDED_B] = {0x0180, 0x0250},
  [LATIN_EXTENDED_ADDITIONAL] = {0x1E00, 0x1F00},
  // TODO: various stuff is skipped over for now
  [GENERAL_PUNCTUATION] = {0x2000, 0x2070},
  [SUPERSCRIPTS_AND_SUBSCRIPTS] = {0x2070, 0x20A0},
  [CURRENCY_SYMBOLS] = {0x20A0, 0x20D0},
  [LETTERLIKE_SYMBOLS] = {0x2100, 0x2150},
  [NUMBER_FORMS] = {0x2150, 0x2190},
  [ARROWS] = {0x2190, 0x2200},
  [MATHEMATICAL_OPERATORS] = {0x2200, 0x2300},
  [MISCELLANEOUS_TECHNICAL] = {0x2300, 0x2400},
  // TODO: skip over some blank control chars for now
  [ENCLOSED_ALPHANUMERICS] = {0x2460, 0x2500},
  [BOX_DRAWING] = {0x2500, 0x2580},
  [BLOCK_ELEMENTS] = {0x2580, 0x25A0},
  [GEOMETRIC_SHAPES] = {0x25A0, 0x2600}
};

void die(const char *s) {
  perror(s);
  exit(1);
}

void str_grow(Str *str) {
  if (str->cap == 0) {
    str->cap = 4;
    str->str = malloc(str->cap * sizeof(char));
  } else {
    str->cap *= 2;
    str->str = realloc(str->str, str->cap * sizeof(char));
  }
}

void str_push(Str *str, char c) {
  if (str->size == str->cap)
    str_grow(str);
  str->str[str->size] = c;
  ++str->size;
}

void str_fmt(Str *str, const char *fmt, ...) {
  if (str->cap == 0)
    str_grow(str);
  char buf[str->cap - str->size];
  va_list va;
  va_start(va, fmt);
  size_t size = vsnprintf(buf, sizeof(buf), fmt, va);
  va_end(va);
  if (size < sizeof(buf)) {
    memcpy(str->str + str->size, buf, size);
  } else {
    while (str->cap < str->size + size)
      str_grow(str);
    char buf2[size + 1]; // TODO: change name
    va_start(va, fmt);
    vsnprintf(buf2, sizeof(buf2), fmt, va);
    va_end(va);
    memcpy(str->str + str->size, buf2, size);
  }
  str->size += size;
}

char *str_mv(Str str) {
  if (str.size == str.cap)
    str_grow(&str);
  char *to = str.str;
  to[str.size] = '\0';
  str.size = 0;
  str.cap = 0;
  str.str = NULL;
  return to;
}

void str_free(Str str) {
  free(str.str);
}

void str_cat(Str *str1, Str str2) {
  while (str1->cap < str1->size + str2.size)
    str_grow(str1);
  memcpy(str1->str + str1->size, str2.str, str2.size);
  str1->size += str2.size;
  str_free(str2);
}

void vec_grow(Vec *vec) {
  if (vec->cap == 0) {
    vec->cap = 4;
    vec->data = malloc(vec->cap * vec->size);
  } else {
    vec->cap *= 2;
    vec->data = realloc(vec->data, vec->cap * vec->size);
  }
}

void vec_push(Vec *vec, char *e) {
  if (vec->len == vec->cap)
    vec_grow(vec);
  char *ptr = vec->data + (vec->len * vec->size);
  memcpy(ptr, e, vec->size);
  ++vec->len;
}

char *vec_get(Vec *vec, size_t i) {
  return (i < vec->len) ? vec->data + (i * vec->size) : NULL;
}

char *vec_mv(size_t *len, Vec vec) {
  char *to = vec.data;
  *len = vec.len;
  vec.len = 0;
  vec.cap = 0;
  vec.data = NULL;
  return to;
}

void vec_free(Vec vec) {
  free(vec.data);
}

unsigned long hash(const char *s) {
  unsigned long hash = 5381;
  unsigned c;
  while ((c = *s++))
    hash = ((hash << 5) + hash) + c;
  return hash;
}

State args_parse(int argc, char *argv[]) {
  if (argc > 1) {
    size_t i;
    // TODO: string ones need checks
    for (i = 1; i < argc && argv[i][0] == '-'; ++i)
      switch (hash(argv[i])) {
        case HASH__H:
        case HASH___HELP:
          printf(HELP);
          return STOP;
        case HASH__B:
        case HASH___BLOCKS:
          printf(BLOCKS);
          return STOP;
        case HASH__O:
        case HASH___OUTPUT:
          A.output = argv[++i];
          break;
        case HASH__D:
        case HASH___DESCENT:
          if (!sscanf(argv[++i], "%u", &A.descent)) {
            printf("'-d, --descent' option requires a numeric argument");
            return ERR;
          }
          break;
        case HASH__F:
        case HASH___FOUNDRY:
          A.foundry = argv[++i];
          break;
        case HASH__A:
        case HASH___FAMILY:
          A.family = argv[++i];
          break;
        case HASH__W:
        case HASH___WEIGHT:
          A.weight = argv[++i];
          break;
        case HASH__S:
        case HASH___SLANT:
          switch (hash(argv[++i])) {
            case HASH_R:
              A.slant = UPRIGHT;
              break;
            case HASH_I:
              A.slant = ITALIC;
              break;
            case HASH_O:
              A.slant = OBLIQUE;
              break;
            case HASH_RI:
              A.slant = REVERSE_ITALIC;
              break;
            case HASH_RO:
              A.slant = REVERSE_OBLIQUE;
              break;
            case HASH_OT:
              A.slant = OTHER;
              break;
            default:
              printf("invalid argument for option '-s, --style'");
              return ERR;
          }
          break;
        case HASH__E:
        case HASH___SETWIDTH:
          A.setwidth = argv[++i];
          break;
        case HASH__T:
        case HASH___STYLE:
          A.style = argv[++i];
          break;
        case HASH__P:
        case HASH___POINT_SIZE:
          if (!sscanf(argv[++i], "%u", &A.point_size)) {
            printf("'-p, --pont_size' option requires numeric argument");
            return ERR;
          }
          break;
        case HASH__X:
        case HASH___RESOLUTION_X:
          if (!sscanf(argv[++i], "%u", &A.resolution_x)) {
            printf("'-x, --resolution_x' option requires numeric argument");
            return ERR;
          }
          break;
        case HASH__Y:
        case HASH___RESOLUTION_Y:
          if (!sscanf(argv[++i], "%u", &A.resolution_y)) {
            printf("'-y, --resolution_y' option requires numeric argument");
            return ERR;
          }
          break;
        case HASH__C:
        case HASH___SPACING:
          switch (hash(argv[++i])) {
            case HASH_M:
              A.spacing = MONOSPACED;
              break;
            case HASH_C:
              A.spacing = CHAR_CELL;
              break;
            default:
              printf("invalid argument for option '-s, --style'");
              return ERR;
          }
          break;
        case HASH__R:
        case HASH___REGISTRY:
          A.charset_registry = argv[++i];
          break;
        case HASH__N:
        case HASH___ENCODING:
          if (!sscanf(argv[++i], "%u", &A.charset_encoding)) {
            printf("'-e, --encoding' option requires numeric argument");
            return ERR;
          }
          break;
        default:
          printf("invalid option, use pbm2bdf -h' for more info");
          return ERR;
      }
    if (!sscanf(argv[i++], "%ux%u", &A.w, &A.h)) {
      printf("invalid dimensions; specify as '[width]x[height]'");
      return ERR;
    }
    char *tok = strtok(argv[i], ",");
    while (tok != NULL) {
      char *name_s = strsep(&tok, "=");
      char *path_s = tok;
      unsigned name;
      switch (hash(name_s)) {
        case HASH_BASIC_LATIN:
          name = BASIC_LATIN;
          break;
        case HASH_LATIN_1_SUPPLEMENT:
          name = LATIN_1_SUPPLEMENT;
          break;
        case HASH_LATIN_EXTENDED_A:
          name = LATIN_EXTENDED_A;
          break;
        case HASH_LATIN_EXTENDED_B:
          name = LATIN_EXTENDED_B;
          break;
        case HASH_LATIN_EXTENDED_ADDITIONAL:
          name = LATIN_EXTENDED_ADDITIONAL;
          break;
        case HASH_GENERAL_PUNCTUATION:
          name = GENERAL_PUNCTUATION;
          break;
        case HASH_SUPERSCRIPTS_AND_SUBSCRIPTS:
          name = SUPERSCRIPTS_AND_SUBSCRIPTS;
          break;
        case HASH_CURRENCY_SYMBOLS:
          name = CURRENCY_SYMBOLS;
          break;
        case HASH_LETTERLIKE_SYMBOLS:
          name = LETTERLIKE_SYMBOLS;
          break;
        case HASH_NUMBER_FORMS:
          name = NUMBER_FORMS;
          break;
        case HASH_ARROWS:
          name = ARROWS;
          break;
        case HASH_MATHEMATICAL_OPERATORS:
          name = MATHEMATICAL_OPERATORS;
          break;
        case HASH_MISCELLANEOUS_TECHNICAL:
          name = MISCELLANEOUS_TECHNICAL;
          break;
        case HASH_ENCLOSED_ALPHANUMERICS:
          name = ENCLOSED_ALPHANUMERICS;
          break;
        case HASH_BOX_DRAWING:
          name = BOX_DRAWING;
          break;
        case HASH_BLOCK_ELEMENTS:
          name = BLOCK_ELEMENTS;
          break;
        case HASH_GEOMETRIC_SHAPES:
          name = GEOMETRIC_SHAPES;
          break;
        default:
          printf("block is unavailable or does not exist");
          return ERR;
      }
      Str path = STR_INIT;
      str_fmt(&path, "%s", path_s); // TODO: some sort of append is probably better
      Pair pair = {name, path};
      vec_push(&A.pairs, (char *)&pair);
      tok = strtok(NULL, ",");
    }
    return OK;
  }
  printf(HELP);
  return ERR;  
}

Pbm pbm_new(size_t w, size_t h) {
  size_t len = (w + CHAR_BIT - 1) / CHAR_BIT;
  Pbm pbm = PBM_INIT(w, h, len);
  pbm.bitmap = malloc(h * sizeof(char *));
  for (size_t y = 0; y < h; ++y)
    pbm.bitmap[y] = calloc(len, sizeof(char));
  return pbm;
}

void pbm_free(Pbm pbm) {
  free(pbm.bitmap);
}

Pbm pbm_from(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp)
    die("pbm_from: null file pointer");
  if (fgetc(fp) != 'P' || fgetc(fp) != '4' || fgetc(fp) != '\n')
    die("pbm_from: file is not PBM");
  int c = fgetc(fp);
  while (c == '#') {
    c = fgetc(fp);
    while (c != '\n' && c != '\r')
      c = fgetc(fp);
  }
  Str str_w = STR_INIT;
  while (!isspace(c)) {
    str_push(&str_w, c);
    c = fgetc(fp);
  }
  Str str_h = STR_INIT;
  c = fgetc(fp);
  while (c != '\n' && c != '\r') {
    str_push(&str_h, c);
    c = fgetc(fp);
  }
  size_t w, h;
  char *buf_w = str_mv(str_w);
  char *buf_h = str_mv(str_h);
  sscanf(buf_w, "%zu", &w);
  sscanf(buf_h, "%zu", &h);
  free(buf_w);
  free(buf_h);
  Pbm pbm = pbm_new(w, h);
  size_t x = 0;
  size_t y = 0;
  while ((c = fgetc(fp)) != EOF) {
    if (x > pbm.len - 1) {
      x = 0;
      ++y;
    }
    pbm.bitmap[y][x] = c;
    ++x;
  }
  fclose(fp);
  return pbm;
}

Pbm *pbm_repage(size_t *size, Pbm pbm) {
  *size = (pbm.w / A.w) * (pbm.h / A.h);
  Pbm *pbms = malloc(*size * sizeof(Pbm)); 
  size_t i = 0;
  for (size_t p_y = 0; p_y < pbm.h; p_y += A.h)
    for (size_t p_x = 0; p_x < pbm.w; p_x += A.w) {
      Pbm p = pbm_new(A.w, A.h);
      for (size_t y = 0; y < A.h; ++y) {
        size_t p_o = p_x / CHAR_BIT;
        char p_b = (CHAR_BIT - 1) - (p_x % CHAR_BIT);
        size_t o = 0;
        char b = CHAR_BIT - 1;
        for (unsigned x = 0; x < A.w; ++x) {
          if (p_b < 0) {
            ++p_o;
            p_b = CHAR_BIT - 1;
          }
          if (b < 0) {
            ++o;
            b = CHAR_BIT - 1;
          }
          unsigned char bit = (pbm.bitmap[p_y + y][p_o] >> p_b) & 1;
          p.bitmap[y][o] |= (bit << b);
          --p_b;
          --b;
        }          
      }
      pbms[i++] = p;
    }
  return pbms;
}

Glyph glyph_new(Pbm pbm, const unsigned encoding) {
  Glyph glyph = GLYPH_INIT;
  glyph.encoding = encoding;
  glyph.swidth_x = (pbm.w * 72) / A.resolution_x; //pbm.w * 100
  glyph.dwidth_x = pbm.w;
  glyph.bbx_w = pbm.w;
  glyph.bbx_h = pbm.h;
  glyph.bbx_y = -A.descent;
  glyph.w = pbm.w;
  glyph.h = pbm.h;
  glyph.len = pbm.len;
  // TODO: change to a move
  glyph.bitmap = malloc(pbm.h * sizeof(char *));
  for (size_t y = 0; y < pbm.h; ++y) {
    glyph.bitmap[y] = malloc(pbm.len * sizeof(char));
    for (size_t x = 0; x < pbm.len; ++x)
      glyph.bitmap[y][x] = pbm.bitmap[y][x];
  }
  pbm_free(pbm);
  return glyph;
}

void glyph_free(Glyph glyph) {
  free(glyph.bitmap);
}

Str glyph_gen(Glyph glyph) {
  Str str = STR_INIT;
  str_fmt(&str, "STARTCHAR U+%04x\nENCODING %u\nSWIDTH %u %u\nDWIDTH %u %u\nBBX %u %u %d %d\nBITMAP\n", glyph.encoding, glyph.encoding, glyph.swidth_x, glyph.swidth_y, glyph.dwidth_x, glyph.dwidth_y, glyph.bbx_w, glyph.bbx_h, glyph.bbx_x, glyph.bbx_y);
  for (size_t y = 0; y < glyph.h; ++y) {
    for (size_t x = 0; x < glyph.len; ++x)
      str_fmt(&str, "%02x", glyph.bitmap[y][x]);
    str_push(&str, '\n');
  }
  str_fmt(&str, "ENDCHAR\n");
  glyph_free(glyph);
  return str;
}

Block block_new(BlockName name, char *path) {
  Block block = BLOCK_INIT(name);
  block.glyphs = malloc(block.size * sizeof(Glyph));
  size_t size;
  Pbm pbm = pbm_from(path);
  Pbm *pbms = pbm_repage(&size, pbm);
  if (size < block.size) {
    printf("pbm '%s' is of size %zu but expected %zu; is it missing control characters?", path, block.size, size);
    die("pbm is not an acceptable size");
  }
  unsigned code = block_ranges[name].start;
  for (size_t i = 0; i < block.size; ++i) {
    Glyph glyph = glyph_new(pbms[i], code);
    block.glyphs[i] = glyph;
    ++code;
  }
  return block;
}

void block_free(Block block) {
  free(block.glyphs);
}

Str block_gen(Block block) {
  Str str = STR_INIT;
  for (size_t i = 0; i < block.size; ++i)
    str_cat(&str, glyph_gen(block.glyphs[i]));
  block_free(block);
  return str;
}

Bdf bdf_new() {
  Bdf bdf = BDF_INIT;
  Str str_slant = STR_INIT;
  switch (A.slant) {
    case UPRIGHT:
      str_push(&str_slant, 'r');
      break;
    case ITALIC:
      str_push(&str_slant, 'i');
      break;
    case OBLIQUE:
      str_push(&str_slant, 'o');
      break;
    case REVERSE_ITALIC:
      str_fmt(&str_slant, "ri");
      break;
    case REVERSE_OBLIQUE:
      str_fmt(&str_slant, "ro");
      break;
    case OTHER:
      str_fmt(&str_slant, "ot");
      break;
  }
  char *slant = str_mv(str_slant);
  unsigned point_size = (A.point_size == 0) ? (A.h * 72) / A.resolution_y : A.point_size;    
  char spacing = (A.spacing == MONOSPACED) ? 'm' : 'c';
  str_fmt(&bdf.font, "-%s-%s-%s-%s-%s-%s-%u-%u-%u-%u-%c-%u-%s-%u", A.foundry, A.family, A.weight, slant, A.setwidth, A.style, A.h, point_size, A.resolution_x, A.resolution_y, spacing, (unsigned)(A.point_size * (float)(A.w / A.h)), A.charset_registry, A.charset_encoding);
  free(slant); 
  bdf.size_pt = point_size;
  bdf.size_xdpi = A.resolution_x;
  bdf.size_ydpi = A.resolution_y;
  bdf.fontboundingbox_w = A.w;
  bdf.fontboundingbox_h = A.h;
  bdf.fontboundingbox_y = -A.descent; // TODO
  bdf.font_ascent = A.h - A.descent; // TODO
  bdf.font_descent = A.descent;
  size_t len;
  Pair *pairs = (Pair *)vec_mv(&len, A.pairs);
  for (size_t i = 0; i < len; ++i) {
    char *path = str_mv(pairs[i].path);
    Block block = block_new(pairs[i].name, path);
    bdf.chars += block.size;
    vec_push(&bdf.blocks, (char *)&block);
  }
  return bdf;
}

void bdf_gen(Bdf bdf) {
  FILE *fp = fopen(A.output, "w");
  Str str = STR_INIT;
  char *font = str_mv(bdf.font);
  str_fmt(&str, "STARTFONT %s\nFONT %s\nSIZE %u %u %u\nFONTBOUNDINGBOX %u %u %d %d\n", bdf.startfont, font, bdf.size_pt, bdf.size_xdpi, bdf.size_ydpi, bdf.fontboundingbox_w, bdf.fontboundingbox_h, bdf.fontboundingbox_x, bdf.fontboundingbox_y);
  free(font);
  if (bdf.properties != 0)
    str_fmt(&str, "STARTPROPERTIES %u\nFONT_ASCENT %u\nFONT_DESCENT %u\nENDPROPERTIES\n", bdf.properties, bdf.font_ascent, bdf.font_descent); // TODO
  str_fmt(&str, "CHARS %u\n", bdf.chars);
  size_t len;
  Block *blocks = (Block *)vec_mv(&len, bdf.blocks); // TODO: is this even really necessary
  for (size_t i = 0; i < len; ++i)
    str_cat(&str, block_gen(blocks[i]));
  free(blocks);
  str_fmt(&str, "ENDFONT\n");
  char *buf = str_mv(str);
  fprintf(fp, buf);
  fclose(fp);
  free(buf);
}

int main(int argc, char *argv[]) {
  switch (args_parse(argc, argv)) {
    case OK:
      break;
    case ERR:
      return 1;
    case STOP:
      return 0;
  }
  Bdf bdf = bdf_new();
  bdf_gen(bdf);
  return 0;
}
