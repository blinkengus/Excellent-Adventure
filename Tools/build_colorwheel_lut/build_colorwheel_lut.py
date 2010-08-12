import sys
import math
import colorsys
import struct

def RGBToHTMLColor(rgb):
    return '0x00%02x%02x%02x' % rgb

def main():
    
    lut_len = 32
    
    colorwheel_lut = []
    
    for i in range(0, lut_len):
        u = float(i) / float(lut_len)
        r, g, b = colorsys.hsv_to_rgb(u, 1, 1)
        colorwheel_lut.append( RGBToHTMLColor((r * 255, g * 255, b * 255)) )
    
    print(", ".join(colorwheel_lut))

if __name__ == '__main__':
    main()