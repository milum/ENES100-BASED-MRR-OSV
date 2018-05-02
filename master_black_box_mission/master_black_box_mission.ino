#include <black_box_osv.h>
#include "Enes100.h"

enum phase {
  phase_0, // Exit the landing zone
  phase_1, // Navigate to center of mission area
  phase_2, // Search for Black Box
  phase_3, // Navigate to Black Box
  phase_4, // Transmit Black Box coordinates
  phase_5, // Secure Black Box
  phase_6  // RTB
};

const double landing_zone_x = 1.375, thresh = .5, angle_thresh = .2, obstacle_distance = 23.5;

/* Initialize osv pins
     Parameters:
      int pin_right_dir1,
      int pin_right_dir2,
      int pin_left_dir1,
      int pin_left_dir2,
      int pin_pwm,
      int trigPin,
      int echoPin
      int irPin
*/
BlackBoxOSV osv(2, 4, 6, 7, 3, 10, 11, 13);
Enes100 enes("BASED-MRR", BLACK_BOX, 41, 8, 9);
Coordinate landing_coordinate, black_box_coordinate, mission_center(0,0,0);
phase cur_phase;
int power;
bool landing_stored, arrived;

void setup() {
  Serial.begin(9600);
  osv.init();
  cur_phase = phase_0;
  power = 255;
  landing_stored = false;
  arrived = false;
  
}

void loop() {
  int counter = 0;
  double starting_theta, theta_sum = 0, theta_temp = 0;
  
  //Phases for panic search, default?
 

  // update location
  updateAndPrintLocation();
  
  switch (cur_phase) {

    /* EXIT THE LANDING ZONE
        Preconditions: N/A
        Postconditions: OSV has stored landing coordinate in landing_coordinate and is now at a
        coordinate outside of the landing zone, i.e., the OSV has a coordinate with X value
        greater than the bound defined by landing_zone_x
        Moves to: Phase 1
    */
    case phase_0:
            
      enes.println("PHASE 0");
      
      //store landing coordinate
      while(!landing_stored) {
        if (updateAndPrintLocation()) {
          
          Coordinate coor (enes.location.x, enes.location.y, enes.location.theta);
          landing_coordinate = coor;
          landing_stored = true;
        }
      }
      //print landing coordiante
      enes.print("LANDING COORDINATE STORED: ");
      enes.print("(");
      enes.print(landing_coordinate.x);
      enes.print(", ");
      enes.print(landing_coordinate.y);
      enes.print("), Theta = ");
      enes.println(landing_coordinate.theta);

      //check for obstacle immediately across rocky terrain?

      updateAndPrintLocation();
      //move in correct theta, readjusting as necessary until past landing_zone_x
      while (enes.location.x < landing_zone_x + thresh) {
        orient(3.14);
        //If osv is pointing in reverse direction
        osv.driveP(-power, 200);
        updateAndPrintLocation();
      }

      //orient to theta 0
      orient(0);
      cur_phase = phase_1;
      break;

    /************************************************************************************************/
    /* NAVIGATE TO CENTER OF MISSION AREA
        Preconditions: OSV outside of landing zone
        Postconditions: OSV at center of mission area (X coordinate, Y coordinate)
        Moves to: Phase 2
    */
    case phase_1:
      enes.println("PHASE 1");
      
      
      //While current coordinate not within threahold values compared with center of mission area,
      while (enes.location.x <= mission_center.x - thresh || enes.location.x >= mission_center.x + thresh) {
        
      }
        //If current x coordinate not within threshold value for mission area center
          //turn in correct x direction facing mission area center (+x or -x)
          
          //proceed along this path (maybe adjust as necessary) until within threshold of x value
          //for mission area center OR until obstacle reached (in which case increment obstacle
          //counter)
          
      //If current y coordinate not within threshold value for mission area center
        //turn in correct y direction facing mission area center (+y or -y)
        
        //proceed along this path (maybe adjust as necessary) until within threshold of y value
        //for mission area center OR until obstacle reached (in which case increment obstacle
        //counter)
      
      //WHERE TO RESET COUNTER?
      
      //If obstacle counter == 2  (OSV blocked in two directions)
        //go around


      while (enes.location.x <= 2.7 + thresh) {
        //If osv is not at mission area center
        while (enes.location.theta <= 0 + thresh) {
          enes.updateLocation();
          osv.turnRight(power);
          delay(500);
        }
        while (osv.obstacle(obstacle_distance)) {
          osv.turnRight(power);
          osv.driveP(power, 500);
        }
        osv.driveP(power, 1000);
        enes.updateLocation();
      }

      break;
    /************************************************************************************************/
    /* SEARCH FOR BLACK BOX
        Preconditions: OSV located at A) center of mission area B) with offset of INSERT in X/Y direction
        Postconditions: OSV has either completed X full rotations in search of black box IR signal,
        or has detected the black box IR signal and is facing the source.
        Notes: This phase concerns dedicated searching immediately following Phase 1.
        It does not involve the constant IR sensor checks during previous phases.
        Moves to: Phase 3 if successful, Phase INSERT if unsuccessful
    */
    case phase_2:
            
      enes.println("PHASE 2");

      //record starting theta
      starting_theta = enes.location.theta;
      
      //rotate in set increments, checking IR sensor at each step
      while (counter < 2) {
        if (irSignalCheck()) {
          break; //changing cur_phase to phase_3 covered by irSignalCheck()
        }
        theta_temp = enes.location.theta;
        osv.turnLeft(power);
        delay(100);
        osv.turnOffMotors();
        delay(100);
        updateAndPrintLocation();
        theta_sum += theta_temp;
        enes.print("Total angle rotated thid rotation: ");
        enes.println(theta_sum);
        if (theta_sum >= (3.14 * 2)) {
          counter++;
          theta_sum = 0;
          enes.print(counter);
          enes.println(" full rotation(s) completed.");
        }
        
      }
      
      //if IR signal found, proceed to phase 3
      //else if two full rotations are completed, proceed to phase INSERT
      break;
    /************************************************************************************************/
    /* NAVIGATE TO BLACK BOX
        Preconditions: OSV has Line Of Sight (LOS) with black box, as defined by detection of the
        IR signal by the forward-facing IR sensor
        Postconditions: OSV has LOS with black box and is positioned INSERT meters from the
        black box
        Moves to: Phase 4
    */
    
    case phase_3:
      enes.println("PHASE 3");
      break;
    /************************************************************************************************/
    /* TRANSMIT BLACK BOX COORDINATES
        Preconditions: OSV has LOS with black box and is positioned INSERT meters from the
        black box
        Postconditions: OSV has transmitted coordinates of the black box via RF communication
        Moves to: Phase 5
    */

    
    case phase_4:
      enes.println("PHASE 4");

        //Verify LOS via US and IR sensors

        //Approach black box, maintaining heading and adjusting as necessary until within THRESHOLD range as determined by US sensor
          //if LOS lost, tun slightly in either direction to reacquire

      
      break;
    /************************************************************************************************/
    /* SECURE BLACK BOX
        Preconditions: OSV has LOS with black box, is positioned INSERT meters from the
        black box, and has transmitted coordinates of the black box
        Postconditions: OSV has successfully lifted black box over the sand
        Moves to: Phase 6
    */
      
    case phase_5:
      enes.println("PHASE 5");

      //turn OSV slightly left to accomodate off-center sensor package: WILL NEED TO CALIBRATE VIA TESTING
    
      //acquire bb with servo-powered arm

      //optional: verify bb picked up with sensors
    
      break;
    /************************************************************************************************/
    /* RTB
        Preconditions: OSV has successfully lifted black box over the sand, is holding black box
        Postconditions: OSV has returned to landing coordinates, as defined by the Coordinate
        landing_coordniate
        Moves to: N/A
    */
    case phase_6:
      enes.println("PHASE 6");
      break;
    /************************************************************************************************/
    /*
        Preconditions: N/A
        Postconditions: N/A
    */
    default:
      break;
  }
}

