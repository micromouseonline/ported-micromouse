/***********************************************************************
 * Created by Peter Harrison on 22/12/2017.
 * Copyright (c) 2017 Peter Harrison
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without l> imitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#include "maze.h"
#include "avr/pgmspace.h"
#include "queue.h"

unsigned char cost[256];
unsigned char walls[256] __attribute__((section(".noinit")));	// the maze walls are preserved after a reset


/***
 * Set a single wall in the maze. Each wall is set from two directions
 * so that it is consistent when seen from the neighbouring cell.
 *
 * The wall is set unconditionally regardless of whether there is
 * already a wall present
 *
 * No check is made on the provided value for direction
 */
void mazeSetWall(unsigned char cell, unsigned char direction) {
  unsigned int nextCell = neighbour(cell, direction);
  switch (direction) {
    case NORTH:
      walls[cell] |= (1 << NORTH);
      walls[nextCell] |= (1 << SOUTH);
      break;
    case EAST:
      walls[cell] |= (1 << EAST);
      walls[nextCell] |= (1 << WEST);
      break;
    case SOUTH:
      walls[cell] |= (1 << SOUTH);
      walls[nextCell] |= (1 << NORTH);
      break;
    case WEST:
      walls[cell] |= (1 << WEST);
      walls[nextCell] |= (1 << EAST);
      break;
    default:; // do nothing -although this is an error
      break;
  }
}

/***
 * Clear a single wall in the maze. Each wall is cleared from two directions
 * so that it is consistent when seen from the neighbouring cell.
 *
 * The wall is cleared unconditionally regardless of whether there is
 * already a wall present
 *
 * No check is made on the provided value for direction
 */
void mazeClearWall(unsigned char cell, unsigned char direction) {
  unsigned int nextCell = neighbour(cell, direction);
  switch (direction) {
    case NORTH:
      walls[cell] &= ~(1 << NORTH);
      walls[nextCell] &= ~(1 << SOUTH);
      break;
    case EAST:
      walls[cell] &= ~(1 << EAST);
      walls[nextCell] &= ~(1 << WEST);
      break;
    case SOUTH:
      walls[cell] &= ~(1 << SOUTH);
      walls[nextCell] &= ~(1 << NORTH);
      break;
    case WEST:
      walls[cell] &= ~(1 << WEST);
      walls[nextCell] &= ~(1 << EAST);
      break;
    default:; // do nothing -although this is an error
      break;
  }
}


/***
 * Initialise a maze and the costs with border walls and the start cell
 *
 * If a test maze is provided, the walls will all be set up from that
 * No attempt is made to verufy the correctness of a test maze.
 *
 */
void mazeInit(const unsigned char *testMaze) {
  for (int i = 0; i < 256; i++) {
    cost[i] = 0;
    walls[i] = 0;
  }
  if (testMaze) {
    mazeCopyWallsFromFlash(testMaze);
    return;
  }
  for (unsigned char i = 0; i < 16; i++) {
    mazeSetWall(i, WEST);
    mazeSetWall(15 * 16 + i, EAST);
    mazeSetWall(i * 16, SOUTH);
    mazeSetWall((16 * i + 15), NORTH);
  }
  mazeSetWall(0, EAST);
  mazeClearWall(0, NORTH);
}

unsigned char cellNorth(unsigned char cell) {
  unsigned char nextCell = (cell + (1));
  return nextCell;
}

unsigned char cellEast(unsigned char cell) {
  unsigned char nextCell = (cell + (16));
  return nextCell;
}

unsigned char cellSouth(unsigned char cell) {
  unsigned char nextCell = (cell + (255));
  return nextCell;
}

unsigned char cellWest(unsigned char cell) {
  unsigned char nextCell = (cell + (240));
  return nextCell;
}

unsigned char neighbour(unsigned char cell, unsigned char direction) {
  unsigned int next;
  switch (direction) {
    case NORTH:
      next = cellNorth(cell);
      break;
    case EAST:
      next = cellEast(cell);
      break;
    case SOUTH:
      next = cellSouth(cell);
      break;
    case WEST:
      next = cellWest(cell);
      break;
    default:
      next = MAX_COST;
  }
  return next;
}


/***
 * Assumes the maze has been flooded
 */
unsigned char neighbourCost(unsigned char cell, unsigned char direction) {
  unsigned char result = MAX_COST;
  unsigned char wallData = walls[cell];
  switch (direction) {
    case NORTH:
      if ((wallData & (1 << NORTH)) == 0) {
        result = cost[cellNorth(cell)];
      }
      break;
    case EAST:
      if ((wallData & (1 << EAST)) == 0) {
        result = cost[cellEast(cell)];
      }
      break;
    case SOUTH:
      if ((wallData & (1 << SOUTH)) == 0) {
        result = cost[cellSouth(cell)];
      }
      break;
    case WEST:
      if ((wallData & (1 << WEST)) == 0) {
        result = cost[cellWest(cell)];
      }
      break;
    default:
      // TODO: this is an error. We should handle it.
      break;
  }
  return result;
}



/***
 * Very simple cell counting flood fills cost array with the
 * manhattan distance from every cell to the target.
 *
 * Although the queue looks complicated, this is a fast flood that
 * examines each accessible cell exactly once. Consequently, it runs
 * in fairly constant time, taking 5.3ms when there are no interrupts.
 *
 * @param target - the cell from which all distances are calculated
 */
