# GUN4IR-edited-recoil
My spin on the free GUN4IR with added recoil functionality.

Currently I have built three guns, all that I have made as a variation of https://github.com/henning2323/IR_LightGun

The notable difference between Henning's build and Jayingbee's official GUN4IR system is that GUN4IR costs money, is slightly more software focused, and as far as I know, is locked to use certain pins.

Henning's build runs exclusively on the arduino, requiring no software to run, the cost being calibration taking a little longer, needing to actually open the arduino IDE, in exchange for customizability.

My addition to this build was adding basic recoil support.

DESERT EAGLE

My first practice run building Henninng's system. It is just the camera and the trigger. It does have recoil support, but from a seperate system outside of this repository. The only changes are to buttons and pins on the arduino.

SHOTGUN

Added singlefire recoil, and a second button in the pump for pump action. Also my first attempt at using a transistor.

MAC10

Added support for a joystick, autofire recoil, and a way to switch between singlefire and autofire. I also experiments with using multiple solenoids for recoil. I had the idea to make them all fire seperately so that I can have a faster 'fire rate', but it didn't feel as great as I thought it would, so I just have them fire all at the same time.
