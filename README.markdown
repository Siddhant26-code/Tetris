# Tetris Game in C++

## Overview
This is a console-based Tetris game implemented in C++ for Windows. It features classic Tetris gameplay with falling tetrominoes, line clearing, scoring, and level progression. The game uses the Windows API for console handling and provides a simple yet functional user interface.

## Features
- **Tetrominoes**: Includes all 7 standard Tetris pieces (I, O, T, S, Z, J, L).
- **Controls**: Move, rotate, and drop pieces using keyboard inputs.
- **Scoring**: Earn points by clearing lines (100, 300, 500, or 800 points per 1, 2, 3, or 4 lines cleared, scaled by level).
- **Levels**: Difficulty increases every 10 lines cleared, speeding up the fall rate.
- **Next Piece Preview**: Displays the upcoming tetromino.
- **Game Over**: Ends when a new piece cannot spawn without collision.

## Requirements
- **Operating System**: Windows (due to use of Windows.h and conio.h).
- **Compiler**: Any C++ compiler supporting Windows API (e.g., MinGW, MSVC).
- **Libraries**: Standard C++ libraries plus Windows-specific headers.

## Installation
1. **Clone or Download**: Obtain the source code.
2. **Compile**: Use a C++ compiler to build the game.
   - Example with g++ (MinGW):
     ```bash
     g++ tetris.cpp -o tetris.exe
     ```
3. **Run**: Execute the compiled `tetris.exe` file in a Windows Command Prompt.

## Controls
- **Left Arrow or A**: Move piece left
- **Right Arrow or D**: Move piece right
- **Up Arrow or W**: Rotate piece
- **Down Arrow or S**: Soft drop (move piece down faster)
- **Spacebar**: Hard drop (instantly place piece at bottom)
- **ESC**: Pause game; press ESC again to quit

## Gameplay
- The game starts with a blank 10x20 grid.
- Tetrominoes fall from the top, one at a time.
- Use the controls to position and rotate pieces.
- Complete horizontal lines to clear them and earn points.
- The game speeds up as you clear more lines and advance levels.
- Game ends when a new piece cannot spawn.


## Submitted By 
- **Group Name**: DSGO

- **Group members**: 
-   **Jay Patel**  
-   **Nikhil Gupta** 
-   **Pearl Tarawat** 
-   **SIddhant Sareen** 