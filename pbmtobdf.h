#define STR_INIT                                                               \
  { 0, 0, NULL }

#define VEC_INIT(S)                                                            \
  { S, 0, 0, NULL }

#define A_INIT                                                                 \
  {                                                                            \
    0, 0, VEC_INIT(sizeof(Pair)), "pbm2.bdf", 0, "", "", "medium", UPRIGHT,    \
        "normal", "", 0, 75, 75, CHAR_CELL, "iso10646", 1                      \
  }

#define HASH_R 0x2b617
#define HASH_I 0x2b60e
#define HASH_O 0x2b614
#define HASH_RI 0x597960
#define HASH_RO 0x597966
#define HASH_OT 0x597908

#define HASH_M 0x2b612
#define HASH_C 0x2b608

#define HELP                                                                   \
  "usage: pbm2bdf [-options] [width]x[height] [<block>],[<block>],[...]\n"     \
  "  -h, --help : this output\n"                                               \
  "  -b, --blocks : list available unicode blocks\n"                           \
  "  -o, --output : <file> : output file\n"                                    \
  "  -d, --descent <number> : descent\n"                                       \
  "  -f, --foundry <string> : foundry\n"                                       \
  "  -a, --family <string> : family\n"                                         \
  "  -w, --weight <string> : weight\n"                                         \
  "  -s, --slant : <slant> : slant (r = roman, i = italic, o = oblique, ri = " \
  "reverse italic, ro = reverse oblique, ot = other)\n"                        \
  "  -e, --setwidth <string> : setwidth\n"                                     \
  "  -t, --style <string> : style\n"                                           \
  "  -p, --point_size <number> : point size\n"                                 \
  "  -x, --resolution_x <number> : horizontal resolution\n"                    \
  "  -y, --resolution_y <number> : vertical resolution\n"                      \
  "  -c, --spacing <spacing> : spacing (m = monospaced, c = char cell)\n"      \
  "  -r, --registry <string> : charset registry\n"                             \
  "  -e, --encoding <number> : charset encoding\n"                             \
  "    (<block> ::= [unicode block name]=[file path])\n"

