#!python3
import freetype
import zlib
import sys
import re
import math
import argparse
from collections import namedtuple

def special_pair(value):
    special_values = {
        'ascii':     (32, 126),
        'lowercase': (97, 122),
        'uppercase': (65,  90),
        'digits':    (48,  57),
        'space':     (32,  32),
         # Add other special values as needed

        # HTML char names
        'excl':   (33, 33), # !
        'quot':   (34, 34), # "
        'num':    (35, 35), # #
        'dollar': (36, 36), # $
        'percnt': (37, 37), # %
        'amp':    (38, 38), # &
        'apos':   (39, 39), # '
        'lparen': (40, 40), # (
        'rparen': (41, 41), # )
        'ast':    (42, 42), # *
        'plus':   (43, 43), # +
        'comma':  (44, 44), # ,

        'minus':  (45, 45), # - un-official
        'hyphen': (45, 45), # un-official
        'dash':   (45, 45), # un-official

        'period': (46, 46), # .
        'dot':    (46, 46), # un-official

        'sol':    (47, 47), # /
        'slash':  (47, 47), # un-official
        
        'colon':  (58, 58), # :
        'semi':   (59, 59), # ;
        'lt':     (60, 60), # <
        'equals': (61, 61), # =
        'gt':     (62, 62), # >
        'quest':  (63, 63), # ?
        'commat': (64, 64), # @

        'lsqb':   (91, 91), # [

        'bsol':   (92, 92), # \
        'bslash': (92, 92), # un-official

        'rsqb':   (93, 93), # ]
        'hat':    (94, 94), # ^
        'lowbar': (95, 95), # _
        'grave':  (96, 96), # `

        'lcub':   (123, 123), # {
        'verbar': (124, 124), # |
        'rcub':   (125, 125), # }
        'tilde':  (126, 126)  # ~
    }

    # Check if the value is a single character
    if len(value) == 1:
        ascii_code = ord(value)
        return (ascii_code, ascii_code)
    
    value_lower = value.lower()
    
    if value_lower in special_values:
        return special_values[value_lower]

    # Check if the value is a single number (works for multi-digit numbers too)
    if value.isdigit():
        return (int(value), int(value))
    
    # Default behavior: convert value to a tuple of integers
    return tuple(map(int, value.split(',')))

parser = argparse.ArgumentParser(description="Generate a header file from a font to be used with epdiy.")
parser.add_argument("name", action="store", help="name of the font.")
parser.add_argument("size", type=int, help="font size to use.")
parser.add_argument("fontstack", action="store", nargs='+', help="list of font files, ordered by descending priority.")
parser.add_argument("--compress", dest="compress", action="store_true", help="compress glyph bitmaps.")
parser.add_argument("--range", type=special_pair, action='append', help='a closed interval of unicode ranges to include: e.g. 32,126 or 32,32 or a special name.', )
args = parser.parse_args()

GlyphProps = namedtuple("GlyphProps", ["width", "height", "advance_x", "left", "top", "compressed_size", "data_offset", "code_point"])

font_stack = [freetype.Face(f) for f in args.fontstack]
compress = args.compress
size = args.size
font_name = args.name
intervals_raw = [(32, 126)] if args.range is None else args.range


# Step 1: Sort each tuple and sort the list based on the first member of the tuples
sorted_interval_tuples = sorted([tuple(sorted(t)) for t in intervals_raw])

# Step 2: Merge overlapping ranges
merged_list = []
for current_tuple in sorted_interval_tuples:
    if not merged_list or merged_list[-1][1] < current_tuple[0] - 1:
        merged_list.append(current_tuple)
    else:
        merged_list[-1] = (merged_list[-1][0], max(merged_list[-1][1], current_tuple[1]))

intervals = merged_list

def norm_floor(val):
    return int(math.floor(val / (1 << 6)))

def norm_ceil(val):
    return int(math.ceil(val / (1 << 6)))

for face in font_stack:
    # shift by 6 bytes, because sizes are given as 6-bit fractions
    # the display has about 150 dpi.
    face.set_char_size(size << 6, size << 6, 150, 150)

