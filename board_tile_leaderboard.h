#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "sprites.h"
#include <stdlib.h>
#include <fstream>
#include <sstream>
using namespace std;

void settext(sf::Text &text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

struct Tile {
    int row;
    int col;
    int adjacent_mine_count;
    vector<Tile*> adjacent_tiles;
    bool hidden = true;
    bool flagged = false;
    bool mine = false;

    Tile(int r, int c) {
        row = r;
        col = c;
        
    }

    void DrawHiddenTile(sf::RenderWindow& window, map<string, sf::Texture>& texture_map) {
        sf::Sprite sprite(texture_map["files/images/tile_hidden.png"]);
        sprite.setPosition(col * 32, row * 32);
        window.draw(sprite);
    }
    void DrawRevealedTile(sf::RenderWindow& window, map<string, sf::Texture>& texture_map) {
        sf::Sprite sprite(texture_map["files/images/tile_revealed.png"]);
        sprite.setPosition(col * 32, row * 32);
        window.draw(sprite);
    }
    void DrawMine(sf::RenderWindow& window, map<string, sf::Texture>& texture_map) {
        sf::Sprite sprite(texture_map["files/images/mine.png"]);
        sprite.setPosition(col * 32, row * 32);
        window.draw(sprite);
    }
    void DrawFlag(sf::RenderWindow& window, map<string, sf::Texture>& texture_map) {
        sf::Sprite sprite(texture_map["files/images/flag.png"]);
        sprite.setPosition(col * 32, row * 32);
        window.draw(sprite);
    }
    void DrawAdjacentCount(sf::RenderWindow& window, map<string, sf::Texture>& texture_map, int adjacent_mines) {
        sf::Sprite sprite(texture_map["files/images/number_" + to_string(adjacent_mines) + ".png"]);
        sprite.setPosition(col * 32, row * 32);
        window.draw(sprite);
    }
};

struct Board {
    int rowCount;
    int colCount;
    bool paused = false;
    bool debug = false;
    bool leaderboardOpen = false;
    int mineCount;
    int minesRemaining;
    vector<vector<Tile*>> tiles;
    bool gameInProgress = true;
    bool gameWon = false;
    bool timerRunning = true;
    int timerTotalSeconds = 0;
    bool leaderboardUpdated = false;


    Board(int r, int c, int mc) {
        rowCount = r;
        colCount = c;
        mineCount = mc;
        minesRemaining = mc;
        gameInProgress = true;
        gameWon = false;
        leaderboardUpdated = false;
        tiles.resize(rowCount);
        for (int i = 0; i < r; i++) {
            for (int j = 0; j < c; j++) {
                Tile* tile = new Tile(i, j);
                tiles[i].push_back(tile);
            }
        }
        PlaceMines();
        CalculateAdjacentTiles();
    }

    void DrawTiles(sf::RenderWindow& window, map<string, sf::Texture>& texture_map) {
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                if (tiles[i][j]->hidden) {
                    tiles[i][j]->DrawHiddenTile(window, texture_map);
                    if (tiles[i][j]->flagged) {
                        tiles[i][j]->DrawFlag(window, texture_map);
                    }
                }
                else {
                    tiles[i][j]->DrawRevealedTile(window, texture_map);
                    if (tiles[i][j]->mine) {
                        tiles[i][j]->DrawMine(window, texture_map);
                    }
                    else if (tiles[i][j]->adjacent_mine_count != 0) {
                        tiles[i][j]->DrawAdjacentCount(window, texture_map, tiles[i][j]->adjacent_mine_count);
                    }
                }
            }
        }
    }

    void Pause(sf::RenderWindow& window, map<string, sf::Texture>& texture_map) {
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                tiles[i][j]->DrawRevealedTile(window, texture_map);
            }
        }
    }

    bool PauseButtonClicked(sf::Vector2i click, int colCount, int rowCount) {
        if (click.x >= (colCount * 32) - 240 && click.x <= (colCount * 32) - 176 && (32 * rowCount) + 0.5f <= click.y  && click.y <= (32 * rowCount) + 64.5f) {
            return true;
        }
        return false;
    }

    bool HappyButtonClicked(sf::Vector2i click, int colCount, int rowCount) {
        if (click.x >= (colCount / 2.0f * 32) - 32 && click.x <= (colCount / 2.0f * 32) + 32 && (32 * rowCount) + 0.5f <= click.y  && click.y <= (32 * rowCount) + 64.5f) {
            return true;
        }
        return false;
    }

    bool DebugButtonClicked(sf::Vector2i click, int colCount, int rowCount) {
        if (click.x >= colCount * 32 - 304 && click.x <= colCount * 32 - 240 && (32 * rowCount) + 0.5f <= click.y  && click.y <= (32 * rowCount) + 64.5f) {
            return true;
        }
        return false;
    }

    bool LeaderboardButtonClicked(sf::Vector2i click, int colCount, int rowCount) {
        if (click.x >= colCount * 32 - 176 && click.x <= colCount * 32 - 112 && (32 * rowCount) + 0.5f <= click.y  && click.y <= (32 * rowCount) + 64.5f) {
            return true;
        }
        return false;
    }

    void Reset() {
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                delete tiles[i][j];
            }
            tiles[i].clear();
        }
        tiles.clear();
        tiles.resize(rowCount);
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                Tile* tile = new Tile(i, j);
                tiles[i].push_back(tile);
            }
        }
        PlaceMines();
        CalculateAdjacentTiles();
        minesRemaining = mineCount;
        gameInProgress = true;
        gameWon = false;
        leaderboardUpdated = false;
    }

    void PlaceMines() {
        int current_mines = 0;
        while (current_mines < mineCount) {
            int randomRow = rand() % rowCount;
            int randomCol = rand() % colCount;
            if (!tiles[randomRow][randomCol]->mine) {
                tiles[randomRow][randomCol]->mine = true;
                current_mines++;
            }
        }
    }

    void Debug(sf::RenderWindow& window, map<string, sf::Texture>& texture_map) {
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                if (tiles[i][j]->mine) {
                    tiles[i][j]->DrawMine(window, texture_map);
                }
            }
        }
    }

    void CalculateAdjacentTiles() {
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                if (i == 0 && j == 0) {
                    tiles[i][j]->adjacent_tiles.push_back(tiles[0][1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[1][1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[1][0]);
                }
                else if (i == rowCount - 1 && j == colCount - 1) {
                    tiles[i][j]->adjacent_tiles.push_back(tiles[rowCount-1][colCount-2]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[rowCount-2][colCount-2]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[rowCount-2][colCount-1]);
                }
                else if (i == 0 && j == colCount - 1) {
                    tiles[i][j]->adjacent_tiles.push_back(tiles[0][colCount-2]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[1][colCount-2]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[1][colCount-1]);
                }
                else if (i == rowCount - 1 && j == 0) {
                    tiles[i][j]->adjacent_tiles.push_back(tiles[rowCount-2][0]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[rowCount-2][1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[rowCount-1][1]);
                }
                else if (i == 0) {
                    tiles[i][j]->adjacent_tiles.push_back(tiles[0][j-1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[1][j-1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[1][j]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[1][j+1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[0][j+1]);
                }
                else if (i == rowCount - 1) {
                    tiles[i][j]->adjacent_tiles.push_back(tiles[rowCount-1][j-1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[rowCount-2][j-1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[rowCount-2][j]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[rowCount-2][j+1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[rowCount-1][j+1]);
                }
                else if (j == 0) {
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i-1][0]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i-1][1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i][1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i+1][1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i+1][0]);
                }
                else if (j == colCount - 1) {
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i-1][colCount-1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i-1][colCount-2]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i][colCount-2]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i+1][colCount-2]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i+1][colCount-1]);
                }
                else {
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i-1][j-1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i-1][j]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i-1][j+1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i][j-1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i][j+1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i+1][j-1]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i+1][j]);
                    tiles[i][j]->adjacent_tiles.push_back(tiles[i+1][j+1]);
                }
                for (int k = 0; k < tiles[i][j]->adjacent_tiles.size(); k++) {
                    if (tiles[i][j]->adjacent_tiles[k]->mine) {
                        tiles[i][j]->adjacent_mine_count++;
                    }
                }
            }
        }
    }

    void RevealTiles(int row, int col) {
        Tile* tile = tiles[row][col];
        if (!tile->hidden || tile->flagged) {
            return;
        }
        tile->hidden = false;
        if (tile->mine) {
            RevealMines();
            gameInProgress = false;
            return;
        }
        if (tile->adjacent_mine_count == 0) {
            for (int i = 0; i < tile->adjacent_tiles.size(); i++) {
                RevealTiles(tile->adjacent_tiles[i]->row, tile->adjacent_tiles[i]->col);
            }
        }
    }

    void RevealMines() {
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                if (tiles[i][j]->mine) {
                    tiles[i][j]->hidden = false;
                }
            }
        }
    }

    void CheckWinner() {
        int hidden_tiles = 0;
        for (int i = 0; i < rowCount; i++) {
            for (int j = 0; j < colCount; j++) {
                if (tiles[i][j]->hidden) {
                    hidden_tiles++;
                }
            }
        }
        if (hidden_tiles == mineCount) {
            gameInProgress = false;
            gameWon = true;
            minesRemaining = 0;
            for (int i = 0; i < rowCount; i++) {
                for (int j = 0; j < colCount; j++) {
                    if (tiles[i][j]->hidden) {
                        tiles[i][j]->flagged = true;
                    }
                }
            }
        }
    }
};

