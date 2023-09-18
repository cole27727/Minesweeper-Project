#include "tile.h"
#include "board.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
using namespace std;

Tile::Tile() {
    m_mine = false;
    m_revealed = false;

    // Hidden texture
    if (!m_hiddenTexture.loadFromFile("files/images/tile_hidden.png")) {
        cout << "Failed to load tile_hidden tile.cpp" << endl;
    }

    // Revealed texture
    if (!m_revealedTexture.loadFromFile("files/images/tile_revealed.png")) {
        cout << "Failed to load tile_revealed in tile.cpp" << endl;
    }

    //Mine texture
    if (!m_mineTexture.loadFromFile("files/images/mine.png")) {
        cout << "Failed to load tile.cpp" << endl;
    }

    //Flag texture
    if (!m_flagTexture.loadFromFile("files/images/flag.png")) {
        cout << "Failed to load tile.cpp" << endl;
    }

    m_sprite.setTexture(m_hiddenTexture);
}

Tile::Tile(int row, int col) {
    m_row = row;
    m_col = col;
    m_mine = false;
    m_numAdjacentMines = 0;

    // Hidden texture
    if (!m_hiddenTexture.loadFromFile("files/images/tile_hidden.png")) {
        cout << "Failed to load tile_hidden tile.cpp" << endl;
    }

    // Revealed texture
    if (!m_revealedTexture.loadFromFile("files/images/tile_revealed.png")) {
        cout << "Failed to load tile_revealed in tile.cpp" << endl;
    }

    //Mine texture
    if (!m_mineTexture.loadFromFile("files/images/mine.png")) {
        cout << "Failed to load tile.cpp" << endl;
    }

    //Flag texture
    if (!m_flagTexture.loadFromFile("files/images/flag.png")) {
        cout << "Failed to load tile.cpp" << endl;
    }

    m_sprite.setTexture(m_hiddenTexture);

}

Tile &Tile::operator=(const Tile &other) {  //Copy Assignment Operator
    if (this == &other) {
        return *this;
    }
    m_mine = other.m_mine;
    m_revealed = other.m_revealed;
    return *this;
}

Tile::Tile(const Tile &other) { //Copy Constructor
    m_mine = other.m_mine;
    m_revealed = other.m_revealed;
}

bool Tile::isMine() const {
    return m_mine;
}

bool Tile::isRevealed() const {
    return m_revealed;
}

bool Tile::isFlagged() const {
    return m_flagged;
}

sf::Sprite& Tile::getSprite() {
    return m_sprite;
}

int Tile::getNumAdjacentMines() const {
    return m_numAdjacentMines;
}

void Tile::setFlagTexture() {
    m_sprite.setTexture(m_flagTexture);
}

void Tile::setHiddenTexture() {
    m_sprite.setTexture(m_hiddenTexture);
}

void Tile::setMineTexture() {
    m_sprite.setTexture(m_mineTexture);
}

void Tile::setNumAdjacentMines(int num) {
    m_numAdjacentMines = num;
}

void Tile::setMine(bool mine) {
    m_mine = mine;
}

void Tile::setRevealed(bool revealed) {
    m_revealed = revealed;
}

void Tile::setFlag(bool flag) {
    m_flagged = flag;
}

void Tile::setRevealedTexture() {
    m_sprite.setTexture(m_revealedTexture);
}








