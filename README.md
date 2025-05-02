# pbmtobdf
A tool to convert sets of PBM images into monospaced BDF bitmap fonts.

## Usage
1. Determine the width and height of your glyphs; it must be consistent across all glyphs.
2. For every Unicode block you wish to include in your font, create a PBM image of a grid of glyphs where the number of glyphs is at least equal to the length of the relevant Unicode block. Extra glyphs will be omitted.
3. `pbmtobdf [width]x[height] [block]=[image.pbm]`

And if you want to include multiple Unicode blocks,
`pbmtobdf [width]x[height] [block]=[image.pbm],[block2]=[image2.pbm],[...]`

## FAQ
Q: How can I convert \[image format\] to PBM?

A: Use [ImageMagick](https://imagemagick.org/index.php)
```bash
for file in *.jpg; do
  base=$(basename $file .jpg)
  magick $file $base.pbm
done
```
Q: How can I convert BDF to \[font format\]?

A: There are a variety of tools to do this, but my preferred ones are bdftopcf and [FontForge](https://fontforge.org/en-US/)
```bash
bdftopcf -p1 -u1 -m -M -t -o pbmto.pcf pbmto.bdf
fontforge -script build.pe pbmto.bdf
```
Where `build.pe` is
```
Open($1)
SetFontNames("pbmto", "pbmto", "pbmto", "Regular")
Generate($1:r + ".otb", "", 0x100)
Quit()
```
