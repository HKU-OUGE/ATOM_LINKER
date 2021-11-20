# ATOM_LINKER 于2021/11/20日从私有库搬迁至公开库
NWPU足基 ATOM_LINKER 唐天扬_硬件组负责
ATOM_LINKER 一种适用于IIC，UART，SPI通讯方式的小端数据包结构，可扩展性强，部署快。目前为v1.0版本,参考工业传感器数据设计框架，今后目标为一种可变结构的数据包快速部署框架。目前公布状态：公开。
目标：
西北工业大学足球机器人基地的核心成果为HAWKING系列足球全向机器人，该机器人目前虽已完成初步的体系搭建但仍有许多不成熟之处。其中一点便是其上下位机通讯的不可靠，不可控与不可拓展性。为了解决这个问题，本项目推出ATOM_LINKER 协议为上下位机与单片机传感器间的可靠通讯做好准备。
目标人群：
目标人群为进行机器人与嵌入式开发的设计人员。软件工作人员。
提供的示例代码为能够在不同平台中编译的项目文件，包含
Measure_Example文件夹：
- Linux_Demo：Linux平台Demo，由ubuntu 18.04系统编译和运行。包括：
MeasureDataDemo文件夹中为Linux环境下获取下位机数据，建立通讯的例程。
- Windows_Demo：Windows平台Demo，由VS2019环境编译运行，包括
MeasureDataDemo文件夹中为windows下获取下位机数据，建立通讯的例程。
- STM32_Demo：STM32平台Demo，由IAR 8在Windows10系统上编译并在STM32F411环境下运行，包括：
MeasureDataDemo文件夹，分为下面几个工程：
STM32 Uart版，由IAR在Windows10系统上编译并在STM32F411环境下运行，演示了如何利用Uart获取下位机数据，建立通讯。 
STM32 IIC版，由IAR在Windows10系统上编译并在STM32F411环境下运行，此版本分为两个子版本，使用DRDY和不使用DRDY版本，演示了如何利用IIC去使用ATOM_LINKER的底层协议与设备通信。 
STM32 SPI版，由IAR在Windows10系统上编译并在STM32F411环境下运行，此版本为使用DRDY的版本（SPI只能使用DRDY进行通信），演示了如何利用SPI去使用ATOM_LINKER的底层协议与设备通信。
SDK文件夹：
ROS文件夹：
- catkin_ws文件夹是ROS和模组通信的例程。ROS版本为Kinetic Kame，环境为 Ubuntu 18.04，64位操作系统 
- 
 ROS例程使用方法： 
1. 打开新终端进入catkin_ws；创建工作空间，将imu_publish中的源码放入对应工作空间中；详细步骤参考ReadMe.txt运行命令catkin_make编译产生可执行文件“Atom”；编译结束后运行roscore。
2. 打开新终端进入catkin_ws；执行命令source devel/setup.bash给串口添加权限sudo chmod 666 /dev/ttyUSB0 (代码写的是ttyUSB0,如果需要更改，可在代码中更改)；执行命令 rosrun Atom_publish Atom(Atom_publish 是package的名字，Atom是可执行文件名)。
3. 打开新终端进入catkin_ws；执行命令source devel/setup.bash；执行命令rostopic echo /Atom_data 可以看见发布在topic的Atom_data。
