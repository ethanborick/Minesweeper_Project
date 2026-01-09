#pragma once
#include <map>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "board_tile_leaderboard.h"
using namespace std;


void Textures(map<string, sf::Texture>& texture_map) {
    sf::Texture mine;
    mine.loadFromFile("files/images/mine.png");
    texture_map["files/images/mine.png"] = mine;

    sf::Texture tile_hidden; // i = 1
    tile_hidden.loadFromFile("files/images/tile_hidden.png");
    texture_map["files/images/tile_hidden.png"] = tile_hidden;

    sf::Texture tile_revealed;
    tile_revealed.loadFromFile("files/images/tile_revealed.png");
    texture_map["files/images/tile_revealed.png"] = tile_revealed;

    sf::Texture number_1;
    number_1.loadFromFile("files/images/number_1.png");
    texture_map["files/images/number_1.png"] = number_1;

    sf::Texture number_2;
    number_2.loadFromFile("files/images/number_2.png");
    texture_map["files/images/number_2.png"] = number_2;

    sf::Texture number_3;
    number_3.loadFromFile("files/images/number_3.png");
    texture_map["files/images/number_3.png"] = number_3;

    sf::Texture number_4;
    number_4.loadFromFile("files/images/number_4.png");
    texture_map["files/images/number_4.png"] = number_4;

    sf::Texture number_5;
    number_5.loadFromFile("files/images/number_5.png");
    texture_map["files/images/number_5.png"] = number_5;

    sf::Texture number_6;
    number_6.loadFromFile("files/images/number_6.png");
    texture_map["files/images/number_6.png"] = number_6;

    sf::Texture number_7;
    number_7.loadFromFile("files/images/number_7.png");
    texture_map["files/images/number_7.png"] = number_7;

    sf::Texture number_8;
    number_8.loadFromFile("files/images/number_8.png");
    texture_map["files/images/number_8.png"] = number_8;

    sf::Texture face_happy;
    face_happy.loadFromFile("files/images/face_happy.png");
    texture_map["files/images/face_happy.png"] = face_happy;

    sf::Texture face_win;
    face_win.loadFromFile("files/images/face_win.png");
    texture_map["files/images/face_win.png"] = face_win;

    sf::Texture face_lose;
    face_lose.loadFromFile("files/images/face_lose.png");
    texture_map["files/images/face_lose.png"] = face_lose;

    sf::Texture debug;
    debug.loadFromFile("files/images/debug.png");
    texture_map["files/images/debug.png"] = debug;

    sf::Texture pause;
    pause.loadFromFile("files/images/pause.png");
    texture_map["files/images/pause.png"] = pause;

    sf::Texture play;
    play.loadFromFile("files/images/play.png");
    texture_map["files/images/play.png"] = play;

    sf::Texture leaderboard;
    leaderboard.loadFromFile("files/images/leaderboard.png");
    texture_map["files/images/leaderboard.png"] = leaderboard;

    sf::Texture flag;
    flag.loadFromFile("files/images/flag.png");
    texture_map["files/images/flag.png"] = flag;

    sf::Texture digits;
    digits.loadFromFile("files/images/digits.png");
    texture_map["files/images/digits.png"] = digits;
}

sf::Sprite DigitSprite(int digit, map<string, sf::Texture>& texture_map) {
    sf::Sprite sprite(texture_map["files/images/digits.png"]);
    sprite.setTextureRect(sf::IntRect(digit * 21, 0, 21, 32));
    return sprite;
}

