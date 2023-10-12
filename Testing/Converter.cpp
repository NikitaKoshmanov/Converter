#include <iostream>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>
#include <sstream>

// Интерфейсы Source и Sink
class Source {
public:
    virtual ~Source() {}
    virtual uint8_t readByte() = 0;
};

class Sink {
public:
    virtual ~Sink() {}
    virtual void writeString(const std::string& str) = 0;
};

class MockSource : public Source
{
public:
    uint8_t readByte() override {
    }
};

class MockSink : public Sink
{
public:
    void writeString(const std::string& str) override {
    }
};

// Класс Converter
class Converter {
public:
    Converter(Source* source, Sink* sink) : source(source), sink(sink), running(false) {}
    void start() {
        if (!running) {
            running = true;
            thread = std::thread(&Converter::run, this);
        }
    }

    void stop() {
        if (running) {
            running = false;
            cv.notify_all();
            thread.join();
        }
    }

private:
    Source* source;
    Sink* sink;
    std::thread thread;
    std::atomic<bool> running;
    std::mutex mutex;
    std::condition_variable cv;
    std::queue<uint8_t> buffer;

    void run() {
        while (running) {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this] { return !buffer.empty() || !running; });

            if (!buffer.empty()) {
                uint8_t byte = buffer.front();
                buffer.pop();
                lock.unlock();

                std::string result = convertByte(byte);
                sink->writeString(result);
            }
        }
    }

    std::string convertByte(uint8_t byte) {
        uint8_t type = (byte >> 6) & 0x03;
        uint8_t data = byte & 0x3F;

        std::stringstream ss;
        if (type == 0b00) {
            ss << "Unsigned integer: " << static_cast<unsigned int>(data);
        }
        else if (type == 0b01) {
            ss << "Signed integer: " << static_cast<int>(data);
        }
        else if (type == 0b10) {
            char letter = 'a' + data;
            ss << "Letter: " << letter;
        }
        else {
            ss << "Invalid type";
        }

        return ss.str();
    }
};

int main() {
    std::cout << "Hello";
}
