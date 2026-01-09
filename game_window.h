#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <stdlib.h>
#include "sprites.h"
#include "board_tile_leaderboard.h"
#include <chrono>
using namespace std;

void GameWindow(int colCount, int rowCount, int mineCount, string PlayerName) {
    int height = rowCount * 32 + 100;
    int width = colCount * 32;
    sf::RenderWindow window(sf::VideoMode(width, height), "Minesweeper");
    Board board(rowCount, colCount, mineCount);
    map<string, sf::Texture> texture_map;
    Textures(texture_map);
    auto start_time = chrono::high_resolution_clock::now();
    while (window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2 click = sf::Mouse::getPosition(window);
                if (board.LeaderboardButtonClicked(click, colCount, rowCount)) {
                    board.leaderboardOpen = !board.leaderboardOpen; // toggle leaderboard
                }
                else if (board.HappyButtonClicked(click, colCount, rowCount)) {
                    board.Reset(); // reset board and replace mines
                    start_time = chrono::high_resolution_clock::now(); // restart timer
                }
                else if (board.gameInProgress) {
                    if (board.PauseButtonClicked(click, colCount, rowCount)) {
                        board.paused = !board.paused; // toggle pause
                    }
                    else if (board.paused) {
                        // if the click wasn't happy face or leaderboard and the game is paused,
                        // ignore the click
                    }
                    else if (board.DebugButtonClicked(click, colCount, rowCount)) {
                        board.debug = !board.debug; // toggle debug
                    }
                    else if (click.y <= rowCount * 32) {
                        int col = click.x / 32;
                        int row = click.y / 32;
                        if (!board.tiles[row][col]->flagged) {
                            board.RevealTiles(row, col);
                            board.CheckWinner();
                        }
                        if (!board.gameInProgress && board.gameWon && !board.leaderboardUpdated) {
                            Leaderboard leaderboard(board); // create the leaderboard
                            leaderboard.Update(PlayerName, board.timerTotalSeconds); // check if they beat anyone
                            board.leaderboardUpdated = true;
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
                sf::Vector2 click = sf::Mouse::getPosition(window);
                if (click.y <= rowCount * 32) { // if click is on the board of tiles
                    int col = click.x / 32;
                    int row = click.y / 32;
                    if (!board.tiles[row][col]->flagged && board.tiles[row][col]->hidden) {
                        board.tiles[row][col]->flagged = true; // if unflagged tile is right-clicked, flag it
                        board.minesRemaining--;
                    }
                    else if (board.tiles[row][col]->flagged && board.tiles[row][col]->hidden) {
                        board.tiles[row][col]->flagged = false; // if flagged tile is right-clicked, unflag it
                        board.minesRemaining++;
                    }
                }
            }
        }

        window.clear(sf::Color::White); // clear window
        // if leaderboard is open
        if (board.leaderboardOpen) {
            // if game is still going
            if (board.gameInProgress) {
                // draw all tiles as revealed
                board.Pause(window, texture_map);
            }
            // if the game is over
            else {
                // draw the tiles in their current state
                board.DrawTiles(window, texture_map);
            }
            // draw buttons and timer while leaderboard is open and display the window
            DrawButtons(window, colCount, rowCount, texture_map, board.paused, board);
            DrawTimer(window, texture_map, board);
            window.display();
            // create a local leaderboard copy and display it
            Leaderboard leaderboard(board);
            leaderboard.Display();
            // close the leaderboard
            board.leaderboardOpen = false;
        }
        // if game is paused
        else if (board.paused) {
            // draw all tiles as revealed
            board.Pause(window, texture_map); // pause game
        }
        // if the game isn't paused and the leaderboard isn't open
        else {
            // draw tiles in their current state
            board.DrawTiles(window, texture_map);
            // if the board is in debug mode
            if (board.debug) {
                // show all mine locations
                board.Debug(window, texture_map);
            }
        }
        // if the game isn't paused, continue counting seconds
        if (board.gameInProgress && !board.paused) {
            auto current_time = chrono::high_resolution_clock::now();
            board.timerTotalSeconds = chrono::duration_cast<chrono::seconds>(current_time - start_time).count();
        }
        // continuously draw buttons and timer
        DrawButtons(window, colCount, rowCount, texture_map, board.paused, board);
        DrawTimer(window, texture_map, board);

        window.display(); // display window

    }
}
