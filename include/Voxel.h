#ifndef VOXEL_H
#define VOXEL_H

#include "ColorMotor.h"
#include "TransparencyMotor.h"

class Voxel {
private:
    ColorMotor colorMotor;
    TransparencyMotor transparencyMotor;

public:
    Voxel(ColorMotor* colorMotor, TransparencyMotor* transparencyMotor);

    // Update both motors
    void update();

    // Getters for motors
    ColorMotor* getColorMotor();
    TransparencyMotor* getTransparencyMotor();

    void turnMotorsToUnmeasured();
    void turnMotorsToMeasured();
    void turnMotorsToMeasured(int angle);

    void setJitter(bool jitter);
};

#endif // VOXEL_H
