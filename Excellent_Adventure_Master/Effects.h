#ifndef _EFFECTS_H
#define _EFFECTS_H
#include "EffectManager.h"

#define NEWEFFECT(NAME) int NAME (Canvas *c, EffectManager *em, char mode)

#define EFFECT(NAME,DURATION)    {&(NAME), (DURATION * 8)}

// Idle effects
NEWEFFECT( BlitzyIdle           );


NEWEFFECT( RingRainbow          );
NEWEFFECT( RingRadio            ); 
NEWEFFECT( RingFlash            );  
NEWEFFECT( RingSolid            );


//NEWEFFECT( SimpleSpectrum       );
NEWEFFECT( WarpSpectrum         );
NEWEFFECT( PinwheelSpectrum     );
NEWEFFECT( ElevatorSpectrum     );
NEWEFFECT( SolidColors          );
NEWEFFECT( SolidSpectrum        );
NEWEFFECT( SpotlightSpectrum    );
//NEWEFFECT( SimpleColumns        );
NEWEFFECT( Spotlight            );
//NEWEFFECT( CheckerBoard         );
//NEWEFFECT( Glitz                );
//NEWEFFECT( Barrel               );
//NEWEFFECT( LightTornado         );
//NEWEFFECT( SoothingRiser        );
//NEWEFFECT( SoulWavesVertical    );
//NEWEFFECT( TheCMYKShuffle       );
//NEWEFFECT( Barrel               );
NEWEFFECT( Accumulator          );


NEWEFFECT( Overtime             );


/*
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
NEWEFFECT(  );
*/    
    /*
int BlitzyIdle        (Canvas *c, EffectManager *em, char mode);

// Ring Effects
int RingRainbow       (Canvas *c, EffectManager *em, char mode);
int RingRadio         (Canvas *c, EffectManager *em, char mode);
int RingFlash         (Canvas *c, EffectManager *em, char mode);

// More effects!
int SimpleSpectrum    (Canvas *c, EffectManager *em, char mode);
int WarpSpectrum      (Canvas *c, EffectManager *em, char mode);
int PinwheelSpectrum  (Canvas *c, EffectManager *em, char mode);
int ElevatorSpectrum  (Canvas *c, EffectManager *em, char mode);
int SolidColors       (Canvas *c, EffectManager *em, char mode);
int SolidSpectrum     (Canvas *c, EffectManager *em, char mode);
int SpotlightSpectrum (Canvas *c, EffectManager *em, char mode);
                      
int SimpleColumns     (Canvas *c, EffectManager *em, char mode);
int Spotlight         (Canvas *c, EffectManager *em, char mode);
int CheckerBoard      (Canvas *c, EffectManager *em, char mode);
int Glitz             (Canvas *c, EffectManager *em, char mode);
int Barrel            (Canvas *c, EffectManager *em, char mode);
int LightTornado      (Canvas *c, EffectManager *em, char mode);
int LightTornado      (Canvas *c, EffectManager *em, char mode);
*/
// Overtime effects
// What to do when people have taken too long
//int Overtime          (Canvas *c, EffectManager *em, char mode);


// Ideas
// Single Color Pulsing with Single Pixel
// Pulsing Spotlight on Phone


#define EFFECTS_NUM_IDLE 1
#define EFFECTS_NUM_RING 1
#define EFFECTS_NUM_CALL 7
#define EFFECTS_NUM_OVER 1
static Effect effectsIdle[EFFECTS_NUM_IDLE] =
{ 
    EFFECT(BlitzyIdle, 64)
//    EFFECT( Glitz , 64)
};

static Effect effectsRing[EFFECTS_NUM_RING] =
{ 
    //EFFECT(RingFlash, 128),
    //EFFECT(RingRainbow, 128)
    //EFFECT(RingRadio, 128)
    EFFECT(RingSolid, 128)
//,   EFFECT( Glitz , 64)
};

static Effect effectsCall[EFFECTS_NUM_CALL] =
{
    EFFECT(Accumulator, 256),
    EFFECT(SpotlightSpectrum, 256),
    EFFECT(WarpSpectrum, 256),
    EFFECT(PinwheelSpectrum, 128),
    EFFECT(SolidSpectrum, 512),
    EFFECT(SolidColors, 256),
    EFFECT(ElevatorSpectrum, 256)
};

static Effect effectsOver[EFFECTS_NUM_OVER] =
{ 
    EFFECT( Overtime , 64)
};



#endif
