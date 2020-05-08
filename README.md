# EE319H-Final-Project-CCS
Smash Bros TM4C edition, written in Code Exploder Studio. Using custom drivers and framework.  
Submission for the final project (lab 10 game design competition) of EE319H.  

Features:  
  • 3 characters: Kirby, Game and Watch, and a newcomer: Dr. Jon Valvano  
  • 6 stages: Final Destination, Battlefield, Smashville, UT Main Mall (original), UT EER (original), UT Gregory Gym (original)  
  • 32K audio SD card streaming  
  • Master-Slave controller interface between TM4Cs   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;‣ Master controller receives player inputs from both controllers and controls game logic and audio  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;‣ Slave controller receives sprite data from Master controller and updates both LCD displays  
  • 1 megabyte flash memory used by slave controller for fast access to raw sprite data  
  • Custom drivers for flash memory and ILI9341 LCD display  
  • Custom PCB - schematic and 3D assembly included in files   
  • [Game Simulator](https://github.com/alljiang/EE319H-Final-Project-Simulation), which was used to develop the 8000+ lines of game logic  

Watch the demo here: https://www.youtube.com/watch?v=PevV5ol1qZk  
  
![alt text](https://github.com/alljiang/EE319H-Final-Project-CCS/blob/master/Schematic/render.png)  
  
