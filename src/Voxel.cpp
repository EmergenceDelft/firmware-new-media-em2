#include "Voxel.h"

int ACTIVE_TRANSPARENCY_ANGLE = 0;
int INACTIVE_TRANSPARENCY_ANGLE = 90;

Voxel::Voxel(ColorMotor* colorMotor, TransparencyMotor* transparencyMotor)
    : colorMotor(*colorMotor), transparencyMotor(*transparencyMotor) {}

void Voxel::update() {
    colorMotor.update();
    transparencyMotor.update();
}



ColorMotor* Voxel::getColorMotor() {
    return &colorMotor;
}

TransparencyMotor* Voxel::getTransparencyMotor() {
    return &transparencyMotor;
}

void Voxel::turnMotorsToUnmeasured(){
    colorMotor.setMovement(true); //this is for color to stop moving on next update
    transparencyMotor.setTargetAngle(INACTIVE_TRANSPARENCY_ANGLE);

}
void Voxel::turnMotorsToMeasured(){
    
    colorMotor.setMovement(false); //this is for color to stop moving on next update
    colorMotor.setTargetAngle(colorMotor.getAngle()); //this is only for color to go to this angle
    transparencyMotor.setTargetAngle(ACTIVE_TRANSPARENCY_ANGLE);

}
void Voxel::turnMotorsToMeasured(int angle){
    colorMotor.setMovement(false); //this is for color to stop moving on next update
    colorMotor.setTargetAngle(angle); //this is only for color to go to this angle
    transparencyMotor.setTargetAngle(ACTIVE_TRANSPARENCY_ANGLE);

}

void Voxel::setJitter(bool jitter){
    colorMotor.setJitter(jitter);
}
