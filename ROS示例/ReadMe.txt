
1. 建立编译环境。

2. 打开终端，创建工作空间，执行以下命令
   mkdir my_test
   cd my_test
   source /opt/ros/kinetic/setup.sh
   mkdir -p ./src
   cd src
   catkin_init_workspace
   cd ..
   catkin_make

   这时候多了build和devel文件夹，devel下有几个setup.*sh需要我们来激活。

   source devel/setup.bash      //激活该工作空间的环境变量
   echo $ROS_PACKAGE_PATH       //读取环境变量

3.创建功能包
   cd src

   catkin_create_pkg imu_publish std_msgs roscpp rospy

   将例程imu_publish文件夹中的所有东西复制覆盖到my_test/src/imu_publish 中

   切换到my_test 目录下

   catkin_make

   编译成功即可在my_test/devel/lib/imu_publish/下看到一个imu的执行文件，将例程中的 cfg.json 文件和imu放在一起。


4. 打开新终端进入my_test运行命令source devel/setup.bash ，结束后运行 roscore。

5. 打开新终端 进入my_test执行命令 source devel/setup.bash
   给串口添加权限 sudo chmod 777 /dev/ttyUSB0 ，配置文件为cfg.json 。

   cd devel/lib/imu_publish 

   执行命令 rosrun imu_publish imu    (imu_publish 是package的名字， imu 是可执行文件名)

6. 打开新终端进入my_test执行命令 source devel/setup.bash
   
  第一种方法：执行命令 rostopic echo /Imu_data 可以看见发布在topic的Imu_data.(推荐)
   
  第二种方法：执行命令 rosrun imu_publish receive

7.可在imu同级目录下看到imu.log的日志文件，程序中接受4000包停止可在main.c中修改。
  