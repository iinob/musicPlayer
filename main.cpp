#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <conio.h>
#include <windows.h>

// this program is very windows-heavy so it won't work on linux without the sufficient libaries (ncurses) and some tweaking

// easier to read
namespace fs = std::filesystem;

// strings to hold colors for ease of use
std::string red = "\033[1;31m";
std::string yellow = "\033[1;33m";
std::string normal = "\033[0m";

std::vector<std::string> files;
std::vector<std::string> folders;

// declare root (cd is temporary)
std::string root = fs::current_path().string();
std::string cd = "D:\\bulkStorage\\programs\\musicPlayer\\testdir";
std::string shortdir = "";

// more convenient way to change the color out of a cout
void color(std::string target) {
    std::cout << target;
}

// read out contents of the current directory with choice of color
void ls(std::string directory, std::string textColor) {
    // flush vectors, otherwise you have to work through all files in the previous directory before you can get into this one
    files.clear();
    folders.clear();

    color(textColor);
    // voodoo
    for (const auto & entry : fs::directory_iterator(directory)) {
        std::string currentFile = entry.path().filename().string();
        std::cout << currentFile << "\n"; // lots of functions lol
        if (fs::is_directory(entry.status())) {
            folders.push_back(currentFile);
        } else {
            files.push_back(currentFile);
        }
    
    }
    color(normal);
}

int main() {

    if(root != cd) {
        std::size_t lastslash = cd.find_last_of("\\");
        shortdir = (lastslash == std::string::npos) ? cd : cd.substr(lastslash + 1);
    } else {
        shortdir = "";
    }
    int input = 0;

    // sets the console virtual so it'll support ANSI escape codes   I love windows :|
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hConsole, &mode);
    SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    
    // test stuffs

    /*
    std::ofstream outfile ("test.txt");
    outfile.close();
    std::remove("D:\\bulkStorage\\programs\\musicPlayer\\test.txt");
    */
    std::cout << "current directory = "  << cd << "\n";
    std::cout << shortdir << "\n";
    ls(cd, yellow);
    

    int selectCounter = 0;
    while(input != 27) {
        std::cout << "\r" << "current file: " << shortdir << "\\" << files[selectCounter] << std::flush;
        input = getch();
        // cases are ascii character codes from the getch
        switch(input) {
            case 80: // without checking for the specific case, uppercase P also counts as down arrow because ascii
                if(selectCounter < files.size() - 1) {
                    selectCounter++;
                } else {
                    selectCounter = 0;
                }
                break;
            case 102:
                std::cout << "\n\nwhat is your desired root directory? ";
                std::cin >> cd; // very dangerous
                ls(cd, yellow);
        }
        // doesn't work with files more than 66 char long, if your file names are that long you should get help
        // using this in my own audio folder gets caught on the file 'lolgam32momentdeeznutshahafunnymusiconthethingwiththesongthathasthefamuilyguypedergriffinghahasofunnyright.mp3'. this is an edge case and I do not care enough to fix it.
        std::cout << "\r" << std::string(80, ' ') << "\r";
        std::cout << "\r" << "current file: " << shortdir << "\\" << files[selectCounter] << std::flush;
    }
    return 0;
    // at least it's not java
}