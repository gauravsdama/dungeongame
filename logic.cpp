#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    ifstream file(fileName);
    if (!file.is_open()) {
       cout << "ERROR: Unable to open:" << fileName << endl;
       return nullptr;
    }

   // Read the dimensions of the level (maxRow and maxCol) from the file.
   file >> maxRow >> maxCol;
   file >> player.row >> player.col;

   // Create the 2D map using createMap function.

    char** map = createMap(maxRow, maxCol);
    if (map == nullptr) {
    cout << "Error: Map did not load." << endl;
    return nullptr;
   }
   // Initialize player's position and treasures.
   for (int i = 0; i < maxRow; i++) {
       for (int j = 0; j < maxCol; j++) {
           file >> map[i][j];
           if (i == player.row && j == player.col) {
               map[i][j] = TILE_PLAYER;
           }
       }
   }

   return map;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {


    // Translate the character input into changes in nextRow and nextCol.
    if (input == MOVE_UP) {
        nextRow = nextRow - 1;
    } else if (input == MOVE_DOWN) {
        nextRow = nextRow + 1;
    } else if (input == MOVE_LEFT) {
        nextCol = nextCol - 1;
    } else if (input == MOVE_RIGHT) {
        nextCol = nextCol + 1;
    }
    else{
        return;
    }
}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {
    // Allocate memory for the 2D map.
    if (maxRow <= 0 || maxCol <= 0) {
        return nullptr; 
        //invalid
    }
    char** map = new char*[maxRow];
    for (int i = 0; i < maxRow; i++) {
        map[i] = new char[maxCol];
        for (int j = 0; j < maxCol; j++) {
            map[i][j] = TILE_OPEN;  
        }
    }


    return map;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {

    if (map != nullptr) {
        for (int i = 0; i < maxRow; ++i) {
            delete[] map[i];
        }
        delete[] map;
        map = nullptr;  // Set the pointer to nullptr after deletion to avoid double deallocation.
        maxRow = 0;     // Update maxRow to indicate that the map has been deleted.
    }
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    
    if (map == nullptr || maxRow <= 0 || maxCol <= 0){
        return nullptr;
    }

    int newMaxRow = maxRow * 2;
    int newMaxCol = maxCol * 2;

    // Create a new 2D array with the doubled dimensions.
    char** newMap = new char*[newMaxRow];
    for (int i = 0; i < newMaxRow; i++) {
        newMap[i] = new char[newMaxCol];
    }

    // Copy the contents from the old map to the new map.
    for (int i = 0; i < maxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            newMap[i][j] = map[i][j];
        }
    }

    for (int i = 0; i < maxRow; i++) {
        for (int j = maxCol; j < newMaxCol; j++) {
            newMap[i][j] = (map[i][j - maxCol] == TILE_PLAYER) ? TILE_OPEN : map[i][j - maxCol];
        }
    }
    for (int i = maxRow; i < newMaxRow; i++) {
        for (int j = 0; j < maxCol; j++) {
            newMap[i][j] = (map[i - maxRow][j] == TILE_PLAYER) ? TILE_OPEN : map[i - maxRow][j];
        }
    }

    for (int i = maxRow; i < newMaxRow; i++) {
        for (int j = maxCol; j < newMaxCol; j++) {
            newMap[i][j] = (map[i - maxRow][j - maxCol] == TILE_PLAYER) ? TILE_OPEN : map[i - maxRow][j - maxCol];
        }
    }
    // Deallocate the old map to prevent memory leaks.
    for (int i = 0; i < maxRow; i ++){
        delete [] map[i];
    }
    delete[] map;


    // Update the new dimensions.
    maxRow = newMaxRow;
    maxCol = newMaxCol;
    // Return the pointer to the newly created map.
    return newMap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
        if (nextRow < 0 || nextRow >= maxRow || nextCol < 0 || nextCol >= maxCol) {
        return STATUS_STAY;
    }

    char nextTile = map[nextRow][nextCol];
    
    if (nextTile == TILE_OPEN) {
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_MOVE;
    } else if (nextTile == TILE_TREASURE) {
        player.treasure++;
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_TREASURE;
    } else if (nextTile == TILE_AMULET) {
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_AMULET;
    } else if (nextTile == TILE_DOOR) {
        map[player.row][player.col] = TILE_OPEN;
        player.row = nextRow;
        player.col = nextCol;
        map[nextRow][nextCol] = TILE_PLAYER;
        return STATUS_LEAVE;
    } else if (nextTile == TILE_EXIT) {
        if (player.treasure > 0) {
            map[player.row][player.col] = TILE_OPEN;
            player.row = nextRow;
            player.col = nextCol;
            map[nextRow][nextCol] = TILE_PLAYER;
            return STATUS_ESCAPE;
        } else {
            return STATUS_STAY;
        }
    } else {
        return STATUS_STAY;
    }
    
// Update the map and player's position.
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    bool playerAttacked = false;

    // Iterate through the map to find and update monster locations.
    // is monster up
    for (int i = player.row - 1; i >= 0; i--) {
        if (map[i][player.col] == TILE_PILLAR) {
            break;
        }
        if (map[i][player.col] == TILE_MONSTER) {
            if (map[i+1][player.col] == TILE_PLAYER) {
                map[i][player.col] = TILE_OPEN;
                map[i+1][player.col] = TILE_MONSTER;
                playerAttacked = true;
            }
            map[i][player.col] = TILE_OPEN;
            map[i+1][player.col] = TILE_MONSTER;
        }
    }

    // is monster down
    for (int i = player.row+1; i < maxRow; i++) {
        if (map[i][player.col] == TILE_PILLAR) {
            break;
        }
        if (map[i][player.col] == TILE_MONSTER) {
            if (map[i-1][player.col] == TILE_PLAYER) {
                map[i][player.col] = TILE_OPEN;
                map[i-1][player.col] = TILE_MONSTER;
                playerAttacked = true;
            }
            map[i][player.col] = TILE_OPEN;
            map[i-1][player.col] = TILE_MONSTER;
        }
    }

    // is monster left
    for(int i = player.col-1; i >= 0; i--) {
        if (map[player.row][i] == TILE_PILLAR) {
            break;
        }
        if (map[player.row][i] == TILE_MONSTER) {
            if (map[player.row][i+1] == TILE_PLAYER) {
                map[player.row][i] = TILE_OPEN;
                map[player.row][i+1] = TILE_MONSTER;
                playerAttacked = true;
            }
            map[player.row][i] = TILE_OPEN;
            map[player.row][i+1] = TILE_MONSTER;
        }
    }

    // is monster right
    for (int i = player.col + 1; i < maxCol; i++) {
        if (map[player.row][i] == TILE_PILLAR) {
            break;
        }
        if (map[player.row][i] == TILE_MONSTER) {
            if (map[player.row][i-1] == TILE_PLAYER) {
                map[player.row][i] = TILE_OPEN;
                map[player.row][i-1] = TILE_MONSTER;
                playerAttacked = true;
            }
            map[player.row][i] = TILE_OPEN;
            map[player.row][i-1] = TILE_MONSTER;
        }
    }

    return playerAttacked;
}
