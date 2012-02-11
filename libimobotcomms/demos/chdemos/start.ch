/* Filename: start.ch 
 * Move the robot endplates. */
#include <mobot.h>
CMobot robot;

/* Connect to the paired MoBot */
robot.connect();

/* Set the robot to "home" position, where all joint angles are 0 degrees. */
robot.moveToZero();

/* Rotate each of the faceplates by 360 degrees */
robot.move(360, 0, 0, 360);