//Updates and prints OSV coordinates
bool updateAndPrintLocation() {
  bool success = false;
  
  if (enes.updateLocation()) {
    enes.print("(");
    enes.print(enes.location.x);
    enes.print(", ");
    enes.print(enes.location.y);
    enes.print("), Theta = ");
    enes.println(enes.location.theta);
    success = true;
  } else {
    enes.println("Location update failed.");
  }
  return success;
};

//Returns true if black box LOS found, sets current phase to 3 (Navigate to black box)
bool irSignalCheck() {
  if (osv.IRsignal()){
    cur_phase = phase_3;
    return true; 
  } else {
    return false;
  }
};

//Orients OSV in angle specified by theta
//-3.14 <= theta <= 3.14
void orient(double theta){
  bool success = false;
  double diff, cur;  
  
  while (!success) {
    if (updateAndPrintLocation()) {
      diff = angle(enes.location.theta, theta);
      
      if (diff < 180 && diff > thresh) {// turn left
        osv.turnLeft(power);
        delay(100);
        osv.turnOffMotors();
        delay(300);
      } else if ((2 * 3.14 - diff) > thresh) {
        osv.turnRight(power);
        delay(100);
        osv.turnOffMotors();
        delay(300);
      } else {
        success = true;
      }
    }
  }
}

// returns double indicating angle from a to b, clockwise (
double angle(double a, double b) {
  double diff;

  diff = fmod(b - a, 2 * 3.14);
  if (diff < 0) {
    diff += (2*3.14);
  }
  return diff;
}
