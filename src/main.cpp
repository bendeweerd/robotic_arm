#include <Arduino.h>
#include <FABRIK2D.h>
#include <stepper.h>

#define base_dir_pin 10       //bottom left on A4988 driver
#define base_step_pin 11      //second to bottom left
#define shoulder_dir_pin 8
#define shoulder_step_pin 9

unsigned base_acceleration = 200;
unsigned base_max_speed = 500;

unsigned shoulder_acceleration = 1000;
unsigned shoulder_max_speed = 4000;

#define base_hall_pin 2
#define shoulder_hall_pin 3
#define led_pin 7

// Fabrik2D initialization, lengths in mm
int lengths[] = {150, 150};
Fabrik2D fabrik2D(3, lengths);

// step_pin, dir_pin, acceleration, max_speed, microstepping resolution, gear reduction
//TODO: find actual base reduction
Stepper baseStepper(base_step_pin, base_dir_pin, base_acceleration, base_max_speed, 4, -4);
Stepper shoulderStepper(shoulder_step_pin, shoulder_dir_pin, shoulder_acceleration, shoulder_max_speed, 4, 100);

//create array of steppers, used for run() method
#define NUM_STEPPERS 2
Stepper* steppers[NUM_STEPPERS];

/*
  run() takes an array of stepper addresses as input
  and runs them until all movement is complete

  this will block until all motors have reached their goals
*/
void run(Stepper* steppers[]){
  while(true){    //repeat until all motors have completed
    bool is_complete = true;    //assume it's completed, check each motor to verify
    for(int i = 0; i < NUM_STEPPERS; i++){
      steppers[i]->run();
      if(steppers[i]->isRunning()){
        is_complete = false;
      }
    }
    if(is_complete) break;    //if is_complete is still true, all the motors have completed and we can exit
  }  
}

void setup()
{
  Serial.begin(9600);

  //the AccelStepper constructor should do these automatically
  // pinMode(base_step_pin, OUTPUT);
  // pinMode(base_dir_pin, OUTPUT);

  pinMode(base_hall_pin, INPUT);
  pinMode(shoulder_hall_pin, INPUT);
  pinMode(led_pin, OUTPUT);

  digitalWrite(base_step_pin, LOW); //helps prevent jittering at startup
  digitalWrite(shoulder_step_pin, LOW);
  digitalWrite(led_pin, LOW);       //init led to low

  steppers[0] = &baseStepper;
  steppers[1] = &shoulderStepper;

  //set Fabrik2D tolerance, same units as before (mm)
  fabrik2D.setTolerance(0.5);

  // Zero from top to bottom to avoid collisions
  shoulderStepper.zero(shoulder_hall_pin, led_pin, -2000);
  baseStepper.zero(base_hall_pin, led_pin, 500);
}

//impelement setAngle() methods to set goal of each stepper, 
//then a single run() method to run until they've all reached their goals

void loop()
{

  //solve IK for positioning at (100, 100) mm
  fabrik2D.solve(100, 100, lengths);

  //get angles (in radians [-pi, pi]) and convert them to degrees [-180, 180]
  float shoulderAngle = fabrik2D.getAngle(0) * RAD_TO_DEG;
  float elbowAngle = fabrik2D.getAngle(1) * RAD_TO_DEG;

  Serial.println("Moving End Effector to (100, 100):");
  Serial.println("\tJoint 1 location: (" + String(fabrik2D.getX(1)) + ", " + String(fabrik2D.getY(1)) + ")");
  Serial.println("\tJoint 2 location: (" + String(fabrik2D.getX(2)) + ", " + String(fabrik2D.getY(2)) + ")");
  Serial.println("\tShoulder angle: " + String(shoulderAngle));
  Serial.println("\tElbow angle: " + String(elbowAngle));

  delay(2000);

  //solve IK for positioning at (200, 200) mm
  fabrik2D.solve(200, 200, lengths);

  //get angles (in radians [-pi, pi]) and convert them to degrees [-180, 180]
  shoulderAngle = fabrik2D.getAngle(0) * RAD_TO_DEG;
  elbowAngle = fabrik2D.getAngle(1) * RAD_TO_DEG;

  Serial.println("Moving End Effector to (200, 200):");
  Serial.println("\tJoint 1 location: (" + String(fabrik2D.getX(1)) + ", " + String(fabrik2D.getY(1)) + ")");
  Serial.println("\tJoint 2 location: (" + String(fabrik2D.getX(2)) + ", " + String(fabrik2D.getY(2)) + ")");
  Serial.println("\tShoulder angle: " + String(shoulderAngle));
  Serial.println("\tElbow angle: " + String(elbowAngle));

  delay(2000);

  // baseStepper.setAngle(90);
  // shoulderStepper.setAngle(45);
  // run(steppers);

  // baseStepper.setAngle(0);
  // shoulderStepper.setAngle(0);
  // run(steppers);
}