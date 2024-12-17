
## 相关依赖
请提前安装ubuntu18.04或20.04，及其对应ROS桌面版本。

## 相关代码库安装
### 安装catkin 工具包
```
sudo apt-get install python3-catkin-tools
```

### 安装 osqp
```
git clone --recursive -b v0.6.3  https://github.com/osqp/osqp.git
cd osqp
mkdir build
cd build
cmake -G "Unix Makefiles" ..
cmake --build .
sudo cmake --build . --target install
```
### 安装 osqp-eigen
```
git clone https://github.com/robotology/osqp-eigen.git
cd osqp-eigen
mkdir build
cd build
cmake ..
make 
sudo make install 
```

### 安装ROS joy手柄包
ubuntu 18.04运行：
```
sudo apt-get install ros-melodic-joy
```
ubuntu 20.04运行：
```
sudo apt-get install ros-noetic-joy
```
## 代码编译
新建catkin_ws文件夹，用作ROS工作空间。在其中新建src文件夹，将代码放入其中。

随后，在catkin文件夹路径下，运行：
```
catkin build
```
编译所有代码。

随后，编辑~/.bashrc文件，将
```
source [代码根目录]/catkin_ws/devel/setup.bash
```
输入~/.bashrc的最后一行。
其中，[代码根目录]为代码所在根目录路径。

## 代码运行
打开新的终端，运行
```
roslaunch unitree_gazebo normal.launch
```
启动宇树a1机器人仿真环境。这时应该看到仿真环境和其中趴下的a1机器人。

在另一终端中运行
```
roslaunch quadruped_control gazebodog.launch
```
以启动控制程序。此时机器狗不会运动。

插入手柄，运行
```
rosrun joy joy_node
```
启动手柄节点，以控制机器人。
按下手柄start按键，机器人站起。
按下A，机器人进入trot步态。
按下B，机器人进入站立模式。
推动两个摇杆，可操纵机器人的姿态或运动。
站立模式时按下LB+start，机器人重新卧倒。
注意，不同型号和模式的手柄，对应的按键编码不同。可以使用
```
rostopic echo /joy
```
检查手柄发送的源编码信息。随后更改src/quadruped_control/quadruped_joy/src/joy_to_cmd.cpp中的宏定义：
```
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
```
使其和手柄真实信息相对应。随后重新编译代码，运行节点。

运行
```
roslaunch quadruped_control dog_visualization.launch
```
可以在rviz中可视化显示足式机器人运动状态信息。
