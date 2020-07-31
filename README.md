# README #

This is the flight software for the Carleton University CanSat team, primarily for the 2014-2015 competition. Full details about CanSat may be found [here](http://www.cansatcompetition.com/). 

### What is this repository for? ###

The Flight_SW directory contains all the required code to build and deploy the firmware for the CanSat Science Vehicle designed in 2014/2015. The GroundStation_SW directory contains the code to run a ground station GUI. The Flight_HW directory contains sample schematics, layouts, and images of PCBs designed for the competition.

### How do I get set up? ###

This software is written with CodeWarrior for MCUs (Special Edition). It is targeting a custom PCB with the MKL16Z.
In order to program the board in that IDE (not just debug it) you must go under Project->Properties->C/C++ Build->Settings->Librarian and change Model to ewl.

### Where can I find more details about the project? ###
Refer to the Critical Design Review (CDR) document: https://github.com/AlokD123/CanSat-Project_SW/blob/master/Cansat2015_1001_CDR_v01.pdf

For brevity, two concept images are shown below. The top image is the Concept of Operations for the competition. The bottom image is a block diagram of the can-sized satellite's electronics.
