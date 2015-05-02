# PingDar
Arduino Sonar Ping TFT Project

This code is tested and "working". The ultrasonic distance sensor had a wider field of view than desirable.
But it still it still works and is fun to experiment with. 

The initial idea was to incorporate it into a robot with a Arduino Esplora controller. Because of the poor resolution that the ping sensors offer I decided not to continue with the initial plan. It is still fun to play with and I have a few ideas to improve the resolution through software.

Notes:
-Used Arduino Duemilanove
-The Parallax Ping sensor works significantly better than the knockoffs. 
-Used cheap 9G blue micro servos 
-stl file is the 3d printed ping sensor servo mount.

Future plans:
-Software filter to improve resolution.
	If there is a gap between detections increase gap by view angle and distance calculation.
-Do something about rewriting screen.
