#include <SFML/Graphics.hpp>
#include "tile.h"
#include "board.h"
#include <iostream>
using namespace std;

vector <vector<Tile>> Board::getTiles() const {
    return tiles;
}

Board::Board(int numRow, int numCol, int numMines) {
    m_numCol = numCol;
    m_numMines = numMines;
    m_numRow = numRow;
    m_numFlags = 0;
    tiles.resize(m_numRow);
        for (int i = 0; i < m_numRow; i++) {
            tiles[i].resize(m_numCol);
            for (int j = 0; j < m_numCol; j++) {
                tiles[i][j] = Tile(i, j);
            }
        }
}

void Board::initBoard() {
    for (int i = 0; i < m_numMines; i++) {
        int row = rand() % m_numRow;
        int col = rand() % m_numCol;
        if (tiles[row][col].isMine()) {
            i--;
        }
        if (!tiles[row][col].isMine() && !tiles[row][col].isRevealed()) {
            tiles[row][col].setMine(true);
        }
    }
    setNumAdjacentMines();
}

Tile Board::getTile(int row, int col) {
    return tiles[row][col];;
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        m_numRow = other.m_numRow;
        m_numCol = other.m_numCol;
        m_numMines = other.m_numMines;
        tiles = other.tiles;
    }
    return *this;
} //Copy assignment operator

Board::~Board() {

} //Destructor

void Board::incrementFlag(int row, int col, bool isFlagged) {
    if (isFlagged) {
        m_numFlags++;
    } else {
        m_numFlags--;
    }
}

int Board::getNumFlags() {
    return m_numFlags;
}

void Board::setNumAdjacentMines() {
    for (int i = 0; i < m_numRow; i++) {
        for (int j = 0; j < m_numCol; j++) {
            int count = 0;
            for (int r = i - 1; r <= i + 1; r++) {
                for (int c = j - 1; c <= j + 1; c++) {
                    if (r >= 0 && r < m_numRow && c >= 0 && c < m_numCol && tiles[r][c].isMine()) {
                        count++;
                    }
                }
            }
            tiles[i][j].setNumAdjacentMines(count);
        }
    }
}

int Board::getNumAdjacentMines(int row, int col) {
    int numAdjacentMines = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int checkRow = row + i;
            int checkCol = col + j;
            if (i == 0 && j == 0) {
                continue;
            }
            if (checkRow >= 0 && checkRow < m_numRow && checkCol >= 0 && checkCol < m_numCol) {
                if (tiles[checkRow][checkCol].isMine()) {
                    numAdjacentMines++;
                }
            }
        }
    }
    return numAdjacentMines;
}
