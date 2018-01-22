
**PORTed micromouse**
--
Here is a complete project for a micromouse using the hardware available from UTAD in Portugal. This mouse uses stepper motors and an ATMega32u4 processor. The mouse is fully compatible with the Arduino environment and can be programmed using  that if you wish.

For this project, I have used SLOEBER - the Eclipse-Arduino IDE that runs on Windows, Linux and Mac. You can find out more and get your own copy at:

**[http://eclipse.baeyens.it/](http://eclipse.baeyens.it/)**

The software in this project will not build in the Arduino environment without some serious re-organisation. While that may be annoying, the Arduino environment is not well suited to development of more complex projects. However, you can use the code as you see fit so by all means have a go.

Altogether, there are about 3600 line of code split across 30 or so source files. The project also has CMake files and will build in **[CLion](https://www.jetbrains.com/clion/)** if you install the **[Clion Arduino plugin](https://plugins.jetbrains.com/plugin/9984-arduino)**. CLion is an excellent C/C++ development environment that I would highly recommend.

In its current state, the mouse firmware compiles to about 20k flash and 1200 bytes RAM. It could be reduced by removing a bunch of test and user interface code but there seems little point. Note that, if you build a mouse with the more common ATMega328, it has only 2k RAM compared to the 2.5k available on the ATMega32u4.

**BUILDING**

The repository is the complete Eclipse project folder. A side effect of having the whole project here is that you get to pick up the .cproject, .project and .settings stuff. I may take them out of the git repo since they cause trouble in Sloeber-eclipse. For some reason, sloeber keeps a record of the full system path of the project inside .settings. This makes it impossible to simply import the project onto your own machine because the path will not exist and the build will fail.

To make this code work with sloeber-eclipse on your system, do the following.

1. Clone this repo to some convenient place in your filesystem. It does not matter where.
2. Open Sloeber-Eclipse using your preferred workspace. It does not matter where that is.
2. Choose File | New | Arduino Sketch. Give it some suitable name and alow it to be created in the default location. It could go anywhere else if you prefer. Click Next
3. Select the Platform Folder and choose your board. the code will only build if you have TIMER3 so that means a Leonardo or similar, or you will have to make careful changes. CLick Next.
4. In the template dialogue, select "Default cpp file". Click Finish.
5. You should now see a project in your workspace with just two source files. If your project is called brian, you will see brian.cpp and brian.h. Build the project to make sure all is well and then delete these two files.
6. In your file system manager, Exploreer, Finder or whatever, find the folder where you cloned the repo. In there is a folder called src. Drag this onto the project name in your Sloeber-eclipse workspace. In the dialogue that appears, select the option "Copy files and folders".
7. Now build the project again and you should get a success

*Alternatively - if you want to keep the git links:*

1. Follow steps 1-6 above to get you an empty project. Then right-click the project name and select New | Folder
2. In the dialogue that appears, make sure that ethe project is highlighted and then click the Advanced>> button
3. Choose Link to Alternate location and browse for the src folder in the cloned repo. Click Finish
4. Your project should now have the src folder showing. This is actually a link to the src folder in the repo and so will not appear in the workspace filesystem if you look with your file explorer. It will work just fine and edits in eclipse will change the repo.


_NOTE_ that the hardware in the MMKIT mouse is an ATMEGA32u4. This has more pins and more RAM than the ATMEGA328 in a standard Arduino Uno so porting to something like an Arduino Mini might be tricky.

**FEEDBACK**

You can find out a lot about micromouse robots and their development at my web site **[www.micromouseonline.com](www.micromouseonline.com)**. Have a good look there before posing any questions. Chances are you can find the answer somewhere on the site. Over the years, as I worked out how to do stuff, I wrote it up there as a permanent record to help others.  

Another good resources is at **[www.micromouseusa.com](www.micromouseusa.com)**

Use the contact pages on those sites if you have specific questions.

--

**FEATURES**

At this release, the code is fully functional and the mouse will search the maze and perform two kinds of speed run. One with in-place turns and one with smooth turns.

**WALL FOLLOWING**

There is also an option to run the mouse as a simple wall-follower. In this mode, it will still map the maze.

**CLI**

A rudimentary command-line-interface lets you communicate with the mouse with a serial terminal. If a bluetooth module is attached, that can be done without any physical connection. Be aware, though, that even with the baud rate set fairly high at 57600 baud, sending commands over serial ports takes time and can disrupt the operation of the mouse. Keep it simple.

**Motor Drivers**

The motor drivers used ar DRV8834. These have step and direction pins to control the motors, an Enable pin to discinnect the motors from the power supply and allow for a choice of microstepping modes. It is important that you keep the motors disabled unless they are actually needed to move the mouse because they draw quite a lot of current from the batteries. Choice of microstepping mode is limited by the available drive voltage, the weight of the mouse and the available processing power. If very fine steps are needed, the pulse rate must be higher and the mouse may not be able to service the interrupts fast enough. If the stepping is too coarse, then lack of drive voltage will limit the acceleration and top speed of the mouse. I chose half-steps to give 400 steps per rotation of each wheel.

**LIMITATIONS**

The mouse is intended as a training platform. It is not fast. Instead, it is indeded to demnstrate the operation of a micromouse robot. The techniques in this code will also reach a performance limit if you want to build a fast, DC motor-driven micromouse that can run diagonal paths. For that, you will probably want a bigger processor although it can be done with Arduino processors.

In spite of that, the intent here is to show how the micromouse software is put together and to serve as a basis for further experimentation. I have tried to structure the code so that it is reasonably easy to develop into a much more sophisticated application.

**FURTHER DEVELOPMENT**

Mainly, the challenges faced by a more sophisticated mouse are to be found in:

* **Driving the motors**. Generating the speed profiles and controllers for DC motors is not as hard as you might think but take care to keep rotation and translation profiles separate. The stepper motor drivers here will give smooth acceleration but have no direct control over speed. For DC motors, you will need to be controlling the speed of the motors as a combination of the forward and rotational speeds. Simple PID controllers are adequate for this task. Again, use one controller for forward motion and one for rotation. Combine their outputs to get the desired drive for each wheel.

* **Measuring distances**. Steppers work well in that each step is a fixed distance so you can be reasonably sure about how far the mouse has moved. DC motors need some kind of encoder. Try to get the highest resolution you can. Probably, you need better than the 0.25mm resolution on each wheel of this mouse. The probelm can be solved at that resolution but, as speeds increase, you will find the control more difficult.

* **Path Generation**. PORTed does path generation is two stages. First, a simple string of commands is created. Then, that string is expanded to make it easy to generate motion commands on-the-fly as the mouse moves. For better performance, and to allow for more complex options like all-in-one 180 degree smooth turns and diagonal movement, the same basic path strings can be converted into command lists that can be executed by a function in the code to generate the required speed profiles. 

* **Maze Solving**. The maze solver in PORTed is basic but still very efficient. It can be used exactly as it is to generate perfectly good paths and even for diagonal paths. There will come a time when you want to know that the path chosen is the 'best' rather than just the one that goes through the fewest number of cells. For that, the solver will need to be a little more clever.

*  **Inertial Measurement**. For the best performance at speed, the mouse will want a gyro for feedback of the angular velocity. This is not essential until it becomes clear that you just cannot get good control through turns. You can go surprisingly fast with just the encoder feedback.
*  

