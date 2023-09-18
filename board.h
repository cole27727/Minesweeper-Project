#include <SFML/Graphics.hpp>
#include "tile.h"
using namespace std;

#pragma once
class Board {
public:
    vector<vector<Tile>> tiles;

    Board(int numRow, int numCol, int numMines); //Param Constructor
    Board& operator=(const Board& other); //Copy Assignment Operator
    ~Board(); //Destructor

    void initBoard(); //Randomly place mines
    void resetBoard() {
        m_numFlags = 0;
        m_numRevealedTiles = 0;
        for (int i = 0; i < m_numRow; i++) {
            for (int j = 0; j < m_numCol; j++) {
                tiles[i][j].setMine(false);
                tiles[i][j].setFlag(false);
                tiles[i][j].setRevealed(false);
            }
        }
        initBoard();
    }

    int getNumRevealed() const {
        return m_numRevealedTiles;
    }
    int getNumFlags();
    int getNumAdjacentMines(int row, int col);
    Tile getTile(int row, int col);
    vector<vector<Tile>> getTiles() const;

    void drawMine(int row, int col, sf::RenderWindow& window) {
        if (tiles[row][col].isMine()) {
            tiles[row][col].setMineTexture();
            sf::Sprite sprite = tiles[row][col].getSprite();
            sprite.setPosition(col * 32, row * 32);
            window.draw(sprite);
        }
    }
    void drawHiddenTiles(int row, int col, sf::RenderWindow& window) {
        if (!tiles[row][col].isRevealed()) {
            tiles[row][col].setHiddenTexture();
            sf::Sprite sprite = tiles[row][col].getSprite();
            sprite.setPosition(col * 32, row * 32);
            window.draw(sprite);
        }
    }
    void drawFlags(sf::RenderWindow& window){
        for (int i = 0; i < m_numRow; i++) {
            for (int j = 0; j < m_numCol; j++) {
                if (tiles[i][j].isFlagged()) {
                    tiles[i][j].setFlagTexture();
                    sf::Sprite sprite2 = tiles[i][j].getSprite();
                    sprite2.setPosition(j * 32, i * 32);
                    window.draw(sprite2);
                }
            }
        }
    }

    void revealTiles(int row, int col) {
        Tile& tile = tiles[row][col];
        if (tile.isMine()) {
            return;
        }
        if (tile.getNumAdjacentMines() == 0) {
            tile.setRevealed(true);
            m_numRevealedTiles += 1;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int neighborRow = row + i;
                    int neighborCol = col + j;
                    if (neighborRow >= 0 && neighborRow < m_numRow && neighborCol >= 0 && neighborCol < m_numCol) {
                        Tile& neighborTile = tiles[neighborRow][neighborCol];
                        if (!neighborTile.isRevealed() && !neighborTile.isMine() && !neighborTile.isFlagged()) {
                            revealTiles(neighborRow, neighborCol);
                        }
                    }
                }
            }
        }
        else {
            tile.setRevealed(true);
            m_numRevealedTiles += 1;
        }
    }
    void setNumAdjacentMines();
    void incrementFlag(int row, int col, bool isFlagged);


private:
    int m_numRow = 0;
    int m_numCol = 0;
    int m_numMines = 0;
    int m_numFlags = 0;
    int m_numRevealedTiles = 0;
    sf::RenderWindow m_window;
};

