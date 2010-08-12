import sys
import math

def main():
    
    lut_len = 32
    lut_max = 255
    
    sin_lut = []
    
    for i in range(0, lut_len):
        u = float(i) / float(lut_len)
        
        sinf = (math.sin(u * math.pi * 2.0) + 1.0) / 2.0 * lut_max
        sin_lut.append( int(sinf) )
    
    print('unsigned char sin_lut[] = ' + str(sin_lut) + ';')

if __name__ == '__main__':
    main()