#include <iostream>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

TrafficLightPhase MessageQueue::receive()
{
    // X FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // X to wait for and receive new messages and pull them from the queue using move semantics. 
    // X The received object should then be returned by the receive function.
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this] { return !_queue.empty(); }); // pass unique lock to condition variable

    // remove last element from queue
    TrafficLightPhase phase = std::move(_queue.back());
    //_queue.pop_back();
    _queue.clear();

    return phase; // will not be copied due to return value optimization (RVO) in C++ 
}

void MessageQueue::send(TrafficLightPhase &&msg)
{
    // X FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // X as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    
    // perform queue mod under lock
    std::lock_guard<std::mutex> lock(_mutex);

    _queue.emplace_back(std::move(msg));
    _cond.notify_one(); // notify client after pushing new message into queue
}


/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    //X FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    //X runs and repeatedly calls the receive function on the message queue. 
    //X Once it receives TrafficLightPhase::green, the method returns.
    while(true)
    {
        if (_msg_queue.receive() == TrafficLightPhase::green)
        {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // X FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// FIXME: send update to message queue
// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : 
    // X Implement the function with an infinite loop that measures the time between two loop cycles 
    // X and toggles the current phase of the traffic light between red and green and sends an update method 
    // X to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // X Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

    std::random_device randDevice;
    static std::mt19937 mt(randDevice());
    static std::uniform_real_distribution<double> distribution(4000, 6000);
    float randomTime = distribution(mt);

    auto start = std::chrono::system_clock::now();

    while(true)
    {
        auto timeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if(timeDelta >= randomTime)
        {
            toggleCurrentPhase();

            _msg_queue.send(std::move(_currentPhase));

            randomTime = distribution(mt);
            start = std::chrono::system_clock::now();
    
        }
      
    }
}

void TrafficLight::toggleCurrentPhase()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if(_currentPhase == TrafficLightPhase::red)
    {
        _currentPhase = TrafficLightPhase::green;
    }
    else
    {
        _currentPhase = TrafficLightPhase::red;
    }
}