void DrawButtons(sf::RenderWindow& window, int colCount, int rowCount, map<string, sf::Texture>& texture_map, bool board_paused, Board& board) {
    sf::Sprite _gameInProgress;
    if (board.gameInProgress) {
        _gameInProgress.setTexture(texture_map["files/images/face_happy.png"]);
        _gameInProgress.setPosition((colCount / 2.0f * 32) - 32, (32 * rowCount) + 0.5f);
        window.draw(_gameInProgress);
    }
    else if (board.gameWon) {
        _gameInProgress.setTexture(texture_map["files/images/face_win.png"]);
        _gameInProgress.setPosition((colCount / 2.0f * 32) - 32, (32 * rowCount) + 0.5f);
        window.draw(_gameInProgress);
    }
    else {
        _gameInProgress.setTexture(texture_map["files/images/face_lose.png"]);
        _gameInProgress.setPosition((colCount / 2.0f * 32) - 32, (32 * rowCount) + 0.5f);
        window.draw(_gameInProgress);
    }
    sf::Sprite debug(texture_map["files/images/debug.png"]);
    sf::Sprite pause;
    if(board_paused) {
        pause.setTexture(texture_map["files/images/play.png"]);
    }
    else{
        pause.setTexture(texture_map["files/images/pause.png"]);
    }
    sf::Sprite leaderboard(texture_map["files/images/leaderboard.png"]);
    debug.setPosition((colCount * 32) - 304, (32 * rowCount) + 0.5f);
    pause.setPosition((colCount * 32) - 240, (32 * rowCount) + 0.5f);
    leaderboard.setPosition((colCount * 32) - 176, (32 * rowCount) + 0.5f);
    window.draw(leaderboard);
    window.draw(pause);
    window.draw(debug);
    if (board.minesRemaining >= 0) {
        sf::Sprite hundreds  = DigitSprite(board.minesRemaining / 100, texture_map);
        hundreds.setPosition(33, 32 * (rowCount + 0.5f) + 16);
        sf::Sprite tens = DigitSprite(board.minesRemaining / 10 % 10, texture_map);
        tens.setPosition(54, 32 * (rowCount + 0.5f) + 16);
        sf::Sprite ones = DigitSprite(board.minesRemaining % 10, texture_map);
        ones.setPosition(75, 32 * (rowCount + 0.5f) + 16);
        window.draw(hundreds);
        window.draw(tens);
        window.draw(ones);
    }
    else if (board.minesRemaining < 0) {
        sf::Sprite negative = DigitSprite(10, texture_map);
        negative.setPosition(12, 32 * (rowCount + 0.5f) + 16);
        window.draw(negative);
        sf::Sprite hundreds  = DigitSprite(abs(board.minesRemaining) / 100, texture_map);
        hundreds.setPosition(33, 32 * (rowCount + 0.5f) + 16);
        sf::Sprite tens = DigitSprite(abs(board.minesRemaining) / 10 % 10, texture_map);
        tens.setPosition(54, 32 * (rowCount + 0.5f) + 16);
        sf::Sprite ones = DigitSprite(abs(board.minesRemaining) % 10, texture_map);
        ones.setPosition(75, 32 * (rowCount + 0.5f) + 16);
        window.draw(hundreds);
        window.draw(tens);
        window.draw(ones);
    }
}

void DrawTimer(sf::RenderWindow& window, map<string, sf::Texture>& texture_map, Board& board) {
    sf::Sprite tens_min = DigitSprite(board.timerTotalSeconds / 600, texture_map);
    sf::Sprite ones_min = DigitSprite(board.timerTotalSeconds / 60 % 10, texture_map);
    sf::Sprite tens_sec = DigitSprite(board.timerTotalSeconds % 60 / 10, texture_map);
    sf::Sprite ones_sec = DigitSprite(board.timerTotalSeconds % 10, texture_map);
    tens_min.setPosition((board.colCount * 32) - 97, 32 * (board.rowCount + 0.5) + 16);
    ones_min.setPosition((board.colCount * 32) - 76, 32 * (board.rowCount + 0.5) + 16);
    tens_sec.setPosition((board.colCount * 32) - 54, 32 * (board.rowCount + 0.5) + 16);
    ones_sec.setPosition((board.colCount * 32) - 33, 32 * (board.rowCount + 0.5) + 16);
    window.draw(tens_min);
    window.draw(ones_min);
    window.draw(tens_sec);
    window.draw(ones_sec);
}