#define BLOCKS                                                                 \
  X(BASIC_LATIN, "basic_latin", 0x0000, 0x0080)                                \
  X(LATIN_1_SUPPLEMENT, "latin-1_supplement", 0x0080, 0x0100)                  \
  X(LATIN_EXTENDED_A, "latin_extended-a", 0x0100, 0x0180)                      \
  X(LATIN_EXTENDED_B, "latin_extended-b", 0x0180, 0x0250)                      \
  X(IPA_EXTENSIONS, "ipa_extensions", 0x0250, 0x02b0)                          \
  X(SPACING_MODIFIER_LETTERS, "spacing_modifier_letters", 0x02b0, 0x0300)      \
  X(COMBINING_DIACRITICAL_MARKS, "combining_diacritical_marks", 0x0300,        \
    0x0370)                                                                    \
  X(GREEK_AND_COPTIC, "greek_and_coptic", 0x0370, 0x0400)                      \
  X(CYRILLIC, "cyrillic", 0x0400, 0x0500)                                      \
  X(CYRILLIC_SUPPLEMENT, "cyrillic_supplement", 0x0500, 0x0530)                \
  X(ARMENIAN, "armenian", 0x0530, 0x0590)                                      \
  X(HEBREW, "hebrew", 0x0590, 0x0600)                                          \
  X(ARABIC, "arabic", 0x0600, 0x0700)                                          \
  X(SYRIAC, "syriac", 0x0700, 0x0750)                                          \
  X(ARABIC_SUPPLEMENT, "arabic_supplement", 0x0750, 0x0780)                    \
  X(THAANA, "thaana", 0x0780, 0x07c0)                                          \
  X(NKO, "nko", 0x07c0, 0x0800)                                                \
  X(SAMARITAN, "samaritan", 0x0800, 0x0840)                                    \
  X(MANDAIC, "mandaic", 0x0840, 0x0860)                                        \
  X(SYRIAC_SUPPLEMENT, "syriac_supplement", 0x0860, 0x0870)                    \
  X(ARABIC_EXTENDED_B, "arabic_extended-b", 0x0870, 0x08a0)                    \
  X(ARABIC_EXTENDED_A, "arabic_extended-a", 0x08a0, 0x0900)                    \
  X(DEVANAGARI, "devanagari", 0x0900, 0x0980)                                  \
  X(BENGALI, "bengali", 0x0980, 0x0a00)                                        \
  X(GURMUKHI, "gurmukhi", 0x0a00, 0x0a80)                                      \
  X(GUJARATI, "gujarati", 0x0a80, 0x0b00)                                      \
  X(ORIYA, "oriya", 0x0b00, 0x0b80)                                            \
  X(TAMIL, "tamil", 0x0b80, 0x0c00)                                            \
  X(TELUGU, "telugu", 0x0c00, 0x0c80)                                          \
  X(KANNADA, "kannada", 0x0c80, 0x0d00)                                        \
  X(MALAYALAM, "malayalam", 0x0d00, 0x0d80)                                    \
  X(SINHALA, "sinhala", 0x0d80, 0x0e00)                                        \
  X(THAI, "thai", 0x0e00, 0x0e80)                                              \
  X(LAO, "lao", 0x0e80, 0x0f00)                                                \
  X(TIBETAN, "tibetan", 0x0f00, 0x1000)                                        \
  X(MYANMAR, "myanmar", 0x1000, 0x10a0)                                        \
  X(GEORGIAN, "georgian", 0x10a0, 0x1100)                                      \
  X(HANGUL_JAMO, "hangul_jamo", 0x1100, 0x1200)                                \
  X(ETHIOPIC, "ethiopic", 0x1200, 0x1380)                                      \
  X(ETHIOPIC_SUPPLEMENT, "ethiopic_supplement", 0x1380, 0x13a0)                \
  X(CHEROKEE, "cherokee", 0x13a0, 0x1400)                                      \
  X(UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS,                                     \
    "unified_canadian_aboriginal_syllabics", 0x1400, 0x1680)                   \
  X(OGHAM, "ogham", 0x1680, 0x16a0)                                            \
  X(RUNIC, "runic", 0x16a0, 0x1700)                                            \
  X(TAGALOG, "tagalog", 0x1700, 0x1720)                                        \
  X(HANUNOO, "hanunoo", 0x1720, 0x1740)                                        \
  X(BUHID, "buhid", 0x1740, 0x1760)                                            \
  X(TAGBANWA, "tagbanwa", 0x1760, 0x1780)                                      \
  X(KHMER, "khmer", 0x1780, 0x1800)                                            \
  X(MONGOLIAN, "mongolian", 0x1800, 0x18b0)                                    \
  X(UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED,                            \
    "unified_canadian_aboriginal_syllabics_extended", 0x18b0, 0x1900)          \
  X(LIMBU, "limbu", 0x1900, 0x1950)                                            \
  X(TAI_LE, "tai_le", 0x1950, 0x1980)                                          \
  X(NEW_TAI_LUE, "new_tai_lue", 0x1980, 0x19e0)                                \
  X(KHMER_SYMBOLS, "khmer_symbols", 0x19e0, 0x1a00)                            \
  X(BUGINESE, "buginese", 0x1a00, 0x1a20)                                      \
  X(TAI_THAM, "tai_tham", 0x1a20, 0x1ab0)                                      \
  X(COMBINING_DIACRITICAL_MARKS_EXTENDED,                                      \
    "combining_diacritical_marks_extended", 0x1ab0, 0x1b00)                    \
  X(BALINESE, "balinese", 0x1b00, 0x1b80)                                      \
  X(SUNDANESE, "sundanese", 0x1b80, 0x1bc0)                                    \
  X(BATAK, "batak", 0x1bc0, 0x1c00)                                            \
  X(LEPCHA, "lepcha", 0x1c00, 0x1c50)                                          \
  X(OL_CHIKI, "ol_chiki", 0x1c50, 0x1c80)                                      \
  X(CYRILLIC_EXTENDED_C, "cyrillic_extended-c", 0x1c80, 0x1c90)                \
  X(GEORGIAN_EXTENDED, "georgian_extended", 0x1c90, 0x1cc0)                    \
  X(SUNDANESE_SUPPLEMENT, "sundanese_supplement", 0x1cc0, 0x1cd0)              \
  X(VEDIC_EXTENSIONS, "vedic_extensions", 0x1cd0, 0x1d00)                      \
  X(PHONETIC_EXTENSIONS, "phonetic_extensions", 0x1d00, 0x1d80)                \
  X(PHONETIC_EXTENSIONS_SUPPLEMENT, "phonetic_extensions_supplement", 0x1d80,  \
    0x1dc0)                                                                    \
  X(COMBINING_DIACRITICAL_MARKS_SUPPLEMENT,                                    \
    "combining_diacritical_marks_supplement", 0x1dc0, 0x1e00)                  \
  X(LATIN_EXTENDED_ADDITIONAL, "latin_extended_additional", 0x1e00, 0x1f00)    \
  X(GREEK_EXTENDED, "greek_extended", 0x1f00, 0x2000)                          \
  X(GENERAL_PUNCTUATION, "general_punctuation", 0x2000, 0x2070)                \
  X(SUPERSCRIPTS_AND_SUBSCRIPTS, "superscripts_and_subscripts", 0x2070,        \
    0x20a0)                                                                    \
  X(CURRENCY_SYMBOLS, "currency_symbols", 0x20a0, 0x20d0)                      \
  X(COMBINING_DIACRITICAL_MARKS_FOR_SYMBOLS,                                   \
    "combining_diacritical_marks_for_symbols", 0x20d0, 0x2100)                 \
  X(LETTERLIKE_SYMBOLS, "letterlike_symbols", 0x2100, 0x2150)                  \
  X(NUMBER_FORMS, "number_forms", 0x2150, 0x2190)                              \
  X(ARROWS, "arrows", 0x2190, 0x2200)                                          \
  X(MATHEMATICAL_OPERATORS, "mathematical_operators", 0x2200, 0x2300)          \
  X(MISCELLANEOUS_TECHNICAL, "miscellaneous_technical", 0x2300, 0x2400)        \
  X(CONTROL_PICTURES, "control_pictures", 0x2400, 0x2440)                      \
  X(OPTICAL_CHARACTER_RECOGNITION, "optical_character_recognition", 0x2440,    \
    0x2460)                                                                    \
  X(ENCLOSED_ALPHANUMERICS, "enclosed_alphanumerics", 0x2460, 0x2500)          \
  X(BOX_DRAWING, "box_drawing", 0x2500, 0x2580)                                \
  X(BLOCK_ELEMENTS, "block_elements", 0x2580, 0x25a0)                          \
  X(GEOMETRIC_SHAPES, "geometric_shapes", 0x25a0, 0x2600)                      \
  X(MISCELLANEOUS_SYMBOLS, "miscellaneous_symbols", 0x2600, 0x2700)            \
  X(DINGBATS, "dingbats", 0x2700, 0x27c0)                                      \
  X(MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A,                                      \
    "miscellaneous_mathematical_symbols-a", 0x27c0, 0x27f0)                    \
  X(SUPPLEMENTAL_ARROWS_A, "supplemental_arrows-a", 0x27f0, 0x2800)            \
  X(BRAILLE_PATTERNS, "braille_patterns", 0x2800, 0x2900)                      \
  X(SUPPLEMENTAL_ARROWS_B, "supplemental_arrows-b", 0x2900, 0x2980)            \
  X(MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B,                                      \
    "miscellaneous_mathematical_symbols-b", 0x2980, 0x2a00)                    \
  X(SUPPLEMENTAL_MATHEMATICAL_OPERATORS,                                       \
    "supplemental_mathematical_operators", 0x2a00, 0x2b00)                     \
  X(MISCELLANEOUS_SYMBOLS_AND_ARROWS, "miscellaneous_symbols_and_arrows",      \
    0x2b00, 0x2c00)                                                            \
  X(GLAGOLITIC, "glagolitic", 0x2c00, 0x2c60)                                  \
  X(LATIN_EXTENDED_C, "latin_extended-c", 0x2c60, 0x2c80)                      \
  X(COPTIC, "coptic", 0x2c80, 0x2d00)                                          \
  X(GEORGIAN_SUPPLEMENT, "georgian_supplement", 0x2d00, 0x2d30)                \
  X(TIFINAGH, "tifinagh", 0x2d30, 0x2d80)                                      \
  X(ETHIOPIC_EXTENDED, "ethiopic_extended", 0x2d80, 0x2de0)                    \
  X(CYRILLIC_EXTENDED_A, "cyrillic_extended-a", 0x2de0, 0x2e00)                \
  X(SUPPLEMENTAL_PUNCTUATION, "supplemental_punctuation", 0x2e00, 0x2e80)      \
  X(CJK_RADICALS_SUPPLEMENT, "cjk_radicals_supplement", 0x2e80, 0x2f00)        \
  X(KANGXI_RADICALS, "kangxi_radicals", 0x2f00, 0x2ff0)                        \
  X(IDEOGRAPHIC_DESCRIPTION_CHARACTERS, "ideographic_description_characters",  \
    0x2ff0, 0x3000)                                                            \
  X(CJK_SYMBOLS_AND_PUNCTUATION, "cjk_symbols_and_punctuation", 0x3000,        \
    0x3040)                                                                    \
  X(HIRAGANA, "hiragana", 0x3040, 0x30a0)                                      \
  X(KATAKANA, "katakana", 0x30a0, 0x3100)                                      \
  X(BOPOMOFO, "bopomofo", 0x3100, 0x3130)                                      \
  X(HANGUL_COMPATIBILITY_JAMO, "hangul_compatibility_jamo", 0x3130, 0x3190)    \
  X(KANBUN, "kanbun", 0x3190, 0x31a0)                                          \
  X(BOPOMOFO_EXTENDED, "bopomofo_extended", 0x31a0, 0x31c0)                    \
  X(CJK_STROKES, "cjk_strokes", 0x31c0, 0x31f0)                                \
  X(KATAKANA_PHONETIC_EXTENSIONS, "katakana_phonetic_extensions", 0x31f0,      \
    0x3200)                                                                    \
  X(ENCLOSED_CJK_LETTERS_AND_MONTHS, "enclosed_cjk_letters_and_months",        \
    0x3200, 0x3300)                                                            \
  X(CJK_COMPATIBILITY, "cjk_compatibility", 0x3300, 0x3400)                    \
  X(CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A, "cjk_unified_ideographs_extension-a",  \
    0x3400, 0x4dc0)                                                            \
  X(YIJING_HEXAGRAM_SYMBOLS, "yijing_hexagram_symbols", 0x4dc0, 0x4e00)        \
  X(CJK_UNIFIED_IDEOGRAPHS, "cjk_unified_ideographs", 0x4e00, 0xa000)          \
  X(YI_SYLLABLES, "yi_syllables", 0xa000, 0xa490)                              \
  X(YI_RADICALS, "yi_radicals", 0xa490, 0xa4d0)                                \
  X(LISU, "lisu", 0xa4d0, 0xa500)                                              \
  X(VAI, "vai", 0xa500, 0xa640)                                                \
  X(CYRILLIC_EXTENDED_B, "cyrillic_extended-b", 0xa640, 0xa6a0)                \
  X(BAMUM, "bamum", 0xa6a0, 0xa700)                                            \
  X(MODIFIER_TONE_LETTERS, "modifier_tone_letters", 0xa700, 0xa720)            \
  X(LATIN_EXTENDED_D, "latin_extended-d", 0xa720, 0xa800)                      \
  X(SYLOTI_NAGRI, "syloti_nagri", 0xa800, 0xa830)                              \
  X(COMMON_INDIC_NUMBER_FORMS, "common_indic_number_forms", 0xa830, 0xa840)    \
  X(PHAGS_PA, "phags_pa", 0xa840, 0xa880)                                      \
  X(SAURASHTRA, "saurashtra", 0xa880, 0xa8e0)                                  \
  X(DEVANAGARI_EXTENDED, "devanagari_extended", 0xa8e0, 0xa900)                \
  X(KAYAH_LI, "kayah_li", 0xa900, 0xa930)                                      \
  X(REJANG, "rejang", 0xa930, 0xa960)                                          \
  X(HANGUL_JAMO_EXTENDED_A, "hangul_jamo_extended-a", 0xa960, 0xa980)          \
  X(JAVANESE, "javanese", 0xa980, 0xa9e0)                                      \
  X(MYANMAR_EXTENDED_B, "myanmar_extended-b", 0xa9e0, 0xaa00)                  \
  X(CHAM, "cham", 0xaa00, 0xaa60)                                              \
  X(MYANMAR_EXTENDED_A, "myanmar_extended-a", 0xaa60, 0xaa80)                  \
  X(TAI_VIET, "tai_viet", 0xaa80, 0xaae0)                                      \
  X(MEETEI_MAYEK_EXTENSIONS, "meetei_mayek_extensions", 0xaae0, 0xab00)        \
  X(ETHIOPIC_EXTENDED_A, "ethiopic_extended-a", 0xab00, 0xab30)                \
  X(LATIN_EXTENDED_E, "latin_extended-e", 0xab30, 0xab70)                      \
  X(CHEROKEE_SUPPLEMENT, "cherokee_supplement", 0xab70, 0xabc0)                \
  X(MEETEI_MAYEK, "meetei_mayek", 0xabc0, 0xac00)                              \
  X(HANGUL_SYLLABLES, "hangul_syllables", 0xac00, 0xd7b0)                      \
  X(HANGUL_JAMO_EXTENDED_B, "hangul_jamo_extended-b", 0xd7b0, 0xd800)          \
  X(HIGH_SURROGATES, "high_surrogates", 0xd800, 0xdb80)                        \
  X(HIGH_PRIVATE_USE_SURROGATES, "high_private_use_surrogates", 0xdb80,        \
    0xdc00)                                                                    \
  X(LOW_SURROGATES, "low_surrogates", 0xdc00, 0xe000)                          \
  X(PRIVATE_USE_AREA, "private_use_area", 0xe000, 0xf900)                      \
  X(CJK_COMPATIBILITY_IDEOGRAPHS, "cjk_compatibility_ideographs", 0xf900,      \
    0xfb00)                                                                    \
  X(ALPHABETIC_PRESENTATION_FORMS, "alphabetic_presentation_forms", 0xfb00,    \
    0xfb50)                                                                    \
  X(ARABIC_PRESENTATION_FORMS_A, "arabic_presentation_forms-a", 0xfb50,        \
    0xfe00)                                                                    \
  X(VARIATION_SELECTORS, "variation_selectors", 0xfe00, 0xfe10)                \
  X(VERTICAL_FORMS, "vertical_forms", 0xfe10, 0xfe20)                          \
  X(COMBINING_HALF_MARKS, "combining_half_marks", 0xfe20, 0xfe30)              \
  X(CJK_COMPATIBILITY_FORMS, "cjk_compatibility_forms", 0xfe30, 0xfe50)        \
  X(SMALL_FORM_VARIANTS, "small_form_variants", 0xfe50, 0xfe70)                \
  X(ARABIC_PRESENTATION_FORMS_B, "arabic_presentation_forms-b", 0xfe70,        \
    0xff00)                                                                    \
  X(HALFWIDTH_AND_FULLWIDTH_FORMS, "halfwidth_and_fullwidth_forms", 0xff00,    \
    0xfff0)                                                                    \
  X(SPECIALS, "specials", 0xfff0, 0xffff)

#define PBM_INIT(W, H, L)                                                      \
  { W, H, L, NULL }

#define GLYPH_INIT                                                             \
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL }

#define BLOCK_INIT(N)                                                          \
  { N, (BlockRanges[(N)].end - BlockRanges[(N)].start), NULL }

#define BDF_INIT                                                               \
  {                                                                            \
    "2.1", STR_INIT, 0, 75, 75, 0, 0, 0, 0, 2, 0, 0, 0,                        \
        VEC_INIT(sizeof(Block))                                                \
  }
