#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>
#include <string>
#include <ctype.h>
#include <unordered_map>
#include "board.h"
#include <queue>
#include <chrono>
#include <vector>
#include <algorithm>

using namespace std;

struct Player {
    string name;
    int minutes;
    int seconds;
    int rank;
};

int main() {
    vector<Player> players;
    cout << "SFML version: " << SFML_VERSION_MAJOR << "." << SFML_VERSION_MINOR << "." << SFML_VERSION_PATCH << endl;

    int tileWidth = 32;
    int tileHeight = 32;

    ifstream file("files/leaderboard.txt");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string timeStr, name;
        getline(ss, timeStr, ',');
        getline(ss, name);
        stringstream timeStream(timeStr);
        int minutes, seconds;
        char colon;
        timeStream >> minutes >> colon >> seconds;
        players.push_back({name, minutes, seconds});
    }
    file.close();

    sf::Font font;
    if (!font.loadFromFile("files/font.ttf")) {
        cout << "Could not open font file" << endl;
        return 1;
    } else {
        cout << "Font file opened successfully!" << endl;
    }

    srand(time(NULL));
    int TILE_SIZE = 32;


    ifstream configFile("files/board_config.cfg");
    if (!configFile) {
        cout << "Cannot open Config file." << endl;
        return 1;
    } else {
        cout << "Config file opened successfully!" << endl << endl;
    }

    //Get data from configFile
    vector<int> conFileData;
    for (string line{}; getline(configFile, line);) {
        int var = stoi(line);
        conFileData.push_back(var);
    }

    int numCol = conFileData[0];
    int numRow = conFileData[1];
    int numMines = conFileData[2];
    int winningNum = numCol * numRow - numMines;

    //=========================================Menu Screen=========================================//
    sf::RenderWindow window(sf::VideoMode(numCol * 32, (numRow * 32) + 100), "Minesweeper");
    sf::Text welcomeText("WELCOME TO MINESWEEPER!", font, 24);
    welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    welcomeText.setFillColor(sf::Color::White);
    welcomeText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - 150);
    welcomeText.setOrigin(welcomeText.getLocalBounds().width / 2.0f, welcomeText.getLocalBounds().height / 2.0f);
    sf::Text PromptText("Enter your name:", font, 20);
    PromptText.setStyle(sf::Text::Bold);
    PromptText.setFillColor(sf::Color::White);
    PromptText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - 75);
    PromptText.setOrigin(PromptText.getLocalBounds().width / 2.0f,
                         PromptText.getLocalBounds().height / 2.0f);
    sf::Text userInputText("", font, 18);
    userInputText.setStyle(sf::Text::Bold);
    userInputText.setFillColor(sf::Color::Yellow);
    userInputText.setOrigin(userInputText.getLocalBounds().width / 2.0f, userInputText.getLocalBounds().height / 2.0f);
    userInputText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - 45);
    sf::Text cursor("|", font, 18);
    cursor.setStyle(sf::Text::Bold);
    cursor.setFillColor(sf::Color::Yellow);
    cursor.setOrigin(0.0f, cursor.getLocalBounds().height / 2.0f);
    string userInput;

    //While loop for Menu screen
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                cout << "Menu Window Closed" << endl << endl;
                window.close();
                return 0;
            } else if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8) {
                    if (!userInput.empty()) {
                        userInput.pop_back();
                    }
                } else if (isalpha(event.text.unicode)) {
                    if (userInput.length() == 0) {
                        userInput.push_back(toupper(event.text.unicode));
                    } else if (userInput.length() < 10) {
                        userInput.push_back(tolower(event.text.unicode));
                    }
                    //Centering the userInput text as the user types
                    userInputText.setString(userInput);
                    userInputText.setOrigin(userInputText.getLocalBounds().width / 2.0f,
                                            userInputText.getLocalBounds().height / 2.0f);
                    userInputText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f - 45);
                }
            } else if (event.type == sf::Event::KeyPressed and userInput.length() > 0) {
                if (event.key.code == sf::Keyboard::Return) {
                    cout << "Enter was pressed" << endl;
                    cout << "Username: " << userInput << endl;
                    cout << "Menu Window Closed" << endl << endl;
                    window.close();
                }
            }
        }
        window.clear(sf::Color::Blue);

        userInputText.setString(userInput + cursor.getString());

        window.draw(welcomeText);
        window.draw(PromptText);
        window.draw(userInputText);

        window.display();
    }

    //=========================================Game Window=========================================//
    Board gameBoard(numRow, numCol, numMines);

    gameBoard.initBoard();

    vector<vector<Tile>> tiles = gameBoard.getTiles();

    sf::RenderWindow gameWindow(sf::VideoMode(numCol * 32, (numRow * 32) + 100), "Game Window");

    //Number Textures + Sprites
    //1. Number 1 Texture
    sf::Texture number1Texture;
    if (!number1Texture.loadFromFile("files/images/number_1.png")) {
        cout << "tile_hidden.png did not open." << endl;
        return 1;
    }
    //2. Number 1 Sprite
    vector<sf::Sprite> number1s;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite number1(number1Texture);
            number1.setPosition(j * 32, i * 32);
            number1s.push_back(number1);
        }
    }
    //1. Number 2 Texture
    sf::Texture number2Texture;
    if (!number2Texture.loadFromFile("files/images/number_2.png")) {
        cout << "number_2.png did not open." << endl;
        return 1;
    }
    //2. Number 2 Sprite
    vector<sf::Sprite> number2s;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite number2(number2Texture);
            number2.setPosition(j * 32, i * 32);
            number2s.push_back(number2);
        }
    }
    //3. Number 3 Texture
    sf::Texture number3Texture;
    if (!number3Texture.loadFromFile("files/images/number_3.png")) {
        cout << "number_3.png did not open." << endl;
        return 1;
    }
    //3. Number 3 Sprite
    vector<sf::Sprite> number3s;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite number3(number3Texture);
            number3.setPosition(j * 32, i * 32);
            number3s.push_back(number3);
        }
    }
    //4. Number 4 Texture
    sf::Texture number4Texture;
    if (!number4Texture.loadFromFile("files/images/number_4.png")) {
        cout << "number_4.png did not open." << endl;
        return 1;
    }
    //4. Number 4 Sprite
    vector<sf::Sprite> number4s;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite number4(number4Texture);
            number4.setPosition(j * 32, i * 32);
            number4s.push_back(number4);
        }
    }
    //5. Number 5 Texture
    sf::Texture number5Texture;
    if (!number5Texture.loadFromFile("files/images/number_5.png")) {
        cout << "number_5.png did not open." << endl;
        return 1;
    }
    //5. Number 5 Sprite
    vector<sf::Sprite> number5s;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite number5(number5Texture);
            number5.setPosition(j * 32, i * 32);
            number5s.push_back(number5);
        }
    }

    //6. Number 6 Texture
    sf::Texture number6Texture;
    if (!number6Texture.loadFromFile("files/images/number_6.png")) {
        cout << "number_6.png did not open." << endl;
        return 1;
    }
    //6. Number 6 Sprite
    vector<sf::Sprite> number6s;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite number6(number6Texture);
            number6.setPosition(j * 32, i * 32);
            number6s.push_back(number6);
        }
    }
    //7. Number 7 Texture
    sf::Texture number7Texture;
    if (!number7Texture.loadFromFile("files/images/number_7.png")) {
        cout << "number_7.png did not open." << endl;
        return 1;
    }
    //7. Number 7 Sprite
    vector<sf::Sprite> number7s;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite number7(number7Texture);
            number7.setPosition(j * 32, i * 32);
            number7s.push_back(number7);
        }
    }
    //8. Number 8 Texture
    sf::Texture number8Texture;
    if (!number8Texture.loadFromFile("files/images/number_8.png")) {
        cout << "number_8.png did not open." << endl;
        return 1;
    }
    //8. Number 8 Sprite
    vector<sf::Sprite> number8s;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite number8(number8Texture);
            number8.setPosition(j * 32, i * 32);
            number8s.push_back(number8);
        }
    }



    //1. Hidden Tile Texture
    sf::Texture hiddenTileTexture;
    if (!hiddenTileTexture.loadFromFile("files/images/tile_hidden.png")) {
        cout << "tile_hidden.png did not open." << endl;
        return 1;
    }
    //2. Hidden Tile Sprite
    vector<sf::Sprite> hiddenTiles;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite hiddenTile(hiddenTileTexture);
            hiddenTile.setPosition(j * 32, i * 32);
            hiddenTiles.push_back(hiddenTile);
        }
    }

    //1. Flag Texture
    sf::Texture flagTexture;
    if (!flagTexture.loadFromFile("files/images/flag.png")) {
        cout << "flag.png did not open" << endl;
        return 1;
    }
    //2. Flag Sprite
    vector<sf::Sprite> flags;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite flag(flagTexture);
            flag.setPosition(j * 32, i * 32);
            flags.push_back(flag);
        }
    }

    //1. Revealed Tile Texture
    sf::Texture revealedTileTexture;
    if (!revealedTileTexture.loadFromFile("files/images/tile_revealed.png")) {
        cout << "tile_revealed.png did not open." << endl;
        return 1;
    }
    //2. Revealed Tile Sprite
    vector<sf::Sprite> revealedTiles;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite revealedTile(revealedTileTexture);
            revealedTile.setPosition(j * 32, i * 32);
            revealedTiles.push_back(revealedTile);
        }
    }

    //Mine Texture
    sf::Texture mineTexture;
    if (!mineTexture.loadFromFile("files/images/mine.png")) {
        cout << "mine.png did not open." << endl;
    }
    //2. Mine  Sprite
    vector<sf::Sprite> mines;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            sf::Sprite mine(mineTexture);
            mine.setPosition(j * 32, i * 32);
            mines.push_back(mine);
        }
    }

    //1. Happy Face Texture
    sf::Texture happyFaceTexture;
    if (!happyFaceTexture.loadFromFile("files/images/face_happy.png")) {
        cout << "face_happy.png did not open." << endl;
        return 1;
    }
    //2. Happy face sprite
    sf::Sprite happyFace(happyFaceTexture);
    happyFace.setPosition((((numCol) / 2.0) * 32) - 32, 32 * ((numRow) + 0.5f));

    //1. Lose Face Texture
    sf::Texture loseFaceTexture;
    if (!loseFaceTexture.loadFromFile("files/images/face_lose.png")) {
        cout << "face_lose.png did not open." << endl;
        return 1;
    }
    //3. Win face Texture
    sf::Texture winFaceTexture;
    if (!winFaceTexture.loadFromFile("files/images/face_win.png")) {
        cout << "face_win.png did not open." << endl;
        return 1;
    }

    //1. Debug button Texture
    sf::Texture debugTexture;
    if (!debugTexture.loadFromFile("files/images/debug.png")) {
        cout << "debug.png did not open." << endl;
        return 1;
    }
    //2. Debug sprite
    sf::Sprite debug(debugTexture);
    debug.setPosition(((numCol) * 32) - 304, 32 * ((numRow) + 0.5f));

    //1. Pause Button Texture
    sf::Texture pauseTexture;
    if (!pauseTexture.loadFromFile("files/images/pause.png")) {
        cout << "pause.png did not open." << endl;
        return 1;
    }
    //2. Pause sprite
    sf::Sprite pause(pauseTexture);
    pause.setPosition(((numCol) * 32) - 240, 32 * ((numRow) + 0.5f));

    //1. Play button Texture
    sf::Texture playTexture;
    if (!playTexture.loadFromFile("files/images/play.png")) {
        cout << "play.png did not open" << endl;
        return 1;
    }

    //1. Leaderboard Button Texture
    sf::Texture leaderboardTexture;
    if (!leaderboardTexture.loadFromFile("files/images/leaderboard.png")) {
        cout << "leaderboard.png did not open" << endl;
        return 1;
    }
    //2. Leaderboard sprite
    sf::Sprite leaderboard(leaderboardTexture);
    leaderboard.setPosition(((numCol) * 32) - 176, 32 * ((numRow) + 0.5f));

    sf::Texture digitsTexture;
    if (!digitsTexture.loadFromFile("files/images/digits.png")) {
        cout << "digits.png did not open" << endl;
        return 1;
    }


    //For clock
    sf::Sprite minutesTens(digitsTexture);
    minutesTens.setTextureRect(sf::IntRect(0, 0, 21, 32));
    minutesTens.setPosition(((numCol * 32) - 97), (32 * (numRow + 0.5f)) + 16);

    sf::Sprite minutesOnes(digitsTexture);
    minutesOnes.setTextureRect(sf::IntRect(21, 0, 21, 32));
    minutesOnes.setPosition(((numCol * 32) - 76), (32 * (numRow + 0.5f)) + 16);

    sf::Sprite secondsTens(digitsTexture);
    secondsTens.setTextureRect(sf::IntRect(0, 0, 21, 32));
    secondsTens.setPosition(((numCol * 32) - 54), (32 * (numRow + 0.5f)) + 16);

    sf::Sprite secondsOnes(digitsTexture);
    secondsOnes.setTextureRect(sf::IntRect(21, 0, 21, 32));
    secondsOnes.setPosition(((numCol * 32) - 33), (32 * (numRow + 0.5f)) + 16);

    //For flag counter
    sf::Sprite hundreds(digitsTexture);
    hundreds.setTextureRect(sf::IntRect(0,0,21,32));
    hundreds.setPosition(33, 32 * ((numRow)+0.5f)+16);

    sf::Sprite tens(digitsTexture);
    tens.setTextureRect(sf::IntRect(0,0,21,32));
    tens.setPosition(54, 32 * ((numRow)+0.5f)+16);

    sf::Sprite ones(digitsTexture);
    ones.setTextureRect(sf::IntRect(0,0,21,32));
    ones.setPosition(75, 32 * ((numRow)+0.5f)+16);



    sf::Clock clock;
    sf::Time totalTime = sf::Time::Zero;
    bool isPaused = false;
    bool debugPressed = false;
    int winCount = 0;

    sf::Time winTime = sf::Time::Zero;
    bool hasWon = false;
    bool lose = false;


    //While loop for game window
    while (gameWindow.isOpen()) {
        sf::Event event1;
        while (gameWindow.pollEvent(event1)) {

            if (event1.type == sf::Event::Closed) {
                cout << "Game Window Closed" << endl << endl;
                gameWindow.close();
            }

            if (event1.type == sf::Event::MouseButtonPressed && event1.mouseButton.button == sf::Mouse::Right) {
                for (int i = 0; i < numRow * numCol; i++) {
                    int row = i / numCol;
                    int col = i % numCol;
                    float tileX = col * TILE_SIZE;
                    float tileY = row * TILE_SIZE;
                    if (event1.mouseButton.x >= tileX && event1.mouseButton.x < tileX + TILE_SIZE
                        && event1.mouseButton.y >= tileY && event1.mouseButton.y < tileY + TILE_SIZE) {
                        if (!gameBoard.getTile(row,col).isRevealed()) {
                            bool isFlagged = !tiles[row][col].isFlagged();
                            tiles[row][col].setFlag(isFlagged);
                            gameBoard.incrementFlag(row, col, isFlagged);
                        }
                    }
                }
            }

            else if (event1.type == sf::Event::MouseButtonPressed && event1.mouseButton.button == sf::Mouse::Left) {
                for (int i = 0; i < numRow * numCol; i++) {
                    int row = i / numCol;
                    int col = i % numCol;
                    float tileX = col * TILE_SIZE;
                    float tileY = row * TILE_SIZE;
                    if (event1.mouseButton.x >= tileX && event1.mouseButton.x < tileX + TILE_SIZE
                        && event1.mouseButton.y >= tileY && event1.mouseButton.y < tileY + TILE_SIZE) {
                        if (gameBoard.getTile(row, col).isMine() && !(tiles[row][col].isFlagged())) {
                            happyFace.setTexture(loseFaceTexture);
                            lose = true;
                            isPaused = true;
                        }

                        if (!gameBoard.getTile(row, col).isRevealed() && !(tiles[row][col].isFlagged())) {
                            gameBoard.revealTiles(row,col);
                            winCount += 1;

                            if (!hasWon && gameBoard.getNumRevealed() == winningNum) {
                                hasWon = true;
                                happyFace.setTexture(winFaceTexture);
                                gameWindow.draw(happyFace);
                                gameWindow.display();
                            }

                            if (hasWon) {
                                int totalSeconds = totalTime.asSeconds();
                                int minutes = totalSeconds / 60;
                                int seconds = totalSeconds % 60;
                                int width = numCol * 16;
                                int height = numRow * 16 + 50;

                                sf::RenderWindow leaderboardWindow(sf::VideoMode(numCol * 16, (numRow * 16) + 50), "Leaderboard window");
                                //Leaderboard Window
                                sf::Text leaderboardText("LEADERBOARD", font, 20);
                                leaderboardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
                                leaderboardText.setFillColor(sf::Color::White);
                                leaderboardText.setPosition(width / 2.0f, height / 2.0f - 120);
                                leaderboardText.setOrigin(leaderboardText.getLocalBounds().width / 2.0f, leaderboardText.getLocalBounds().height / 2.0f);

                                players.push_back({userInput, minutes, seconds});
                                sort(players.begin(), players.end(), [](const Player& p1, const Player& p2) {
                                    if (p1.minutes == p2.minutes) {
                                        return p1.seconds < p2.seconds;
                                    } else {
                                        return p1.minutes < p2.minutes;
                                    }
                                });
                                if (players.size() > 5) {
                                    players.pop_back();
                                }
                                ofstream file2("files/leaderboard.txt");
                                if (file2.is_open()) {
                                    for (const auto& player : players) {
                                        cout << "Writing player to file: " << player.name << ", " << player.minutes << ":" << player.seconds << endl;
                                        file2 << setfill('0') << setw(2) << player.minutes << ":"
                                              << setfill('0') << setw(2) << player.seconds << ","
                                              << player.name << endl;
                                        file2.close();
                                    }

                                }
                                else {
                                    cout << "Error opening file!" << endl;
                                }
                                file2.close();
                                string contents;
                                for (int i = 0; i < players.size(); i++) {
                                    players[i].rank = i + 1;
                                }
                                for (const auto& player : players) {
                                    string row = to_string(player.rank) + ".\t";
                                    stringstream ss;
                                    ss << setfill('0') << setw(2) << player.minutes << ":" << setfill('0') << setw(2) << player.seconds;
                                    row += ss.str() + "\t" + player.name + "\n\n";
                                    contents += row;
                                }
                                sf::Text leaderboardContents(contents, font, 18);
                                leaderboardContents.setStyle(sf::Text::Bold);
                                leaderboardContents.setFillColor(sf::Color::White);
                                leaderboardContents.setPosition(width / 2.0f, height / 2.0f + 20);
                                leaderboardContents.setOrigin(leaderboardContents.getLocalBounds().width / 2.0f, leaderboardContents.getLocalBounds().height / 2.0f);
                                //While loop for Leaderboard window
                                while (leaderboardWindow.isOpen()) {
                                    sf::Event event2;
                                    while (leaderboardWindow.pollEvent(event2)) {
                                        if (event2.type == sf::Event::Closed) {
                                            cout << "Leaderboard Window Closed" << endl << endl;
                                            leaderboardWindow.close();


                                        }
                                        //Draw events
                                        leaderboardWindow.clear(sf::Color::Blue);
                                        leaderboardWindow.draw(leaderboardText);
                                        leaderboardWindow.draw(leaderboardContents);
                                        leaderboardWindow.display();
                                    }
                                }
                            }
                        }
                    }
                }

                if (happyFace.getGlobalBounds().contains(event1.mouseButton.x, event1.mouseButton.y)) {
                    totalTime = sf::Time::Zero;
                    totalTime -= clock.getElapsedTime();
                    happyFace.setTexture(happyFaceTexture);
                    cout << "Happy Face Pressed" << endl;
                    gameBoard.resetBoard();
                    pause.setTexture(pauseTexture);
                    isPaused = false;
                    lose = false;
                    winCount = 0;
                }
                if (debug.getGlobalBounds().contains(event1.mouseButton.x, event1.mouseButton.y)) {
                    cout << "Debug Pressed" << endl;
                    debugPressed = !debugPressed;

                }
                if (pause.getGlobalBounds().contains(event1.mouseButton.x, event1.mouseButton.y)) {
                    isPaused = !isPaused;
                    if (isPaused) {
                        pause.setTexture(playTexture);
                        totalTime += clock.getElapsedTime();
                        cout << "Pause Pressed" << endl;
                    } else {
                        pause.setTexture(pauseTexture);
                        totalTime -= clock.getElapsedTime();
                        cout << "Play Pressed" << endl;
                    }
                }
                if (leaderboard.getGlobalBounds().contains(event1.mouseButton.x, event1.mouseButton.y)) {
                    cout << "Leaderboard Pressed" << endl;
                    isPaused = true;
                    pause.setTexture(playTexture);
                    gameWindow.draw(pause);
                    gameWindow.display();
                    int width = numCol * 16;
                    int height = numRow * 16 + 50;
                    sf::RenderWindow leaderboardWindow(sf::VideoMode(numCol * 16, (numRow * 16) + 50), "Leaderboard window");
                    sf::Text leaderboardText("LEADERBOARD", font, 20);
                    leaderboardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
                    leaderboardText.setFillColor(sf::Color::White);
                    leaderboardText.setPosition(width / 2.0f, height / 2.0f - 120);
                    leaderboardText.setOrigin(leaderboardText.getLocalBounds().width / 2.0f, leaderboardText.getLocalBounds().height / 2.0f);
                    string contents;
                    for (int i = 0; i < players.size(); i++) {
                        players[i].rank = i + 1;
                    }
                    for (const auto& player : players) {
                        string row = to_string(player.rank) + ".\t";
                        stringstream ss;
                        ss << setfill('0') << setw(2) << player.minutes << ":" << setfill('0') << setw(2) << player.seconds;
                        row += ss.str() + "\t" + player.name + "\n\n";
                        contents += row;
                    }
                    sf::Text leaderboardContents(contents, font, 18);
                    leaderboardContents.setStyle(sf::Text::Bold);
                    leaderboardContents.setFillColor(sf::Color::White);
                    leaderboardContents.setPosition(width / 2.0f, height / 2.0f + 20);
                    leaderboardContents.setOrigin(leaderboardContents.getLocalBounds().width / 2.0f, leaderboardContents.getLocalBounds().height / 2.0f);
                    //While loop for Leaderboard window
                    while (leaderboardWindow.isOpen()) {
                        sf::Event event2;
                        while (leaderboardWindow.pollEvent(event2)) {
                            if (event2.type == sf::Event::Closed) {
                                cout << "Leaderboard Window Closed" << endl << endl;
                                leaderboardWindow.close();


                            }
                            //Draw events
                            leaderboardWindow.clear(sf::Color::Blue);
                            leaderboardWindow.draw(leaderboardText);
                            leaderboardWindow.draw(leaderboardContents);
                            leaderboardWindow.display();
                        }
                    }
                }
            }
        }
        if (!isPaused)
        {
            totalTime += clock.getElapsedTime();
            clock.restart();
        }

        int minutes = totalTime.asSeconds() / 60;
        int seconds = static_cast<int>(totalTime.asSeconds()) % 60;
        minutesTens.setTextureRect(sf::IntRect(21 * (minutes / 10), 0, 21, 32));
        minutesOnes.setTextureRect(sf::IntRect(21 * (minutes % 10), 0, 21, 32));
        secondsTens.setTextureRect(sf::IntRect(21 * (seconds / 10), 0, 21, 32));
        secondsOnes.setTextureRect(sf::IntRect(21 * (seconds % 10), 0, 21, 32));

        int remainBombs = numMines - gameBoard.getNumFlags();
        hundreds.setTextureRect(sf::IntRect(21* (remainBombs / 100),0,21,32));
        tens.setTextureRect(sf::IntRect(21* ((remainBombs / 10) % 10),0,21,32));
        ones.setTextureRect(sf::IntRect(21* (remainBombs % 10),0,21,32));



        // draw menu bar aspects
        gameWindow.clear(sf::Color::White);
        gameWindow.draw(happyFace);
        gameWindow.draw(leaderboard);
        gameWindow.draw(debug);
        gameWindow.draw(pause);

        for (int i = 0; i < numRow; i++) {
            for (int j = 0; j < numCol; j++) {
                int index = i * numCol + j;
                int numAdjMine = 0;
                if (tiles[i][j].isFlagged()) {
                    gameBoard.drawHiddenTiles(i, j, gameWindow);
                    gameWindow.draw(flags[index]);
                }
                else if (lose) {
                    gameBoard.drawHiddenTiles(i, j, gameWindow);
                    if (tiles[i][j].isFlagged()) {
                        gameBoard.drawMine(i,j, gameWindow);
                        gameWindow.draw(flags[index]);

                    }
                    else {
                        gameBoard.drawMine(i,j, gameWindow);
                    }
                    isPaused = true;
                }
                else if (debugPressed) {
                    if (tiles[i][j].isFlagged()) {
                        continue;
                    }
                    else {
                        gameWindow.draw(hiddenTiles[index]);
                        gameBoard.drawMine(i,j, gameWindow);
                    }
                }
                else if (isPaused) {
                    gameBoard.drawHiddenTiles(i,j,gameWindow);
                }
                else if (gameBoard.getTile(i,j).isRevealed()) {
                    gameWindow.draw(revealedTiles[index]);
                    numAdjMine = gameBoard.getNumAdjacentMines(i,j);
                    if (numAdjMine == 1) {
                        gameWindow.draw(number1s[index]);
                    }
                    else if (numAdjMine == 2) {
                        gameWindow.draw(number2s[index]);
                    }
                    else if (numAdjMine == 3) {
                        gameWindow.draw(number3s[index]);
                    }
                    else if (numAdjMine == 4) {
                        gameWindow.draw(number4s[index]);
                    }
                    else if (numAdjMine == 5) {
                        gameWindow.draw(number5s[index]);
                    }
                    else if (numAdjMine == 6) {
                        gameWindow.draw(number6s[index]);
                    }
                    else if (numAdjMine == 7) {
                        gameWindow.draw(number7s[index]);
                    }
                    else if (numAdjMine == 8) {
                        gameWindow.draw(number8s[index]);
                    }
                }
                else {
                    gameBoard.drawHiddenTiles(i, j, gameWindow);
                }
            }
        }




        //draw timer
        gameWindow.draw(minutesTens);
        gameWindow.draw(minutesOnes);
        gameWindow.draw(secondsTens);
        gameWindow.draw(secondsOnes);

        gameWindow.draw(hundreds);
        gameWindow.draw(tens);
        gameWindow.draw(ones);

        gameWindow.display();
    }

    return 0;
}
