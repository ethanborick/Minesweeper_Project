#include <iostream>
#include <SFML/Graphics.hpp>
#include "welcome_screen.h"
#include "game_window.h"
using namespace std;


int main() {
    string path = "files/config.cfg";
    ifstream file;
    file.open(path);
    string line;
    vector<int> game_data;
    while (getline(file, line)) {
        game_data.push_back(stoi(line));
    }
    WelcomeScreen(game_data[0], game_data[1], game_data[2]);
    return 0;
}