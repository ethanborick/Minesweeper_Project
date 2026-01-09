#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "game_window.h"
using namespace std;

string FormatName(const string& str) {
    string result = str;
    if (!result.empty()) {
        result[0] = toupper(result[0]);
        for (int i = 1; i < result.length(); i++) {
            result[i] = tolower(result[i]);
        }
    }
    return result;
}

void setText(sf::Text &text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}

void WelcomeScreen(int colCount, int rowCount, int mineCount) {
    int height = rowCount * 32 + 100;
    int width = colCount * 32;
    sf::RenderWindow window(sf::VideoMode(width, height), "Minesweeper");

    sf::Font font;
    if (!font.loadFromFile("files/font.ttf")) {
        cout << "Couldn't load \"files/font.ttf\"." << endl;
    }

    sf::Text WelcomeText;
    WelcomeText.setFont(font);
    WelcomeText.setString("WELCOME TO MINESWEEPER!");
    WelcomeText.setCharacterSize(24);
    WelcomeText.setFillColor(sf::Color::White);
    WelcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    setText(WelcomeText, width / 2, height / 2 - 150);

    sf::Text PromptText;
    PromptText.setFont(font);
    PromptText.setString("Enter your name:");
    PromptText.setCharacterSize(20);
    PromptText.setFillColor(sf::Color::White);
    PromptText.setStyle(sf::Text::Bold);
    setText(PromptText, width / 2, height / 2 - 75);

    sf::Text NameText;
    NameText.setFont(font);
    NameText.setString("");
    NameText.setCharacterSize(18);
    NameText.setFillColor(sf::Color::Yellow);
    NameText.setStyle(sf::Text::Bold);

    string PlayerName;

    while (window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8) {
                    if (!PlayerName.empty()) {
                        PlayerName.pop_back();
                    }
                }
                else if (event.text.unicode == 10 && PlayerName.length() != 0) {
                    window.close();
                    GameWindow(colCount, rowCount, mineCount, FormatName(PlayerName));
                }
                else if (PlayerName.length() > 9) {
                    continue;
                }
                else if (isalpha(event.text.unicode)) {
                    char EnteredChar = static_cast<char>(event.text.unicode);
                    PlayerName += EnteredChar;
                }
            }
        }
        string FormattedPlayerName = FormatName(PlayerName);
        NameText.setString(FormattedPlayerName + "|");
        setText(NameText, width / 2, height / 2 - 45);

        window.clear(sf::Color::Blue);
        window.draw(WelcomeText);
        window.draw(PromptText);
        window.draw(NameText);

        window.display();
    }
}


