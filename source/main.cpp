#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <thread>
#include "../include/download.hpp"
#include "../include/json.hpp"

using json = nlohmann::json;

template <typename T>
T fromString(const std::string& input) {
    std::istringstream iss(input);
    T output;
    iss >> output;
    return output;
}

std::string convert12(std::string str) 
{ 
    std::string outputStr;

    // Get Hours 
    int h1 = (int)str[0] - '0'; 
    int h2 = (int)str[1] - '0'; 
  
    int hh = h1 * 10 + h2; 
  
    // Finding out the Meridien of time 
    // ie. AM or PM 
    std::string Meridien; 
    if (hh < 12) { 
        Meridien = "AM"; 
    } 
    else
        Meridien = "PM"; 
  
    hh %= 12; 
  
    // Handle 00 and 12 case separately 
    if (hh == 0) { 
        outputStr.append("12");
        // Printing minutes and seconds 
        for (int i = 2; i < 8; ++i) { 
            outputStr.append(std::string(1, str[i]));
        } 
    } 
    else { 
        outputStr.append(std::to_string(hh));
        // Printing minutes and seconds 
        for (int i = 2; i < 8; ++i) { 
            outputStr.append(std::string(1, str[i]));
        } 
    } 
    
    outputStr.append(" ");
    outputStr.append(Meridien);

    return outputStr;
}

std::string cut_meridien(const std::string& str)
{
    std::string output = str;
    size_t AM_Pos = output.find('A');
    size_t PM_Pos = output.find('P');

    if (AM_Pos != std::string::npos) {
        output = output.substr(0, AM_Pos);
    } else if (PM_Pos != std::string::npos) {
        output = output.substr(0, PM_Pos);
    }

    return output;
}

int main(int argc, char** argv) {
    downloadFile(CIRCLE_USER_INFO_LINK, "USERINFO.json");

    std::fstream userInfoFile;
    userInfoFile.open("USERINFO.json", std::ios::in);
    json circleJson;
    userInfoFile >> circleJson;

    std::map<std::string, std::pair<std::string, std::pair<std::string, std::string>>> categories;
    std::map<std::string, std::string> bedtime;

    try {
        for (auto& [key, value] : circleJson["info"].items()) {
            for (auto& [sKey, sValue] : circleJson["info"]["limits"].items()) {
                auto categoryValue = sValue["category"].get<std::string>();
                auto minuteValue = sValue["minutes"].get<std::string>();
                auto dayValue = sValue["days"].get<std::string>();
                
                auto firstPair = std::make_pair(minuteValue, dayValue);
                auto secondPair = std::make_pair(categoryValue, firstPair);
                categories[sKey] = secondPair;
            }

            if (key == "bedtime") {
                bedtime["Start"] = value["start"];
                bedtime["End"] = value["end"];
            }
        }
    } catch (nlohmann::detail::type_error& e) {
        std::cerr << "Error: " << e.what() << '\n';
    }

    std::cout << "All categories under time limits: " << '\n';
    std::cout << "|    Category     |     Time Limit    |     Days    |" << '\n';
    std::cout << "----------------------------------------------------" << '\n';

    std::string currentMinuteElem;
    std::string currentDayElem;

    for (const auto& x : categories) {  
        std::cout << x.second.first << "   |   " << x.second.second.first << "minutes    |   " << x.second.second.second << '\n';
        std::cout << "----------------------------------------------------" << '\n';
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(8000)); // 5 seconds

    std::cout << "\nBedtime: " << '\n';

    std::string bedtime_start_12;
    std::string bedtime_end_12;

    for (const auto& x : bedtime) {
        if (x.first == "Start")
            bedtime_start_12 = convert12(x.second);
        else if (x.first == "End")
            bedtime_end_12 = convert12(x.second);
    }

    std::cout << "Start: " << bedtime_start_12 << '\n' <<
    "End: " << bedtime_end_12 << '\n';

    std::this_thread::sleep_for(std::chrono::milliseconds(3000)); // 5 seconds

    std::cout << '\n' << "Category usage for today: " << '\n';
    std::cout << "|    Category     |     Time Spent    |" << '\n';
    std::cout << "-----------------------------------------------" << '\n';

    for (auto& [key, value] : circleJson["info"]["categoryusage"].items()) {
        std::cout << key << "   |   " << value.get<std::string>() << " minutes" << '\n';
        std::cout << "-----------------------------------------------" << '\n';
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10000)); // 5 seconds

    std::cout << '\n' << "Websites usage for today: " << '\n';
    std::cout << "Warning: Usually this would be formatted, this is not totally formatted." << '\n' << '\n';

    for (auto& [key, value] : circleJson["info"]["siteusage"].items()) {
        std::cout << key << "   |   " << value << '\n';
    }
}