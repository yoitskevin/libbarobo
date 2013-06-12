import math
import threading
import time
import pylab
#from mobot import *
import mobot

def deg2rad(deg):
  return deg * math.pi / 180.0

def rad2deg(deg):
  return deg * 180.0 / math.pi

class Linkbot():
  """
  Each instance of the Linkbot class represents a physical Linkbot. This class
  is used to control and get data from a Linkbot.
  """
  def __init__(self):
    self._mobot = mobot.mobot_t()
    mobot.Mobot_init(self._mobot)

  def connect(self):
    """Connect to a Linkbot
  
    Use this function to control Linkbots that are currently 
    connected in BaroboLink. """
    if mobot.Mobot_connect(self._mobot) != 0:
      raise RuntimeError("Error connecting to robot.")

  def connectWithBluetoothAddress(self, address, channel=1):
    """Connect to a Bluetooth-enabled Linkbot"""
    if mobot.Mobot_connectWithBluetoothAddress(self._mobot, address, channel) != 0:
      raise RuntimeError("Error connecting to robot.")

  def disconnect(self):
    """Disconnect from a Linkbot"""
    mobot.Mobot_disconnect(self._mobot)

  def getJointAngles(self):
    """Return a list of joint angles in degrees"""
    r = mobot.Mobot_getJointAngles(self._mobot)
    if r[0] != 0:
      return -1
    else:
      return map(lambda x: rad2deg(x), r[1:4])

  def getJointAnglesTime(self):
    """Returns a list: [millis, deg1, deg2, deg3]"""
    r = mobot.Mobot_getJointAnglesTime(self._mobot)
    if r[0] != 0:
      return None
    else:
      ret = [r[1]]
      ret.extend( map(lambda x: rad2deg(x), r[2:5]) )
      return ret

  def driveJointTo(self, joint, angle):
    """Drive joint 'joint' to 'angle' (degrees) using PID controller"""
    mobot.Mobot_driveJointTo(self._mobot, joint, deg2rad(angle))

  def driveJointToNB(self, joint, angle):
    """Drive joint 'joint' to 'angle' (degrees) using PID controller"""
    mobot.Mobot_driveJointToNB(self._mobot, joint, deg2rad(angle))

  def moveJoint(self, joint, angle):
    mobot.Mobot_moveJoint(self._mobot, joint, deg2rad(angle))

  def moveJointNB(self, joint, angle):
    mobot.Mobot_moveJointNB(self._mobot, joint, deg2rad(angle))

  def moveJointTo(self, joint, angle):
    mobot.Mobot_moveJointTo(self._mobot, joint, deg2rad(angle))

  def moveJointToNB(self, joint, angle):
    mobot.Mobot_moveJointToNB(self._mobot, joint, deg2rad(angle))

  def move(self, angle1, angle2, angle3):
    """Move the joints on a Linkbot
      
    Move joints from current location by amount specified in arguments in
    degrees. If the joint is not a movable joint (e.g. Joint 2 on a
    Linkbot-I), the argument is ignored."""
    self.moveNB(angle1, angle2, angle3)
    self.moveWait()

  def moveNB(self, angle1, angle2, angle3):
    """Nonblocking version ofthe move() function"""
    mobot.Mobot_moveNB(self._mobot, deg2rad(angle1), deg2rad(angle2), deg2rad(angle3), 0)

  def moveTo(self, angle1, angle2, angle3):
    """Move the joints on a Linkbot

    Move the joints on a Linkbot to absolute positions."""
    self.moveToNB(angle1, angle2, angle3)
    self.moveWait()

  def moveToNB(self, angle1, angle2, angle3):
    """Nonblocking version of the moveTo() function"""
    mobot.Mobot_moveToNB(self._mobot, 
        deg2rad(angle1),
        deg2rad(angle2),
        deg2rad(angle3), 
        0)

  def moveWait(self):
    """Wait until a non-blocking movement function is finished moving"""
    while mobot.Mobot_isMoving(self._mobot):
      time.sleep(0.5)

  def recordAnglesBegin(self, delay=0.05):
    """Begin recording joint angles.

    Keyword arguments:
    delay -- Seconds to delay between recorded datapoints. 0 indicates no delay (as fast as possible)
    """
    self.recordThread = _LinkbotRecordThread(self, delay)
    self.recordThread.start()

  def recordAnglesEnd(self):
    """ End recording angles and return a list consisting of [time_values,
    joint1angles, joint2angles, joint3angles]"""
    self.recordThread.runflag_lock.acquire()
    self.recordThread.runflag = False
    self.recordThread.runflag_lock.release()
    # Wait for recording to end
    while self.recordThread.isRunning:
      time.sleep(0.5)
    return [map(lambda x: x-self.recordThread.time[0], self.recordThread.time), 
        self.recordThread.angles[0], 
        self.recordThread.angles[1], 
        self.recordThread.angles[2]]

  def recordAnglesPlot(self):
    """Plot recorded angles.

    See recordAnglesBegin() and recordAnglesEnd() to record joint motions.
    """
    pylab.plot(
        self.recordThread.time, 
        self.recordThread.angles[0],
        self.recordThread.time, 
        self.recordThread.angles[2])
    pylab.show()

  def reset(self):
    mobot.Mobot_reset(self._mobot)

  def resetToZero(self):
    """Reset the Linkbot to its zero positions."""
    mobot.Mobot_resetToZero(self._mobot)

  def resetToZeroNB(self):
    """Reset the Linkbot to its zero positions."""
    mobot.Mobot_resetToZeroNB(self._mobot)

  def setBuzzerFrequency(self, frequency):
    """Sets the buzzer to buz at a frequency specified in Hz.

    Set frequency to 0 to silence the buzzer"""
    mobot.Mobot_setBuzzerFrequencyOn(self._mobot, int(frequency))

  def setColorRGB(self, r, g, b):
    """Set the multicolor LED on a linkbot.

    Arguments should be in the range [0, 255].
    """
    mobot.Mobot_setColorRGB(self._mobot, int(r), int(g), int(b))

  def setJointSpeed(self, joint, speed):
    """Set the constant-velocity speed of a joint to "speed", in deg/sec."""
    mobot.Mobot_setJointSpeed(self._mobot, int(joint), float(deg2rad(speed)))

  def setMotorPower(self, joint, power):
    """Set the power of a joint.

    Power is a value in the range [0, 255]
    """
    mobot.Mobot_setMotorPower(self._mobot, int(joint), int(power))
   
  def stop(self):
    """Stop all motors."""
    mobot.Mobot_stop(self._mobot) 
    
class _LinkbotRecordThread(threading.Thread):
  def __init__(self, linkbot, delay):
    self.delay = delay
    self.linkbot = linkbot
    self.runflag = False
    self.isRunning = False;
    self.runflag_lock = threading.Lock()
    self.time = []
    self.angles = [ [], [], [] ]
    threading.Thread.__init__(self)

  def run(self):
    self.runflag = True
    self.isRunning = True
    while True:
      self.runflag_lock.acquire()
      if self.runflag == False:
        self.runflag_lock.release()
        break
      self.runflag_lock.release()
      # Get the joint angles and stick them into our data struct
      data = self.linkbot.getJointAnglesTime()
      self.time.append(data[0])
      self.angles[0].append(data[1])
      self.angles[1].append(data[2])
      self.angles[2].append(data[3])
      time.sleep(self.delay)
    self.isRunning = False