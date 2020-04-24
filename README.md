# Maze Escape Game

## Author

Alex Engle

## Description

This is a small game where you have to navigate a randomly generated maze, avoiding enemies in your way.

## Building

You will need to have Windows Visual Studio 2019, and a built version of the AftrBurner library. Run the file
"build.bat" to generate the cwin64 folder. Inside there, open Final.sln and build from Visual Studio. After this is built,
there should be a new folder based on the build you ran. Open the irrKlang folder in the project root, then open bin/winx64-visualStudio.
Copy all 3 .dll files into the folder in cwin64 explained earlier (otherwise sounds won't play). Now run the executable, either
from Visual Studio or from the command line. If you take the command line path, you must be in the cwin64 directory (1 level
up from the executable).

## Program Driving

Driving the program is pretty simple. Just press Enter to start the game when prompted. While in the game, use the left-click to
look around, and hold right-click to move forward. Holding right-click also lets you look around. You won't be able to change
your speed, go through walls, or move up and down. Known issue: you can kind of phase through walls a bit, but you can't fully
pass through them.

The blue square is the starting point, and the green square is the ending point. Grey virus enemies will be searching you out. Coming
into close contact with one will make you lose. Reach the green square to win. No matter the outcome, you can press Enter to play again.
Press Escape at any time to close the program, or just close the window directly.

## Configuration

In the aftr.conf file, there are a few variables at the bottom to modify the gameplay. You can do things like change how often
walls are randomly removed during generation, change the maze size, number of enemies, and enemy difficulty. Instructions for
each variable are included in the file.

## Hints

The enemies will never do an about-face unless they reach a complete dead-end, so feel free to trail them until that point.

When an enemy is chasing you, you will hear a faint sound coming from it. Listen for it to know when you need to get away!

The start and end squares are in the same places everytime, and are on opposite corners of each other.

If an enemy is not chasing you, it will randomly take a path when it comes to a fork. If it is chasing you, it takes the path that is quickest to you.

## Resource Credits

MichaelTaylor3D for the virus model

Background music, Bewitching Puzzles, comes from Professor Layton vs Phoenix Wright

Enemy sound effect is the Pac-Man chomp sound
