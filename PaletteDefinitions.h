#include <FastLED.h>
//Defines different palettes

// Pulse Palette
DEFINE_GRADIENT_PALETTE( pulse_gradient ) {
  0, 0, 0, 0,
  128, 255, 255, 255,
  255, 0, 0, 0
};

// Gradient palette "bhw1_13_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_13.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 8 bytes of program space.
DEFINE_GRADIENT_PALETTE( bhw1_13_gp ) {
  0, 255, 255, 45, 
  255, 157, 57, 197
};

// Gradient palette "GMT_hot_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/gmt/tn/GMT_hot.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.
DEFINE_GRADIENT_PALETTE( GMT_hot_gp ) {
    0,   0,  0,  0,
   95, 255,  0,  0,
  191, 255,255,  0,
  255, 255,255,255
};


// Gradient palette "purplefly_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/rc/tn/purplefly.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.
DEFINE_GRADIENT_PALETTE( purplefly_gp ) {
  0, 0, 0, 0, 
  63, 239, 0, 122, 
  191, 252, 255, 78,
  255, 0, 0, 0
};

