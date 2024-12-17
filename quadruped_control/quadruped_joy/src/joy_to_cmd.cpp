#include "joy_to_cmd.h"

// #define BT_A 0
// #define BT_X 3
// #define BT_Y 4
// #define BT_B 1
// #define BT_START 11
// #define BT_LT 8
// #define BT_LB 6


#define BT_A 2
#define BT_X 3
#define BT_Y 0
#define BT_B 1
#define BT_START 9
#define BT_LT 6
#define BT_LB 4
#define LEFT_HOR_AXIS 0
#define LEFT_VER_AXIS 1
#define RIGHT_HOR_AXIS 2
#define RIGHT_VER_AXIS 3

/********************************
 * joy to command
 * empty:
 *      start: stand up
 *      A: walk
 *      B: stop
 * LB: for high level command
 *      start: lay down
 *      A: switch gait 1 walk
 *      B: switch gait 2 pee
 *      X: switch gait 3 probe
 *      Y: switch gait 4
 * LT: for low level command
 *      A: gait cmd 1
 *      B: gait cmd 2 
 *      X: gait cmd 3 
 *      Y: gait cmd 4
********************************/





JoyToCmd::JoyToCmd()
{
    if (!(nh_.param("quadruped_joy/max_vel_x", max_vel_x_, 0.1) &&
          nh_.param("quadruped_joy/max_vel_y", max_vel_y_, 0.1) &&
          nh_.param("quadruped_joy/max_body_roll", max_body_roll_, 0.1) &&
          nh_.param("quadruped_joy/max_body_pitch", max_body_pitch_, 0.1) &&
          nh_.param("default_body_height", default_body_height_, 0.28) &&
          nh_.param("quadruped_joy/max_yaw_rate", max_yaw_rate_, 0.28) &&
          nh_.param("quadruped_joy/body_height_max_delta", body_height_max_delta_, 0.1)))
    {
        ROS_ERROR("joy_to_cmd_node: Fail to load param!");
    }
    reset_quadruped_cmd();
    joy_sub_ = nh_.subscribe("/joy", 3000, &JoyToCmd::joy_msg_callback, this);
    pub_quadruped_cmd_ = nh_.advertise<quadruped_msgs::QuadrupedMove>("/quadruped_cmd", 100);
    first_msg_ = true;
}

void JoyToCmd::reset_quadruped_cmd()
{
    quadruped_cmd_.vel_x = 0;
    quadruped_cmd_.vel_y = 0;
    quadruped_cmd_.yaw_rate = 0;
    quadruped_cmd_.body_height = 0;
    quadruped_cmd_.body_roll = 0;
    quadruped_cmd_.body_pitch = 0;
    quadruped_cmd_.cmd_code = quadruped_cmd_.GAIT_EMPTY;
    quadruped_cmd_.gait_code = quadruped_cmd_.GAIT_EMPTY;
    quadruped_cmd_.is_walking = false;
}

bool JoyToCmd::button_edge(bool press, int button)
{
    return (joy_msg_now_.buttons[button] == press && joy_msg_last_.buttons[button] == !press);
}
void JoyToCmd::joy_msg_callback(const sensor_msgs::Joy &msg)
{
    joy_msg_now_ = msg;
    if (first_msg_)
    {
        first_msg_ = false;
        joy_msg_last_ = msg;
        return;
    }
    quadruped_cmd_.gait_code = quadruped_cmd_.GAIT_EMPTY;
    quadruped_cmd_.cmd_code = quadruped_cmd_.CMD_EMPTY;

    if (msg.buttons[BT_LB]) // LB CMD
    {
        if (msg.buttons[BT_A]) // BT_A
        {
            quadruped_cmd_.cmd_code = quadruped_cmd_.CMD_GAIT_SWITCH_1;
        }
        if (msg.buttons[BT_B]) // BT_B
        {
            quadruped_cmd_.cmd_code = quadruped_cmd_.CMD_GAIT_SWITCH_2;
        }
        if (msg.buttons[BT_X]) // BT_X
        {
            quadruped_cmd_.cmd_code = quadruped_cmd_.CMD_GAIT_SWITCH_3;
        }
        if (msg.buttons[BT_Y]) // BT_Y
        {
            quadruped_cmd_.cmd_code = quadruped_cmd_.CMD_GAIT_SWITCH_4;
        }
        if (msg.buttons[BT_START]) // start
        {
            quadruped_cmd_.cmd_code = quadruped_cmd_.CMD_LAY_DOWN;
            quadruped_cmd_.is_walking = false;
        }
    }
    else if (msg.buttons[BT_LT]) // LT GAIT
    {
        if (msg.buttons[BT_A]) // BT_A
        {
            quadruped_cmd_.gait_code = quadruped_cmd_.GAIT_MOVE_1;
        }
        if (msg.buttons[BT_B]) // BT_B
        {
            quadruped_cmd_.gait_code = quadruped_cmd_.GAIT_MOVE_2;
        }
        if (msg.buttons[BT_X]) // BT_X
        {
            quadruped_cmd_.gait_code = quadruped_cmd_.GAIT_MOVE_3;
        }
        if (msg.buttons[BT_Y]) // BT_Y
        {
            quadruped_cmd_.gait_code = quadruped_cmd_.GAIT_MOVE_4;
        }
        
    }
    else
    {
        if (msg.buttons[BT_START]) // start
        {
            quadruped_cmd_.cmd_code = quadruped_cmd_.CMD_STAND_UP;
            quadruped_cmd_.is_walking = false;
        }
        if (msg.buttons[BT_A])
        {
            quadruped_cmd_.is_walking = true;
        }
        if (msg.buttons[BT_B])
        {
            quadruped_cmd_.is_walking = false;
        }
        if (quadruped_cmd_.is_walking)
        {
            quadruped_cmd_.vel_y = msg.axes[LEFT_HOR_AXIS] * max_vel_y_;
            quadruped_cmd_.vel_x = msg.axes[LEFT_VER_AXIS] * max_vel_x_;
            quadruped_cmd_.yaw_rate = msg.axes[RIGHT_HOR_AXIS] * max_yaw_rate_;
            quadruped_cmd_.body_height = 0;
        }
        else
        {
            quadruped_cmd_.body_roll = -msg.axes[LEFT_HOR_AXIS] * max_body_roll_;
            quadruped_cmd_.body_pitch = msg.axes[LEFT_VER_AXIS] * max_body_pitch_;
            quadruped_cmd_.body_yaw = msg.axes[RIGHT_HOR_AXIS];
            quadruped_cmd_.yaw_rate = 0;
            quadruped_cmd_.body_height = msg.axes[RIGHT_VER_AXIS] * body_height_max_delta_;
        }
    }

    quadruped_cmd_.proportional_cmd = true;
    quadruped_cmd_.proportional_attitude = true;
    quadruped_cmd_.reverve_val1 = (0.5 - msg.axes[5] * 0.5);
    quadruped_cmd_.reverve_val2 = (0.5 - msg.axes[4] * 0.5);
    joy_msg_last_ = msg;
}

void JoyToCmd::publish_quadruped_cmd()
{
    pub_quadruped_cmd_.publish(quadruped_cmd_);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "joy_to_cmd_node");
    std::unique_ptr<JoyToCmd> a1 = std::make_unique<JoyToCmd>();
    ros::Rate rate(1000);
    while (ros::ok())
    {
        a1->publish_quadruped_cmd();
        ros::spinOnce();
        rate.sleep();
    }
    return 0;
}