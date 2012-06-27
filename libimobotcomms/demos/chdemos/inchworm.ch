/* File: inchworm.ch 
 * Perform the "inchworm" motion four times */
#include <mobot.h>
CMobot robot;

/* Connect to the paired Mobot */
robot.connect();

/* Set robot motors to speed of 0.50 */
robot.setJointSpeedRatio(MOBOT_JOINT2, 0.50);
robot.setJointSpeedRatio(MOBOT_JOINT3, 0.50);

/* Set the robot to "home" position, where all joint angles are 0 degrees. */
robot.moveToZero();

/* Do the inchworm motion four times */
robot.motionInchwormLeft(4);

