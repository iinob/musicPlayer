#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <conio.h>
#include <windows.h>

// this program is very windows-heavy so it won't work on linux without the sufficient libaries (ncurses) and some tweaking

// MAKE SURE YOU HAVE WAV FILES IN THE FOLDER YOU START THE PROGRAM IN

// g++ main.cpp -lwinmm

/*
    PROGRESS:
    reading files/directories, changing directories, playing .wav, filtering files

    TODO:
    other audio formats, better controls/ux, don't crash when reading a folder with no wavs, save root location (this would be super ez but I don't wanna)
*/

// easier to read
namespace fs = std::filesystem;

// strings to hold colors for ease of use
std::string red = "\033[1;31m";
std::string yellow = "\033[1;33m";
std::string green = "\033[1;32m";
std::string normal = "\033[0m";

std::vector<std::string> files;
std::vector<std::string> wavs;
std::vector<std::string> folders;

// declare root
std::string root = fs::current_path().string();
std::string cd = root; // the cd system is fundamentally broken and should be removed. however, I still want the subfolder feature because that is cool
std::string shortdir = "";

boolean displayWavs = true; // whether the program should use the wavs vector or the files vector

std::vector<std::string>* cvec = &wavs; // pointer to hold whatever the current used vector is

// more convenient way to change the color out of a cout
void color(std::string target) {
    std::cout << target;
}

// read out contents of the current directory with choice of color
void ls(std::string directory, std::string textColor, boolean output) {
    // flush vectors, otherwise you have to work through all files in the previous directory before you can get into this one
    files.clear();
    wavs.clear();
    folders.clear();

    boolean hasFiles = false;
    boolean hasWavs = false;


    color(textColor);
    // voodoo
    for (const auto & entry : fs::directory_iterator(directory)) {
        std::string currentFile = entry.path().filename().string(); // lots of functions
        if (output) { // sometimes I want the function to shut up
            std::cout << currentFile << "\n";
        }
        if (fs::is_directory(entry.status())) {
            folders.push_back(currentFile);
        } else if(currentFile.substr(currentFile.length() - 4) == ".wav"){
            files.push_back(currentFile);
            wavs.push_back(currentFile);
            hasFiles = true;
            hasWavs = true;
        } else {
            files.push_back(currentFile);
            hasFiles = true;
        }
    }
    color(normal);
        std::cout << "\n" << hasFiles << "\n";
        if(!hasFiles) {
            (*cvec).push_back("NO FILES FOUND!");
        } else if(hasFiles && !hasWavs) {
            (*cvec).push_back("NO AUDIO FILES FOUND! PRESS R TO CHANGE SORT MODE!");
        }
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
    std::cout << "current directory = "  << cd << "\n";
    //std::cout << shortdir << "\n";
    ls(cd, yellow, true);
    

    int selectCounter = 0;
    std::string tempdir;
    while(input != 27) {
        if((*cvec).size() <= 0) { // this should never occur
            std::cout << "\nreaching this should be impossible. please tell me if that happens\n";
            system("pause");
            return 1;
        }
        std::cout << "\r" << "current file: " << shortdir << "\\" << ((displayWavs) ? wavs[selectCounter] : files[selectCounter]) << std::flush;
        input = getch();
        // cases are ascii character codes from the getch
        switch(input) {
            case 80: // without checking for the specific case, uppercase P also counts as down arrow because ascii
                if(selectCounter < (*cvec).size() - 1) {
                    selectCounter++;
                } else {
                    selectCounter = 0;
                }
                break;
            case 102:
                std::cout << "\n\nwhat is your desired root directory? ";
                color(red);
                std::cin >> tempdir;
                 if (std::filesystem::exists(tempdir) && std::filesystem::is_directory(tempdir)) { // not cin >> cd, who would possibly do that
                    cd = tempdir;
                    std::cout << "root change successful\n";
                } else {
                    std::cout << "\aroot change failed\n";
                }
                color(normal);
                ls(cd, yellow, true);
                break;
            case 13:
                PlaySound(TEXT((*cvec)[selectCounter].c_str()), NULL, SND_FILENAME | SND_ASYNC); // this will say it's broken in vscode, but it works, only works with wav
                break;
            case 114:
                displayWavs = !displayWavs;
                if(displayWavs) {
                    cvec = &wavs;
                } else {
                    cvec = &files;
                }
                ls(cd, yellow, false);
                break;
            case 100:
                std::cout << "\n";
                ls(cd, green, true);
                break;
        }
        // doesn't work with files more than 66 char long, if your file names are that long you should get help
        // using this in my own audio folder gets caught on the file 'lolgam32momentdeeznutshahafunnymusiconthethingwiththesongthathasthefamuilyguypedergriffinghahasofunnyright.mp3'. this is an edge case and I do not care enough to fix it.
        std::cout << "\r" << std::string(80, ' ') << "\r";
        std::cout << "\r" << "current file: " << shortdir << "\\" << ((displayWavs) ? wavs[selectCounter] : files[selectCounter]) << std::flush;
    }
    return 0;
    // at least it's not java
}