#include "TrafficLight.h"
#include <chrono>
#include <iostream>
#include <random>
#include <thread>

using Clock = std::chrono::high_resolution_clock;

/* Implementation of class "MessageQueue" */

template <typename T>
T MessageQueue<T>::receive() {
    std::unique_lock<std::mutex> u_lock(_mutex);
    _condition.wait(u_lock, [this] { return !_queue.empty(); });

    T message = std::move(_queue.back());
    _queue.pop_back();

    return message;
}

template <typename T>
void MessageQueue<T>::send(T&& msg) {
    std::lock_guard<std::mutex> lock(_mutex);

    _queue.push_back(std::move(msg));

    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() { _currentPhase = TrafficLightPhase::red; }

void TrafficLight::waitForGreen() {
    while(true)
        if(_msg_q.receive() == TrafficLightPhase::green)
            return;
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
    // The cycle duration should be a random value between 4 and 6 seconds.
    auto uniform = std::uniform_real_distribution<double>(4, 6);
    std::default_random_engine engine;
    auto target_cycle_duration = std::chrono::duration<double>(uniform(engine));

    auto stop_watch = Clock::now();

    // Implement the function with an infinite loop
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // measure the time between two loop cycles
        auto diff_time =
            std::chrono::duration<double>(Clock::now() - stop_watch);

        if(diff_time > target_cycle_duration) {
            // toggles the current phase of the traffic light between red and
            // green
            if(_currentPhase == TrafficLightPhase::red)
                _currentPhase = TrafficLightPhase::green;
            else
                _currentPhase = TrafficLightPhase::red;

            // send an update method to the message queue using move semantics.
            _msg_q.send(std::move(getCurrentPhase()));

            target_cycle_duration =
                std::chrono::duration<double>(uniform(engine));
            stop_watch = Clock::now();
        }
    }
}
