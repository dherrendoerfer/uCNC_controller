Copyright (C) 2017 D.Herrendoerfer 

# Simple tools and instructions for building a WS2812 based round display:

The geometry of the display of lines of LEDs with the following length:

(6,10,14,16,18,20,22,22,24,24,24,26,26,26,26,24,24,24,22,22,20,18,16,14,10,6)

Like so:
```
         XXXXXX
       XXXXXXXXXX
     XXXXXXXXXXXXXX
    XXXXXXXXXXXXXXXX
   XXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXX
 XXXXXXXXXXXXXXXXXXXXXX
 XXXXXXXXXXXXXXXXXXXXXX
 XXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXX
XXXXXXXXXXXXXXXXXXXXXXXX
 XXXXXXXXXXXXXXXXXXXXXX
 XXXXXXXXXXXXXXXXXXXXXX
 XXXXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXX
  XXXXXXXXXXXXXXXXXXXX
   XXXXXXXXXXXXXXXXXX
    XXXXXXXXXXXXXXXX
     XXXXXXXXXXXXXX
      XXXXXXXXXX
        XXXXXX
```

The sketch will display the images, the tool will generate the source code
from an image.

There are 2 switches (to GND) on D2 and D3, one will advance to the next image,
the other will apply a variation to the image to make simple animations possible,
like a smily giving a wink.
Pressing any switch for more than 2 seconds turns off the display.

