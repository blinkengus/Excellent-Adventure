/**
 *
 * Example sketch. 
 * The update function needs to go in the switch statement in the main loop()
 *
 */

void me_update() {
    for(int y = 0; y < YCOUNT; y++) {
        for(int x = 0; x < XCOUNT; x++) {
            setPixel(x, y,
                (int)map(sin(frameCount / 100.0f + x), -1, 1, 0, 255),
                (int)map(sin(frameCount / 68.0f  + y),  -1, 1, 0, 255),
                (int)map(sin(frameCount / 32.0f  + x+y),  -1, 1, 0, 255)
            );
        }
    }
}

