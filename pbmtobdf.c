#include "pbmtobdf.h"

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define X(I, N, S, E) I,
typedef enum { BLOCKS SIZE_BLOCKS } BlockID;
#undef X

#define X(I, N, S, E) N,
static const char *BlockNames[SIZE_BLOCKS] = {BLOCKS};
#undef X

typedef struct {
  unsigned start;
  unsigned end;
} Range;

#define X(I, N, S, E) {S, E},
static const Range BlockRanges[SIZE_BLOCKS] = {BLOCKS};
#undef X

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

typedef struct {
  BlockID id;
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

typedef enum { MONOSPACED, CHAR_CELL } Spacing;

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
  BlockID id;
  size_t size;
  Glyph *glyphs;
} Block;

typedef struct {
  char *startfont;
  Str font;
  unsigned size_pt, size_xdpi, size_ydpi;
  unsigned fontboundingbox_w, fontboundingbox_h;
  int fontboundingbox_x, fontboundingbox_y;
  unsigned properties;                // TODO
  unsigned font_ascent, font_descent; // TODO
  unsigned chars;
  Vec blocks;
} Bdf;

static const struct option Options[] = {
    {"help", no_argument, NULL, 'h'},
    {"blocks", no_argument, NULL, 'b'},
    {"output", required_argument, NULL, 'o'},
    {"descent", required_argument, NULL, 'd'},
    {"foundry", required_argument, NULL, 'f'},
    {"family", required_argument, NULL, 'a'},
    {"weight", required_argument, NULL, 'w'},
    {"slant", required_argument, NULL, 's'},
    {"setwidth", required_argument, NULL, 'e'},
    {"style", required_argument, NULL, 't'},
    {"point_size", required_argument, NULL, 'p'},
    {"resolution_x", required_argument, NULL, 'x'},
    {"resolution_y", required_argument, NULL, 'y'},
    {"spacing", required_argument, NULL, 'c'},
    {"registry", required_argument, NULL, 'r'},
    {"encoding", required_argument, NULL, 'n'},
    {NULL, 0, NULL, 0}};

static Args A = A_INIT;

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

void str_free(Str str) { free(str.str); }

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

void vec_free(Vec vec) { free(vec.data); }

unsigned long hash(const char *s) {
  unsigned long hash = 5381;
  unsigned c;
  while ((c = *s++))
    hash = ((hash << 5) + hash) + c;
  return hash;
}

