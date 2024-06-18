#ifndef THREAD_SAFE_LOGGER_H
#define THREAD_SAFE_LOGGER_H

#include <iostream>
#include <sstream>
#include <mutex>

class ThreadSafeLogger {
public:
    // Deleted copy constructor and assignment operator to ensure singleton properties
    ThreadSafeLogger(const ThreadSafeLogger&) = delete;
    ThreadSafeLogger& operator=(const ThreadSafeLogger&) = delete;

    // Method to get the singleton instance of the logger
    static ThreadSafeLogger& getInstance() {
        static ThreadSafeLogger instance;
        return instance;
    }

    // Template function for handling various types with the << operator
    template<typename T>
    ThreadSafeLogger& operator<<(const T& msg) {
#ifndef NDEBUG
        std::ostringstream& stream = getThreadLocalStream();
        stream << msg;
#endif
        return *this;

    }

    // Flushes the current thread's log stream to std::cout
    void flush() {
#ifndef NDEBUG

        std::lock_guard<std::mutex> lock(mutex_);
        std::ostringstream& stream = getThreadLocalStream();
        std::cout << stream.str();
        stream.str(""); // Clear the stream buffer after flushing
        stream.clear(); // Clear any error flags
#endif
    }

private:
    ThreadSafeLogger() = default;

    static std::ostringstream& getThreadLocalStream() {
        thread_local std::ostringstream stream;
        return stream;
    }

    std::mutex mutex_;
};

// Helper function that flushes the log on destruction, ensuring message completion
class LogFlusher {
public:
    ~LogFlusher() {
        ThreadSafeLogger::getInstance().flush();
    }
};

// Macro to simplify usage and ensure flush on destruction
#define debug_log LogFlusher(), ThreadSafeLogger::getInstance()

#endif // THREAD_SAFE_LOGGER_H
