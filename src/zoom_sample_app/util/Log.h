#ifndef MEETING_SDK_LINUX_SAMPLE_LOG_H
#define MEETING_SDK_LINUX_SAMPLE_LOG_H

#include <string>
#include <iostream>

using namespace std;

namespace Emoji {
    const string checkMark = "✅";
    const string crossMark = "❌";
    const string hourglass = "⏳";
}

class Log {
    public:
        static void success(const string& message) {
            cout << Emoji::checkMark << " " << message << endl;
        }

        static void info(const std::string& message) {
            cout << Emoji::hourglass << " " << message << endl;

        }

        static void error(const string& message) {
            cerr << Emoji::crossMark << " " << message << endl;
        }
};


#endif //MEETING_SDK_LINUX_SAMPLE_LOG_H
