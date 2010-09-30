String packColor(int r, int g, int b)
{
    return "0x" + Integer.toHexString((1 << 15) | (b << 10) | (g << 5) | r);
}

void setup()
{
    colorMode(HSB, 1);
    
    int lut_len = 32;
    int rgb_max = 26;//31;
    
    for(int i = 0; i < lut_len; i++){
        int c = color((float)i / lut_len, 1, 1);
        print(packColor(
            int(red(c) * rgb_max),
            int(green(c) * rgb_max),
            int(blue(c) * rgb_max)
        ));
        if(i + 1 < lut_len)
            print(", ");
    }
    
    exit();
}
