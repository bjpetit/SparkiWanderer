# SparkiWanderer
Basic Sparki program for obstacle avoidance using the ultrasonic sensor

This program moves Sparki around and uses the servo mounted ultrasonic 
range finder to avoid colisions. Here's a basic overview of the program...

- Move forward until;
  - An obstacle is detected 20cm in front of Sparki, or
  - It has been too long since Sparki detected shrinking distance between it and an obstacle
- When either condition is encountered, Sparki will look around and find another path forward by sampling range to an obstacle at different angles and picking the clearest path

Note: the second condition is intended to catch the situations where sparki has gotten stuck on an obstacle which was out of view of the ultrasonic range finder
