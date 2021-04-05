#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include "TrafficObject.h"
#include <condition_variable>
#include <deque>
#include <mutex>

// forward declarations to avoid include cycle
class Vehicle;

enum class TrafficLightPhase { red, green };

template <class T>
class MessageQueue {
public:
    void send(T&& TrafficLight);
    T receive();

private:
    std::condition_variable _condition;
    std::mutex _mutex;
    std::deque<T> _queue;
};

class TrafficLight : public TrafficObject {
public:
    // constructor / desctructor
    TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods

    void cycleThroughPhases();

    MessageQueue<TrafficLightPhase> _msg_q;

    TrafficLightPhase _currentPhase;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif // TRAFFICLIGHT_H
