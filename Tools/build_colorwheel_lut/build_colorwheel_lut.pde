String packColor(int r, int g, int b)
{
    return "0x" + Integer.toHexString((1 << 15) | (b << 10) | (g << 5) | r);
}

void setup()
{
    colorMode(HSB, 1);
    
    int lut_len = 32;
    
    for(int i = 0; i < lut_len; i++){
        int c = color((float)i / lut_len, 1, 1);
        print(packColor(
            int(red(c) * 31),
            int(green(c) * 31),
            int(blue(c) * 31)
        ));
        if(i + 1 < lut_len)
            print(", ");
    }
    
    exit();
}
