// Wander around room... stop and find a new path if obstacle detected in front
//   of Sparki. Also, find new path if distance reading doesn't appear to be
//   shrinking as expected.
//
//   Brent Petit, 2016

#include <Sparki.h> // include the sparki library

#define NO_ACCEL   // disables the Accelerometer, frees up 598 Bytes Flash Memory
#define NO_MAG     // disables the Magnetometer, frees up 2500 Bytes Flash Memory

#define RGB_VIOLET  60, 0, 100

int min_distance = 20; // distance at which another path will be chosen
int samples = 0;       // samples for change in proximity
int moving = 0;        // indicator of change in proximity
int prior_cm = 0;      // distance at prior iteration

//
// Figure out the next best path
//   step_degrees tells the servo the granularity of samples
//     must be between 1 and 180
//   Defaults to 45 degrees on bad input
void find_next_path(int step_degrees)
{
  int best_distance = 0;  // Reset the best
  int best_angle = 0;
  int distance;
  long int delay_time = 900;

  // Input must be between 1 and 180
  if (step_degrees < 1 || step_degrees > 180)
  {
    sparki.print("ERROR: step_degrees invalid - defaulting to 45");
    step_degrees = 45;
  }

  delay_time = (delay_time * step_degrees) / 180;

  sparki.print("delay_time: ");
  sparki.println(delay_time);
  sparki.updateLCD(); // put the drawings on the screen

  // start at 90 degrees left and check each step_degrees for
  // the best path
  for (int angle = -90; angle <= 90; angle += step_degrees) {

    sparki.servo(angle); // rotate the servo to next postion
    delay(100);        // Overhead
    delay(delay_time); // wait for servo movement

    // measures the distance with Sparki's eyes
    // and keep this angle if it's the best
    if ((distance = sparki.ping()) > best_distance)
    {
      best_distance = distance;
      best_angle = angle;
    }
    // sparki.print(angle);
    // sparki.print(" ");
    // sparki.updateLCD(); // put the drawings on the screen
  }
  // sparki.println(" ");

  // We found something. Re-center the eyes
  sparki.servo(SERVO_CENTER);

  sparki.print("Selecting ");
  sparki.print(best_angle);
  sparki.println(" degrees");
  sparki.updateLCD(); // put the drawings on the screen

  // Turn sparki left or right
  if (best_angle >= 0)
  {
    sparki.moveRight(best_angle); // rotate right 'angle' degrees
  }
  else
  {
    sparki.moveLeft(abs(best_angle)); // rotate left 'angle' degrees
  }
}

void setup() {
  sparki.servo(SERVO_CENTER); // Center the Servo
  sparki.clearLCD(); // wipe the LCD clear
  sparki.println("Starting Wanderer...");
  sparki.updateLCD(); // put the drawings on the screen
}

void loop()
{
  sparki.RGB(RGB_GREEN); // turn the light green
  sparki.moveForward(); // move Sparki forward
  int cm = sparki.ping(); // measures the distance with Sparki's eyes

  if (cm != -1) // make sure it's a valid response from ping
  {
    if (cm < 20) // if the distance measured is less than 20 centimeters
    {

      sparki.RGB(RGB_RED); // turn the light red
      sparki.beep(); // beep!
      sparki.moveBackward(min_distance / 2); // back up 10 centimeters

      find_next_path(10);

      samples = moving = 0; // we chose another path, reset sampling
    }
    else
    {
      // If the distance appears to not shrink - we may be stuck
      if (prior_cm && (prior_cm > cm))
      {
        moving++;
      }
      prior_cm = cm;
      samples++;


      // once we have more than 30 samples, check that the distance
      // is shrinking more than it is growing. Note, if the room is large
      // this could trigger before Sparki reaches objects.
      if ((samples > 30) && (moving <= (samples / 2)))
      {
        sparki.println("I think I'm stuck! - finding another path");
        sparki.updateLCD(); // put the drawings on the screen
        sparki.RGB(RGB_VIOLET); // turn the light orange
        sparki.beep(); // beep!
        sparki.moveBackward(min_distance / 2); // back up 10 centimeters

        find_next_path(180);

        samples = moving = 0; // we chose another path, reset sampling
      }
      else if (samples > 100)
      {
        samples = moving = 0; // just keeping the count reasonable, reset sampling
      }
      else if (samples % 10 == 0)
      {

        sparki.print("Range: ");
        sparki.print(cm);
        sparki.println(" cm");

        sparki.print((100.0 * moving) / samples);
        sparki.println(" % confidence");
      }

    }
  }
  sparki.updateLCD(); // put the drawings on the screen
  delay(1000); // wait 1 seconds (1000 milliseconds)
}
