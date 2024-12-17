#!/usr/bin/env python

import rospy
from geometry_msgs.msg import Twist
import sys, select, termios, tty

# Function to get key press
def getKey():
    tty.setraw(sys.stdin.fileno())
    rlist, _, _ = select.select([sys.stdin], [], [], 0.1)
    if rlist:
        key = sys.stdin.read(1)
    else:
        key = ''
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
    return key

# Function to send Twist command
def send_cmd(linear, angular):
    twist = Twist()
    twist.linear.y = 0
    twist.linear.x = linear
    twist.angular.z = angular
    print("---")
    print(twist)
    pub.publish(twist)

if __name__=="__main__":
    settings = termios.tcgetattr(sys.stdin)

    rospy.init_node('keyboard_teleop')
    pub = rospy.Publisher('/cmd_vel', Twist, queue_size=10)

    linear_speed = 0.6  # adjust as needed
    angular_speed = 0.5  # adjust as needed

    try:
        print("Use arrow keys to control the robot. Press 'Ctrl+C' to exit.")
        while True:
            key = getKey()
            if key == 'w':
                send_cmd(linear_speed, 0.0)
            elif key == 's':
                send_cmd(-linear_speed, 0.0)
            elif key == 'a':
                send_cmd(0.0, angular_speed)
            elif key == 'd':
                send_cmd(0.0, -angular_speed)
            elif key == ' ':
                send_cmd(0.0, 0.0)  # Stop
            elif key == '\x03':  # Ctrl+C
                break

    except rospy.ROSInterruptException:
        pass

    finally:
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
