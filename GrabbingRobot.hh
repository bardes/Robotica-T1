#ifndef __GRABBING_ROBOT_H__
#define __GRABBING_ROBOT_H__

#include "Robot.hh"

class GrabbingRobot : Robot
{
public:
    enum State { ROAM, PURSUIT, GRAB, CARRY };

    GrabbingRobot(const std::string& host, short unsigned int port);
    virtual void Update();

private:
    State currState;
};

#endif
