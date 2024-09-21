#include "Voxel.h"

int ACTIVE_TRANSPARENCY_ANGLE = 0;
int INACTIVE_TRANSPARENCY_ANGLE = 90;

Voxel::Voxel(ColourMotor* colourMotor, TransparencyMotor* transparencyMotor)
    : colourMotor(*colourMotor), transparencyMotor(*transparencyMotor) {}

void Voxel::update() {
    colourMotor.update();
    transparencyMotor.update();
}


ColourMotor* Voxel::getColourMotor() {
    return &colourMotor;
}

TransparencyMotor* Voxel::getTransparencyMotor() {
    return &transparencyMotor;
}

//quite straightforward functions
//this is where measured, unmeasured and measured but go to entangeld angles are defined
void Voxel::turnMotorsToUnmeasured(){
    colourMotor.setMovement(true); //this is for colour to stop moving on next update
    transparencyMotor.setTargetAngle(transparencyMotor.getInactiveAngle());
}
void Voxel::turnMotorsToMeasured(){
    
    colourMotor.setMovement(false); //this is for colour to stop moving on next update
    colourMotor.setTargetAngle(colourMotor.getAngle()); //this is only for colour to go to this angle
    transparencyMotor.setTargetAngle(transparencyMotor.getActiveAngle());

}
void Voxel::turnMotorsToMeasured(int angle){
    colourMotor.setMovement(false); //this is for colour to stop moving on next update
    colourMotor.setTargetAngle(angle); //this is for colour going to entangled angle
    transparencyMotor.setTargetAngle(transparencyMotor.getActiveAngle());
}

void Voxel::setColourMotorJitter(bool jitter){
    colourMotor.setJitter(jitter);
}

void Voxel::setTransparencyMotorJitter(bool jitter){
    transparencyMotor.setJitter(jitter);
}
