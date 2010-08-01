void convertHsvToRgb(const unsigned char hsv[3], unsigned char rgb[3])
{
    if(hsv[2] == 0){ // black
        set(rgb, 0);
    } else if(hsv[1] == 0) { // grays
    set(rgb, hsv[2]);
    } else {
    float h = hsv[0] * 6. / 255.;
    float s = hsv[1] / 255.;
    int hCategory = (int) floorf(h);
    float hRemainder = h - hCategory;
    unsigned char pv = (unsigned char) ((1.f - s) * hsv[2]);
    unsigned char qv = (unsigned char) ((1. - s * hRemainder) * hsv[2]);
    unsigned char tv = (unsigned char) ((1. - s * (1. - hRemainder)) * hsv[2]);
    switch(hCategory) {
    case 0: // r
    rgb[0] = hsv[2];
    rgb[1] = tv;
    rgb[2] = pv;
    break;
    case 1: // g
    rgb[0] = qv;
    rgb[1] = hsv[2];
    rgb[2] = pv;
    break;
    case 2:
    rgb[0] = pv;
    rgb[1] = hsv[2];
    rgb[2] = tv;
    break;
    case 3: // b
    rgb[0] = pv;
    rgb[1] = qv;
    rgb[2] = hsv[2];
    break;
    case 4:
    rgb[0] = tv;
    rgb[1] = pv;
    rgb[2] = hsv[2];
    break;
    case 5: // back to r
    rgb[0] = hsv[2];
    rgb[1] = pv;
    rgb[2] = qv;
    break;
    }
    }
}