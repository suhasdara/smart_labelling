#include <ros/ros.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <boost/filesystem.hpp>


using namespace std;
using namespace cv;

//Simple point structure
struct point {
    double x;
    double y;
};

vector<string> files;
vector<point> corners;
int currWidth;
int currHeight;
string currFile;

string ogpath;

//Ctrl+C handler
void my_handler(int s){
    ofstream start;
    cout << "\nSIGINT caught. start.txt updated" << endl;
    start.open(ogpath + "src/start.txt");
    start << currFile;
    start.close();
    exit(1);
}

//read all file names in a directory
void read_directory(const string& name)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    string delimiter = ".";
    while ((dp = readdir(dirp)) != NULL) {
      string name = dp->d_name;
      if(name.find(".JPEG") != string::npos) {
        string substr = name.substr(0, name.find(delimiter));
        files.push_back(substr);
      }
    }
    closedir(dirp);
}

//mouse-click event handler
void callback(int event, int x, int y, int flags, void* userdata) {
    //left button down
    if(event == EVENT_LBUTTONDOWN) {
        double newX = x * 1.0 / currWidth;
        double newY = y * 1.0 / currHeight;
        point p = {newX, newY};
        corners.push_back(p);
        cout << "Click detected, corner " << corners.size() << ": " << newX << " " << newY << endl;
    }
    //middle button down
    else if(event == EVENT_MBUTTONDOWN) {
        if(corners.size() > 0) {
            corners.pop_back();
            cout << "Click detected: undo" << endl;
        }
    }
}

//saving a label. Recursive to prevent incorrect labels getting saved
void saveLabel(Mat* img, string& outTxt) {
    namedWindow("ImageDisplay", 1);
    setMouseCallback("ImageDisplay", callback, NULL);
    imshow("ImageDisplay", *img);
    waitKey(0);

    if(corners.size() != 0 && corners.size() != 4) {
        corners.clear();
        cout << "Invalid number of corners, reset corners" << endl;
        saveLabel(img, outTxt);
    }

    ofstream fileOut;
    fileOut.open(outTxt);

    for(int i = 0; i < corners.size(); i++) {
        point p = corners[i];
        fileOut << p.x;
        fileOut << " ";
        fileOut << p.y;
        fileOut << " ";
    }
    fileOut.close();

    corners.clear();
}

//main to loop through all images further from start.txt
int main(int argc, char ** argv) {
    boost::filesystem::path full_path(boost::filesystem::current_path());
    cout << "CWD: " << full_path << endl;
    ogpath = full_path.string() + "/src/label/";

    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = my_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    string path = ogpath + "images/";
    string outPath = ogpath + "labels/";
    read_directory(path);
    sort(files.begin(), files.end());

    ifstream start(ogpath + "src/start.txt");
    string line;
    getline(start, line);

    int index = find(files.begin(), files.end(), line) - files.begin();

    for(int i = index; i < files.size(); i++) {
        currFile = files[i];
        string fileJpg = path + files[i] + ".JPEG";
        string outTxt = outPath + files[i] + ".txt";
        cout << "Image: " << fileJpg << endl;

        Mat img = imread(fileJpg);
        currWidth = img.size().width;
        currHeight = img.size().height;

        saveLabel(&img, outTxt);
    }
}
