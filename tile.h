#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
using namespace std;
#pragma once
class Tile {
public:
    Tile();
    Tile(int row, int col);
    Tile& operator=(const Tile& other); //Copy Assignment Operator
    Tile(const Tile& other); //Copy Constructor


    void setMine(bool mine);
    void setRevealed(bool revealed);
    void setFlag(bool flag);
    void setNumAdjacentMines(int num);
    void setMineTexture();
    void setFlagTexture();
    void setHiddenTexture();
    void setRevealedTexture();

    bool isMine() const;
    bool isRevealed() const;
    bool isFlagged() const;


    int getNumAdjacentMines() const;
    sf::Sprite& getSprite();


private:
    bool m_mine = false;
    bool m_revealed = false;
    bool m_flagged = false;
    int m_numAdjacentMines = 0;
    int m_row = 0;
    int m_col = 0;
    sf::RectangleShape m_tileShape;
    sf::Sprite m_sprite;

    sf::Texture m_revealedTexture;
    sf::Texture m_hiddenTexture;
    sf::Texture m_mineTexture;
    sf::Texture m_flagTexture;



};

