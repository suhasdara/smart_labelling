A ROS package to label images for training datasets. Modify according to needs.

Currently designed to label an image inside images/ for four corners of a door-sign.


Requirements:

--ROS, OpenCV, boost libraries

--ROS workspace (built with catkin build) to clone this project into with name 'label'

--Directories named ./images/ and ./labels/


Run:

--Navigate to workspace

--catkin build

--source devel/setup.bash

--rosrun label label


Perks:

--Set event handlers according to comfort

--Continue from where you stopped labelling


Structure:

--./images/ is where you put your photos

--./labels/ is where the labels are outputted

--./src/start.txt is where to begin iterating over images from
