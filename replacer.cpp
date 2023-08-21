#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <regex>
#include <chrono>
#include <thread>

int main() {
    std::string sourceFileName, replacementFileName, outputFileName;
    std::cout << "Enter the location of the input file (name of story script text file followed by .txt): ";
    std::cin >> sourceFileName;
    std::cout << "Enter the location of the replacement (.csv) file: ";
    std::cin >> replacementFileName;
    std::cout << "Enter the location of the output file (name of formatted output file followed by .txt): ";
    std::cin >> outputFileName;


    std::ifstream sourceFile(sourceFileName);
    if (!sourceFile.is_open()) {
        std::cerr << "Error: Source file not found or could not be opened." << std::endl;
        return 1;
    }

    std::ifstream replacementFile(replacementFileName);
    if (!replacementFile.is_open()) {
        std::cerr << "Error: Replacement file not found or could not be opened." << std::endl;
        return 1;
    }

    std::ofstream outputFile(outputFileName);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not create or open the output file." << std::endl;
        return 1;
    }

    // Extract the number following "text" and the text after the first quotation mark
    std::map<std::string, std::string> replacementMap;
    std::string replacementLine;
    //get each string in the csv file line by line
    while (std::getline(replacementFile, replacementLine)) {
        //number of extra quotation marks after text####: is 2 in the csv file compared to the game script's .txt files for dialogue
        const std::size_t OFFSET = 2;
        std::smatch match;
        if (std::regex_search(replacementLine, match, std::regex("text([0-9]+)"))) {
            
            
            //find the first occurence of ", which comes after the text### labels, the ; symbol and the two extra quotation marks.
            std::size_t replacementTextStartPos = replacementLine.find("\"") + OFFSET;
            //find the length of the entire line of the csv file but subtract the offset from it because we don't want to store the last two quotation marks.
            std::size_t replacementTextEndPos = replacementLine.length() - OFFSET;
            std::size_t replacementTextLength = replacementTextEndPos - replacementTextStartPos;
            //store everything from the third quotation mark on this line until the 3rd-to-last character, which is the last quotation mark we need, as a new string.
            std::string textInsideQuotes = replacementLine.substr(replacementTextStartPos, replacementTextLength);
            //store the value of the text on this line with its corresponding text number.
            replacementMap[match[0]] = textInsideQuotes;
        }
    }

    std::string sourceLine;
    while (std::getline(sourceFile, sourceLine)) {
        std::string updatedLine = sourceLine;
        std::smatch match;
        if (std::regex_search(sourceLine, match, std::regex("text([0-9]+)"))) {
            for (const auto& entry : replacementMap) {
                const std::string& textNumber = entry.first;
                const std::string& replacementText = entry.second;
                if (textNumber == match[0]) {
                    //Find the first quotation mark and the end of the line and replace it with the properly formatted text line from the translation csv file in the string variable updatedLine.
                    std::size_t pos = updatedLine.find("\"");
                    std::size_t sourceLineLength = updatedLine.length();
                    updatedLine.replace(pos, sourceLineLength, replacementText);
                }

            }
        }
        //updatedLine is only changed when a line containing the word "text" is found and matched with a line in the csv file to replace the text with its translation.
        //Lines with only script commands are written to the output file unmodified. 
        outputFile << updatedLine << std::endl;
    }
    std::cout << "Replacement completed. Output written to " << outputFileName << std::endl << "Closing in 3 seconds." << std::endl;

    sourceFile.close();
    replacementFile.close();
    outputFile.close();

    // Add a delay before closing the program
    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}