void mazeFlood(unsigned char target) {
  for (int i = 0; i < 256; i++) {
    cost[i] = MAX_COST;
  }
  Queue<unsigned char> queue;
  cost[target] = 0;
  queue.add(target);
  while (queue.size() > 0) {
    unsigned char here = queue.head();
    unsigned int newCost = cost[here] + 1;

    for (unsigned char direction = 0; direction < 4; direction++) {
      if (hasExit(here,  direction)) {
        unsigned int nextCell = neighbour(here, direction);
        if (cost[nextCell] > newCost) {
          cost[nextCell] = newCost;
          queue.add(nextCell);
        }
      }
    }
  }
}


/***
 * Algorithm looks around the current cell and records the smallest
 * neighbour and its direction. By starting with the supplied direction,
 * then looking right, then left, the result will preferentially be
 * ahead if there are multiple neighbours with the same cost.
 *
 * @param cell
 * @param startDirection
 * @return
 */
unsigned char directionToSmallest(unsigned char cell, unsigned char startDirection) {
  unsigned char nextDirection = startDirection;
  unsigned char smallestDirection = INVALID_DIRECTION;
  unsigned int nextCost;
  unsigned int smallestCost = cost[cell];
  nextCost = neighbourCost(cell,  nextDirection);
  if (nextCost < smallestCost) {
    smallestCost = nextCost;
    smallestDirection = nextDirection;
  };
  nextDirection = (startDirection + 1) % 4; // right
  nextCost = neighbourCost(cell,  nextDirection);
  if (nextCost < smallestCost) {
    smallestCost = nextCost;
    smallestDirection = nextDirection;
  };
  nextDirection = (startDirection + 3) % 4; // left
  nextCost = neighbourCost(cell,  nextDirection);
  if (nextCost < smallestCost) {
    smallestCost = nextCost;
    smallestDirection = nextDirection;
  };
  nextDirection = (startDirection + 2) % 4; // behind
  nextCost = neighbourCost(cell,  nextDirection);
  if (nextCost < smallestCost) {
    smallestCost = nextCost;
    smallestDirection = nextDirection;
  };
  if (smallestCost == MAX_COST) {
    smallestDirection = 0;
  }
  return smallestDirection;
}



/***
 * Since the sample mazes are in flash memory, we cannnot simply copy
 * them without using the PROGMEM stuff
 */
void mazeCopyWallsFromFlash(const unsigned char *src) {
  memcpy_P(walls, src, 256);
}

// some sample maze data
const PROGMEM unsigned char emptyMaze[] = {
  0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09,
  0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x06, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03
};


const PROGMEM unsigned char japan2007[] = {
  0x0E, 0x08, 0x0A, 0x09, 0x0C, 0x09, 0x0C, 0x09, 0x0C, 0x0A, 0x0A, 0x09, 0x0D, 0x0C, 0x0A, 0x09,
  0x0C, 0x02, 0x09, 0x06, 0x03, 0x06, 0x03, 0x06, 0x01, 0x0C, 0x0A, 0x03, 0x04, 0x02, 0x09, 0x05,
  0x06, 0x08, 0x02, 0x08, 0x0B, 0x0C, 0x0A, 0x0A, 0x03, 0x06, 0x0A, 0x0A, 0x03, 0x0C, 0x03, 0x05,
  0x0C, 0x02, 0x09, 0x06, 0x0A, 0x03, 0x0C, 0x0A, 0x0A, 0x0A, 0x0A, 0x08, 0x0B, 0x06, 0x09, 0x05,
  //
  0x04, 0x0A, 0x00, 0x0B, 0x0E, 0x08, 0x03, 0x0C, 0x0A, 0x09, 0x0E, 0x00, 0x0B, 0x0C, 0x03, 0x05,
  0x04, 0x0A, 0x00, 0x0B, 0x0C, 0x01, 0x0E, 0x02, 0x09, 0x05, 0x0E, 0x00, 0x0B, 0x06, 0x09, 0x05,
  0x04, 0x0A, 0x00, 0x0B, 0x05, 0x04, 0x08, 0x0A, 0x03, 0x05, 0x0C, 0x03, 0x0C, 0x09, 0x05, 0x05,
  0x05, 0x0C, 0x03, 0x0C, 0x03, 0x05, 0x07, 0x0C, 0x09, 0x05, 0x06, 0x0A, 0x03, 0x05, 0x05, 0x05,
  //
  0x04, 0x02, 0x0A, 0x03, 0x0E, 0x00, 0x0B, 0x04, 0x03, 0x06, 0x0A, 0x0A, 0x09, 0x05, 0x05, 0x05,
  0x06, 0x0A, 0x0A, 0x0A, 0x0A, 0x03, 0x0C, 0x00, 0x0B, 0x0C, 0x0A, 0x0A, 0x03, 0x05, 0x05, 0x05,
  0x0C, 0x09, 0x0C, 0x0A, 0x0A, 0x0A, 0x03, 0x04, 0x0B, 0x06, 0x0A, 0x0A, 0x08, 0x03, 0x05, 0x05,
  0x05, 0x05, 0x05, 0x0C, 0x08, 0x0A, 0x0A, 0x02, 0x0B, 0x0C, 0x0A, 0x09, 0x06, 0x09, 0x05, 0x05,
  //
  0x05, 0x05, 0x05, 0x05, 0x06, 0x0A, 0x0A, 0x0A, 0x09, 0x06, 0x09, 0x06, 0x09, 0x06, 0x01, 0x05,
  0x05, 0x05, 0x05, 0x05, 0x0C, 0x09, 0x0C, 0x09, 0x06, 0x09, 0x06, 0x09, 0x06, 0x09, 0x05, 0x05,
  0x05, 0x06, 0x03, 0x06, 0x03, 0x06, 0x03, 0x06, 0x09, 0x06, 0x0A, 0x02, 0x0B, 0x06, 0x01, 0x05,
  0x06, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x02, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x02, 0x03
};

//--------------------------------------------------------------------------