State args_parse(int argc, char *argv[]) {
  int opt, i;
  while ((opt = getopt_long(argc, argv, "hbo:d:f:a:w:s:e:t:p:x:y:c:r:n:",
                            Options, &i)) != -1) {
    switch (opt) {
    case 'h':
      printf(HELP);
      return STOP;
    case 'b':
      printf("blocks:\n");
      for (size_t j = 0; j < SIZE_BLOCKS; ++j)
        printf("  %s\n", BlockNames[j]);
      return STOP;
    case 'o':
      A.output = optarg;
      break;
    case 'd':
      if (!sscanf(optarg, "%u", &A.descent)) {
        printf("'-d, --descent' requires numeric argument");
        return ERR;
      }
      break;
    case 'f':
      A.foundry = optarg;
      break;
    case 'a':
      A.family = optarg;
      break;
    case 'w':
      A.weight = optarg;
      break;
    case 's':
      // TODO
      switch (hash(optarg)) {
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
        printf("invalid argument for '-s, --style'");
        return ERR;
      }
      break;
    case 'e':
      A.setwidth = optarg;
      break;
    case 't':
      A.style = optarg;
      break;
    case 'p':
      if (!sscanf(optarg, "%u", &A.point_size)) {
        printf("'-p, --point_size' requires numeric argument");
        return ERR;
      }
      break;
    case 'x':
      if (!sscanf(optarg, "%u", &A.resolution_x)) {
        printf("'-x, --resolution_x' requires numeric argument");
        return ERR;
      }
      break;
    case 'y':
      if (!sscanf(optarg, "%u", &A.resolution_y)) {
        printf("'-y, --resolution_y' requires numeric argument");
        return ERR;
      }
      break;
    case 'c':
      switch (hash(optarg)) {
      case HASH_M:
        A.spacing = MONOSPACED;
        break;
      case HASH_C:
        A.spacing = CHAR_CELL;
        break;
      default:
        printf("invalid argument for '-s, --style'");
        return ERR;
      }
      break;
    case 'r':
      A.charset_registry = optarg;
      break;
    case 'n':
      if (!sscanf(optarg, "%u", &A.charset_encoding)) {
        printf("'-n, --encoding' requires numeric argument");
        return ERR;
      }
      break;
    default:
      printf("invalid option, run '%s -h' for help", argv[0]);
      return ERR;
    }
  }
  if (argc > optind) {
    i = optind;
    if (!sscanf(argv[i++], "%ux%u", &A.w, &A.h)) {
      printf("invalid dimensions; specify as '[width]x[height]'");
      return ERR;
    }
    char *tok = strtok(argv[i], ",");
    while (tok != NULL) {
      char *name_s = strsep(&tok, "=");
      char *path_s = tok;
      unsigned id;
      // TODO: hashing would still be faster, but this is easier
      for (size_t j = 0; j < SIZE_BLOCKS; ++j)
        if (strcmp(name_s, BlockNames[j]) == 0) {
          id = j;
          break;
        }
      Str path = STR_INIT;
      str_fmt(&path, "%s",
              path_s); // TODO: some sort of append is probably better
      Pair pair = {id, path};
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

void pbm_free(Pbm pbm) { free(pbm.bitmap); }

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
  glyph.swidth_x = (pbm.w * 72) / A.resolution_x; // pbm.w * 100
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

void glyph_free(Glyph glyph) { free(glyph.bitmap); }

Str glyph_gen(Glyph glyph) {
  Str str = STR_INIT;
  str_fmt(&str,
          "STARTCHAR U+%04x\nENCODING %u\nSWIDTH %u %u\nDWIDTH %u %u\nBBX %u "
          "%u %d %d\nBITMAP\n",
          glyph.encoding, glyph.encoding, glyph.swidth_x, glyph.swidth_y,
          glyph.dwidth_x, glyph.dwidth_y, glyph.bbx_w, glyph.bbx_h, glyph.bbx_x,
          glyph.bbx_y);
  for (size_t y = 0; y < glyph.h; ++y) {
    for (size_t x = 0; x < glyph.len; ++x)
      str_fmt(&str, "%02x", glyph.bitmap[y][x]);
    str_push(&str, '\n');
  }
  str_fmt(&str, "ENDCHAR\n");
  glyph_free(glyph);
  return str;
}

Block block_new(BlockID id, char *path) {
  Block block = BLOCK_INIT(id);
  block.glyphs = malloc(block.size * sizeof(Glyph));
  size_t size;
  Pbm pbm = pbm_from(path);
  Pbm *pbms = pbm_repage(&size, pbm);
  if (size < block.size) {
    printf("pbm '%s' is of size %zu but expected %zu; is it missing control "
           "characters?",
           path, block.size, size);
    die("pbm is not an acceptable size");
  }
  unsigned code = BlockRanges[id].start;
  for (size_t i = 0; i < block.size; ++i) {
    Glyph glyph = glyph_new(pbms[i], code);
    block.glyphs[i] = glyph;
    ++code;
  }
  return block;
}

void block_free(Block block) { free(block.glyphs); }

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
  unsigned point_size =
      (A.point_size == 0) ? (A.h * 72) / A.resolution_y : A.point_size;
  char spacing = (A.spacing == MONOSPACED) ? 'm' : 'c';
  str_fmt(&bdf.font, "-%s-%s-%s-%s-%s-%s-%u-%u-%u-%u-%c-%u-%s-%u", A.foundry,
          A.family, A.weight, slant, A.setwidth, A.style, A.h, point_size,
          A.resolution_x, A.resolution_y, spacing,
          (unsigned)(A.point_size * (float)(A.w / A.h)), A.charset_registry,
          A.charset_encoding);
  free(slant);
  bdf.size_pt = point_size;
  bdf.size_xdpi = A.resolution_x;
  bdf.size_ydpi = A.resolution_y;
  bdf.fontboundingbox_w = A.w;
  bdf.fontboundingbox_h = A.h;
  bdf.fontboundingbox_y = -A.descent; // TODO
  bdf.font_ascent = A.h - A.descent;  // TODO
  bdf.font_descent = A.descent;
  size_t len;
  Pair *pairs = (Pair *)vec_mv(&len, A.pairs);
  for (size_t i = 0; i < len; ++i) {
    char *path = str_mv(pairs[i].path);
    Block block = block_new(pairs[i].id, path);
    bdf.chars += block.size;
    vec_push(&bdf.blocks, (char *)&block);
  }
  return bdf;
}

void bdf_gen(Bdf bdf) {
  FILE *fp = fopen(A.output, "w");
  Str str = STR_INIT;
  char *font = str_mv(bdf.font);
  str_fmt(&str,
          "STARTFONT %s\nFONT %s\nSIZE %u %u %u\nFONTBOUNDINGBOX %u %u %d %d\n",
          bdf.startfont, font, bdf.size_pt, bdf.size_xdpi, bdf.size_ydpi,
          bdf.fontboundingbox_w, bdf.fontboundingbox_h, bdf.fontboundingbox_x,
          bdf.fontboundingbox_y);
  free(font);
  if (bdf.properties != 0)
    str_fmt(
        &str,
        "STARTPROPERTIES %u\nFONT_ASCENT %u\nFONT_DESCENT %u\nENDPROPERTIES\n",
        bdf.properties, bdf.font_ascent, bdf.font_descent); // TODO
  str_fmt(&str, "CHARS %u\n", bdf.chars);
  size_t len;
  Block *blocks =
      (Block *)vec_mv(&len, bdf.blocks); // TODO: is this even really necessary
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
