#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

using namespace std;
using namespace cv;

class Logger {
public:
    Logger(const string &logFilePath) : logFilePath(logFilePath) {
        logFile.open(logFilePath, ios::app);
        if (!logFile.is_open()) {
            throw runtime_error("Could not open log file.");
        }
    }

    ~Logger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
    void log(const string &message) {
        if (logFile.is_open()) {
            logFile << message << endl;
        }
    }

private:
    string logFilePath;
    ofstream logFile;
};

class Steganography {
public:
    void embedMessage(const string &message, const string &imagePath, const string &outputPath, Logger &logger) {
        Mat image = imread(imagePath, IMREAD_COLOR);
        if (image.empty()) {
            throw runtime_error("Error loading image!");
        }

        string binaryMessage;
        for (char c : message) {
            binaryMessage += charToBinary(c);
        }
        binaryMessage += "1111111111111110"; // End delimiter

        int index = 0;
        for (int i = 0; i < image.rows && index < binaryMessage.size(); i++) {
            for (int j = 0; j < image.cols && index < binaryMessage.size(); j++) {
                Vec3b &pixel = image.at<Vec3b>(i, j);
                for (int k = 0; k < 3 && index < binaryMessage.size(); k++) {
                    // Replace LSB
                    if (binaryMessage[index] == '1') {
                        pixel[k] |= 1; // Set LSB to 1
                    } else {
                        pixel[k] &= ~1; // Set LSB to 0
                    }
                    index++;
                }
            }
        }

        imwrite(outputPath, image);
        logger.log("Message embedded successfully!");
    }

    string extractMessage(const string &imagePath, Logger &logger) {
        Mat image = imread(imagePath, IMREAD_COLOR);
        if (image.empty()) {
            throw runtime_error("Error loading image!");
        }

        string binaryMessage;
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                Vec3b pixel = image.at<Vec3b>(i, j);
                for (int k = 0; k < 3; k++) {
                    binaryMessage += (pixel[k] & 1) ? '1' : '0'; // Get the LSB
                }
            }
        }

        string extractedMessage;
        for (size_t i = 0; i < binaryMessage.size(); i += 8) {
            string byte = binaryMessage.substr(i, 8);
            if (byte == "11111111") break; // Stop at the delimiter
            char c = static_cast<char>(stoi(byte, nullptr, 2));
            extractedMessage += c;
        }

        logger.log("Message extracted successfully!");
        return extractedMessage;
    }

private:
    string charToBinary(char c) {
        string binary;
        for (int i = 7; i >= 0; --i) {
            binary += (c & (1 << i)) ? '1' : '0';
        }
        return binary;
    }
};

int main(int argc, char **argv) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <input_image> <command_file> <output_image>" << endl;
        return 1;
    }

    string imagePath = argv[1];
    string commandFilePath = argv[2];
    string outputImagePath = argv[3];
    Logger logger("steganography.log");
    Steganography stego;

    try {
        // Read command from file
        ifstream commandFile(commandFilePath);
        if (!commandFile.is_open()) {
            throw runtime_error("Error opening command file!");
        }
        string command((istreambuf_iterator<char>(commandFile)), istreambuf_iterator<char>());
        commandFile.close();

        // Embed and extract message
        stego.embedMessage(command, imagePath, outputImagePath, logger);
        string extractedCommand = stego.extractMessage(outputImagePath, logger);
        cout << "Extracted Command: " << extractedCommand << endl;
    } catch (const runtime_error &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

//./steganography <input_image> <command_file> <output_image>

