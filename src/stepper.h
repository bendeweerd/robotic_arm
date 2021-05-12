#include <AccelStepper.h>

class Stepper
{
public:
    Stepper(const int step_pin, const int dir_pin, unsigned acceleration, unsigned max_speed, long micro_res, double gear_red);
    void zero(int hall_pin, int led_pin, int speed);
    long setAngle(double angle);

    bool isRunning() { return myStepper->isRunning(); }
    void run() { myStepper->run(); }

private:
    AccelStepper *myStepper;
    int my_angle; // -360 < my_angle < 360
    long steps_per_output_rotation;
};

/*
Stepper Class Constructor - creates a new wrapped AccelStepper object and initializes instance variables
parameters:
    step_pin, pin output to step_pin on A4988 driver
    dir_pin, pin output to dir_pin on A4988 driver
    acceleration, the acceleration of the stepper in steps/s^2
    max_speed, the maximum angular velocity of the stepper in steps/s
    micro_res, resolution the driver is microstepping on
    gear_red, gear reduction of output shaft attachment, to calculate actual rotation
*/
Stepper::Stepper(const int step_pin, const int dir_pin, unsigned acceleration, unsigned max_speed, long micro_res, double gear_red)
{
    myStepper = new AccelStepper(AccelStepper::DRIVER, step_pin, dir_pin);
    myStepper->setAcceleration(acceleration);
    myStepper->setMaxSpeed(max_speed);
    my_angle = 0;

    //had issues with data types here
    //see https://arduino.stackexchange.com/questions/20379/multiplication-division-what-is-wrong
    steps_per_output_rotation = 200L * micro_res * gear_red; //computer total number of driver steps per output revolution
}

// set stepper goal to a specified angle (-360 < angle < 360), return steps to get to that angle
long Stepper::setAngle(double angle)
{
    Serial.println("\nSetting stepper goal to: " + String(angle) + " degrees");

    // want to travel to absolute angle, not relative to current position
    long motor_goal = (angle / 360L) * steps_per_output_rotation;
    Serial.println("\tSteps to complete: " + String(motor_goal));

    myStepper->moveTo(motor_goal); //set absolute step target
    my_angle = angle;
    return motor_goal;
}

// use hall sensor to zero current motor
void Stepper::zero(int hall_pin, int led_pin, int speed)
{
    myStepper->setSpeed(speed);           //positive rotates gearbox output ccw, negative cw
    while (digitalRead(hall_pin) == HIGH) //hall sensor floats, magnet brings it low
    {
        //run until the sensor triggers
        myStepper->runSpeed();
    }
    //zero position
    myStepper->setCurrentPosition(0);
    my_angle = 0;
    Serial.println("Stepper Zeroed!");
    digitalWrite(led_pin, HIGH);
    delay(200);
    digitalWrite(led_pin, LOW);
}