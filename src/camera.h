/*
* Aquest mòdul s'encarrega de la gestió de la càmara i la captura d'imgatges
*/

#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>

void camera_init();

String capture_photo();
#endif