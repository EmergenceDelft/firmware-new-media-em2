#ifndef VOXEL_H
#define VOXEL_H

#include "ColourMotor.h"
#include "TransparencyMotor.h"

class Voxel {
private:
    //every voxel owns 2 motors
    ColourMotor colourMotor;
    TransparencyMotor transparencyMotor;

public:
    Voxel(ColourMotor* colourMotor, TransparencyMotor* transparencyMotor);

    // Update both motors
    void update();

    // Getters for motors
    ColourMotor* getColourMotor();
    TransparencyMotor* getTransparencyMotor();

    void turnMotorsToUnmeasured();
    void turnMotorsToMeasured();
    void turnMotorsToMeasured(int angle);

    void setColourMotorJitter(bool jitter);
    void setTransparencyMotorJitter(bool jitter);
};

#endif // VOXEL_H
 