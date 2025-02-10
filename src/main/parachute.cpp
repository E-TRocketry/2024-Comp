#include <Servo.h>
#include<parachute.h>
#include <Sensors.cpp>

Servo doorServo;

int doorServoPin = 10; //TODO: CHANGE TO CORRECT PIN!!!
float altThreshold = 182.88; //Meters - CHANGE TO CORRECT THRESHOLD!!! 182.88 meters = 600 feet

float altitude = readAltitudeFromBMP();

bool altitudePassed = false
bool doorOpened = false;

int openDoorServo = 150; //CHANGE TO CORRECT ANGLE!!!
int closeDoorServo = -2;

void initializeParachute(){
    doorServo.attach(doorServoPin);
    initializeSensors();
    doorServo.write(closeDoorServo);
}

void deployParachute(){
    if(altitudePassed == false && altitude >= altThreshold){
        altitudePassed = true;
    } else {
        altitudePassed = false;
    }
    if (altitudePassed == true && altitude <= altThreshold && doorOpened == false){
        doorServo.write(openDoorServo);
        doorOpened = true;
        delay(3000);
    } else{
        doorOpened = false;
        }
}

void closeParachuteServo(){
    if(doorOpened == true && altitude <= altThreshold){
        doorServo.write(closeDoorServo);
        doorOpened = false;
        altitudePassed = false;
    } else 
        altitudePassed = true;
    }
}