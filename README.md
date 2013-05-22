

### Robot Localization Problem
---
In this project we explore the various algorithms for the Robot Local
ization Problem and build a simulator to visualize the results on vari
ous 2D maps. Robot localization is an important problem in robotics.
Simply put, the robot localization problem is as follows. A robot is
placed at an unknown point inside a simple polygon P . The robot
has a map of P and can compute visibility polygon from its current
location. The robot must determine its correct location inside the
polygon P at a minimum cost of travel distance. We implement an
approximation algorithm as given by Apurva Mudgal [2006]. The pa
per gives an O(log3 n) factor approximation algorithm however our
main emphasis is to show the practicality of the algorithm. In this
project we are simulating it on different maps without taking time
complexity in consideration. Computational Geometry Algorithms
Library CGAL has been used for the various computational geometry
algorithms.