struct Leaderboard {
    Board* board;
    vector<string> leaders;

    Leaderboard(Board& b) {
        board = &b;
        string path = "files/leaderboard.txt";
        ifstream file;
        file.open(path);
        string line;
        while(getline(file, line)) {
            istringstream stream(line);
            string token;
            while (getline(stream, token, ',')) {
                leaders.push_back(token);
            }
        }
    }

    void Display() {
        int height = board->rowCount * 16 + 50;
        int width = board->colCount * 16;
        sf::RenderWindow window(sf::VideoMode(width, height), "Minesweeper");

        sf::Font font;
        if (!font.loadFromFile("files/font.ttf")) {
            cout << "Couldn't load \"files/font.ttf\"." << endl;
        }

        sf::Text LeaderboardText;
        LeaderboardText.setFont(font);
        LeaderboardText.setString("LEADERBOARD");
        LeaderboardText.setCharacterSize(20);
        LeaderboardText.setFillColor(sf::Color::White);
        LeaderboardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        settext(LeaderboardText, width / 2, height / 2 - 120);


        string lb = "1.\t" + leaders[0] + "\t" + leaders[1] + "\n\n" + "2.\t" + leaders[2] + "\t" + leaders[3] + "\n\n" + "3.\t" + leaders[4] + "\t" + leaders[5] + "\n\n" + "4.\t" + leaders[6] + "\t" + leaders[7] + "\n\n" + "5.\t" + leaders[8] + "\t" + leaders[9];
        sf::Text content;
        content.setFont(font);
        content.setString(lb);
        content.setCharacterSize(18);
        content.setFillColor(sf::Color::White);
        content.setStyle(sf::Text::Bold);
        settext(content, width / 2, height / 2 + 20);

        while (window.isOpen()) {
            sf::Event event;
            while(window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }


            window.clear(sf::Color::Blue);
            window.draw(LeaderboardText);
            window.draw(content);

            window.display();
        }
    }

    void Update(string PlayerName, int TotalSeconds) {
        bool added = false;
        for (int i = 0; i < 10; i+=2) {
            int curr_seconds = 60 * stoi(leaders[i].substr(0, 2)) + stoi(leaders[i].substr(3,2));
            if (TotalSeconds < curr_seconds) {
                int added_time_min = TotalSeconds / 60;
                string added_min = to_string(added_time_min);
                if (added_time_min < 10) {
                    added_min = "0" + to_string(added_time_min);
                }
                int added_time_sec = TotalSeconds % 60;
                string added_sec = to_string(added_time_sec);
                if (added_time_sec < 10) {
                    added_sec = "0" + to_string(added_time_sec);
                }
                string added_time = added_min + ":" + added_sec;
                leaders.insert(leaders.begin() + i, added_time);
                leaders.insert(leaders.begin() + i + 1, PlayerName + "*");
                added = true;
                break;
            }
        }
        if (added) {
            leaders.pop_back();
            leaders.pop_back();
        }
        string path = "files/leaderboard.txt";
        ofstream file;
        file.open(path);
        for (int i = 0; i < 10; i+=2) {
            file << leaders[i] << "," << leaders[i+1] << endl;
        }

    }
};