def chunks(l, n):
    for i in range(0, len(l), n):
        yield l[i:i + n]

total_size = 0
total_packed = 0
all_glyphs = []

def load_glyph(code_point):
    face_index = 0
    while face_index < len(font_stack):
        face = font_stack[face_index]
        glyph_index = face.get_char_index(code_point)
        if glyph_index > 0:
            face.load_glyph(glyph_index, freetype.FT_LOAD_RENDER)
            return face
            break
        face_index += 1
        print (f"falling back to font {face_index} for {chr(code_point)}.", file=sys.stderr)
    raise ValueError(f"code point {code_point} not found in font stack!")

for i_start, i_end in intervals:
    for code_point in range(i_start, i_end + 1):
        face = load_glyph(code_point)
        bitmap = face.glyph.bitmap
        pixels = []
        px = 0
        for i, v in enumerate(bitmap.buffer):
            y = i / bitmap.width
            x = i % bitmap.width
            if x % 2 == 0:
                px = (v >> 4)
            else:
                px = px | (v & 0xF0)
                pixels.append(px);
                px = 0
            # eol
            if x == bitmap.width - 1 and bitmap.width % 2 > 0:
                pixels.append(px)
                px = 0

        packed = bytes(pixels);
        total_packed += len(packed)
        compressed = packed
        if compress:
            compressed = zlib.compress(packed)

        glyph = GlyphProps(
            width = bitmap.width,
            height = bitmap.rows,
            advance_x = norm_floor(face.glyph.advance.x),
            left = face.glyph.bitmap_left,
            top = face.glyph.bitmap_top,
            compressed_size = len(compressed),
            data_offset = total_size,
            code_point = code_point,
        )
        total_size += len(compressed)
        all_glyphs.append((glyph, compressed))

# pipe seems to be a good heuristic for the "real" descender
face = load_glyph(ord('|'))

glyph_data = []
glyph_props = []
for index, glyph in enumerate(all_glyphs):
    props, compressed = glyph
    glyph_data.extend([b for b in compressed])
    glyph_props.append(props)

print("ranges", merged_list, file=sys.stderr)
print("total", total_packed, file=sys.stderr)
print("compressed", total_size, file=sys.stderr)

print("#pragma once")
print("#include \"epd_driver.h\"")
print(f"const uint8_t {font_name}Bitmaps[{len(glyph_data)}] = {{")
for c in chunks(glyph_data, 16):
    print ("    " + " ".join(f"0x{b:02X}," for b in c))
print ("};");

print(f"const GFXglyph {font_name}Glyphs[] = {{")
for i, g in enumerate(glyph_props):
    print ("    { " + ", ".join([f"{a}" for a in list(g[:-1])]),"},", f"// {chr(g.code_point) if g.code_point != 92 else '<backslash>'}")
print ("};");

print(f"const UnicodeInterval {font_name}Intervals[] = {{")
offset = 0
for i_start, i_end in intervals:
    # Check if the range is within the printable ASCII zone
    if 32 <= i_start < 127 and 32 <= i_end < 127:
        vis = ' // ' + ''.join(chr(i) for i in range(i_start, i_end + 1) if 32 <= i < 127)
    else:
        vis = ''  # Skip creating the result if the range is outside the printable zone

    print (f"    {{ 0x{i_start:X}, 0x{i_end:X}, 0x{offset:X} }},{vis}")
    offset += i_end - i_start + 1
print ("};");

print(f"const GFXfont {font_name} = {{")
print(f"    (uint8_t*){font_name}Bitmaps,")
print(f"    (GFXglyph*){font_name}Glyphs,")
print(f"    (UnicodeInterval*){font_name}Intervals,")
print(f"    {len(intervals)},")
print(f"    {1 if compress else 0},")
print(f"    {norm_ceil(face.size.height)},")
print(f"    {norm_ceil(face.size.ascender)},")
print(f"    {norm_floor(face.size.descender)},")
print("};")
