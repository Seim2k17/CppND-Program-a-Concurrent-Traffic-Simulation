#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;


enum class TrafficLightPhase{
    red,
    green
};

// X FP.3 Define a class „MessageQueue“ which has the public methods send and receive. 
// X Send should take an rvalue reference of type TrafficLightPhase whereas receive should return this type. 
// X Also, the class should define an std::dequeue called _queue, which stores objects of type TrafficLightPhase. 
// X Also, there should be an std::condition_variable as well as an std::mutex as private members. 

class MessageQueue
{
public:
    TrafficLightPhase receive();
    void send(TrafficLightPhase &&msg);

private:
    std::deque<TrafficLightPhase> _queue;
    std::condition_variable _cond;
    std::mutex _mutex;
    
};

// X FP.1 : Define a class „TrafficLight“ which is a child class of TrafficObject. 
// X The class shall have the public methods „void waitForGreen()“ and „void simulate()“ 
// X as well as „TrafficLightPhase getCurrentPhase()“, where TrafficLightPhase is an enum that 
// X can be either „red“ or „green“. Also, add the private method „void cycleThroughPhases()“. 
// X Furthermore, there shall be the private member _currentPhase which can take „red“ or „green“ as its value. 

class TrafficLight : public TrafficObject, public std::enable_shared_from_this<TrafficLight>
{
public:
    // constructor / desctructor
    TrafficLight();
    ~TrafficLight(){};

    // getters / setters

    // typical behaviour methods
    void waitForGreen();
    void simulate();
    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    void toggleCurrentPhase();

    //X FP.4b : create a private member of type MessageQueue for messages of type TrafficLightPhase 
    //X and use it within the infinite loop to push each new TrafficLightPhase into it by calling 
    //X send in conjunction with move semantics.
    MessageQueue _msg_queue;

    std::mutex _mutex;
    TrafficLightPhase _currentPhase;

};

#endif