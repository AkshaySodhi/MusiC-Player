#include<iostream>
#include<string>
#include<vector>
#include<windows.h>
#include<dirent.h>
#include<color.hpp>
#include<ctime>
using namespace std;

void greeting() {
    time_t t = time(0);
    int hour = localtime(&t)->tm_hour;
    if (hour > 2 && hour < 12) cout << dye::aqua("Good morning\n");
    else if (hour >= 12 && hour <= 16) cout << dye::aqua("Good afternoon\n");
    else if (hour <= 20) cout << dye::aqua("Good evening\n");
    else cout << dye::aqua("Welcome\n");
}

float randomNumber() {
    return ((float)rand() / RAND_MAX);
}

void printTrackList(vector<pair<string, string>>tracks, pair<string, string> currTrack = {}) {
    cout << dye::aqua("\nPlaylist\n");
    for (int i = 0; i < tracks.size(); i++)
    {
        if (tracks[i] == currTrack) cout << dye::green(i + 1) << dye::green(" - ") << dye::green(tracks[i].first) << "\n";
        else cout << i + 1 << " - " << tracks[i].first << "\n";
    }
    cout << "\n";
}

vector<pair<string, string>> generateTrackList(const char* path) {
    int isFile = 32768;
    vector<pair<string, string>>list;

    struct dirent* entry;
    DIR* dir = opendir(path);

    if (!dir) return list;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == isFile)
        {
            string fileName = entry->d_name;
            string fileType = fileName.substr(fileName.find_last_of('.') + 1);
            if (fileType == "wav" || fileType == "WAV" || fileType == "mp3" || fileType == "MP3")
            {
                list.push_back({ fileName, fileType });
            }
        }
    }
    closedir(dir);
    return list;
}

class Player {
    private:
    int i; //index of current track in tracklist
    int n; //size of tracklist
    bool isShuffled;
    bool isPlaying;
    string path;
    vector<pair<string, string>>trackList;
    pair<string, string> currTrack;

    void shuffle() {
        int j = randomNumber() * n;
        while (i == j)
        {
            j = randomNumber() * n;
        }
        i = j;
    }

    void setTrackList() {
        cout << "Enter path to music folder\n\n";
        cin >> path;

        this->trackList = generateTrackList(path.c_str());
        n = trackList.size();
        while (n == 0)
        {
            cout << dye::red("No audio files found!\n\n");
            cout << "Enter path to music folder\n\n";
            cin >> path;
            this->trackList = generateTrackList(path.c_str());
        }
    }

    void playTrack() {
        string openStart = "open";
        string openMid = path + "/" + currTrack.first;
        string openEnd;
        if (currTrack.second == "wav" || currTrack.second == "WAV") openEnd = "type waveaudio alias track";
        if (currTrack.second == "mp3" || currTrack.second == "MP3") openEnd = "type mpegvideo alias track";

        string openCmd = openStart + " \"" + openMid + "\" " + openEnd;
        string playCmd = "play track";

        if (isPlaying) mciSendString(TEXT("close track"), NULL, 0, NULL);
        mciSendString(openCmd.c_str(), NULL, 0, NULL);
        mciSendString(playCmd.c_str(), NULL, 0, NULL);
        isPlaying = true;
        cout << dye::aqua("Now playing-> ") << currTrack.first << "\n\n";
    }

    void setPlayer() {
        this->i = 0;
        this->isShuffled = false;
        this->isPlaying = false;
        setTrackList();
        this->n = trackList.size();
        printTrackList(this->trackList, this->currTrack);
    }

    void playerHandler() {
        string input;
        cout << dye::red("Enter start to play music\n\n");
        cin >> input;

        while (input != "start")
        {
            if (input == "exit") exit(0);
            cout << dye::red("Enter start to play music\n\n");
            cin >> input;
        }
        currTrack = trackList[i];
        playTrack();

        while (input != "exit")
        {
            cin >> input;
            if (input == "pause")
            {
                mciSendString(TEXT("pause track"), NULL, 0, NULL);
                cout << dye::yellow("Paused\n\n");
            }
            else if (input == "resume")
            {
                mciSendString(TEXT("resume track"), NULL, 0, NULL);
                cout << dye::green("Resumed-> ") << currTrack.first << "\n\n";
            }
            else if (input == "stop")
            {
                mciSendString(TEXT("stop track"), NULL, 0, NULL);
                cout << dye::red("Stopped\n\n");
            }
            else if (input == "replay")
            {
                mciSendString(TEXT("play track from 0"), NULL, 0, NULL);
                cout << dye::aqua("Replaying-> ") << currTrack.first << "\n\n";
            }
            else if (input == "next")
            {
                if (isShuffled) shuffle();
                else i == n - 1 ? i = 0 : i++;
                currTrack = trackList[i];
                playTrack();
            }
            else if (input == "prev")
            {
                i == 0 ? i = n - 1 : i--;
                currTrack = trackList[i];
                playTrack();
            }
            else if (input == "shuffle")
            {
                if (!isShuffled) cout << dye::green("Shuffle enabled\n\n");
                else cout << dye::red("Shuffle disabled\n\n");
                isShuffled = !isShuffled;
            }
            else if (input == "playlist")
            {
                cout << "\n";
                printTrackList(trackList, currTrack);
            }
            else if (input == "playtrack")
            {
                int j;
                cout << "Enter track number-> ";
                cin >> j;

                if (j > 0 && j <= trackList.size())
                {
                    i = j - 1;
                    currTrack = trackList[i];
                    playTrack();
                }
                else
                {
                    cout << dye::red("Invalid track no.\n\n");
                }
            }
            else if (input == "changeFolder")
            {
                setPlayer();
                isPlaying = true;
                string input;
                cout << dye::red("Enter start to play music\n\n");
                cin >> input;

                while (input != "start")
                {
                    if (input == "exit") exit(0);
                    cout << dye::red("Enter start to play music\n\n");
                    cin >> input;
                }
                currTrack = trackList[i];
                playTrack();
            }
            else if (input == "help")
            {
                cout << dye::red("\nCommands\n") << "pause -> to pause current track\nresume-> to resume current track\nstop -> to stop current track\nreplay -> to replay current track\nnext -> to play next track\nprev -> to play previous track\nshuffle -> toggle shuffle\nplaylist -> to show current playlist\nplaytrack -> to play a track acc. to track no.\nchangeFolder -> to change music source folder\nexit -> to close the app\n\n";
            }
            else
            {
                cout << dye::red("Invalid input\n\n");
            }
        }
        mciSendString(TEXT("close track"), NULL, 0, NULL);
    }

    public:
    Player() {
        cout << "\033]0;MusiC++\007";
        greeting();
        setPlayer();
        playerHandler();
    }
};

int main() {
    Player app;
}

