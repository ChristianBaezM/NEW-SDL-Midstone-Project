#pragma once

// This file is to keep track of the dimentions for the grid

const int GRID_COLS = 14;
const int GRID_ROWS = 13;
const int CELL_SIZE = 48; // our assets are 16 by 16 pixels but we are scaling it up by a factor of 3 or else sly will be too tiny :(

const int SCREEN_WIDTH = GRID_COLS * CELL_SIZE; // 672
const int SCREEN_HEIGHT = GRID_ROWS * CELL_SIZE; // 624
// This is a great screen size if you put the game window and put the https://happyhopper.org/ 
// game window side by side you'll see the windows for this game and our game are almost exact.


