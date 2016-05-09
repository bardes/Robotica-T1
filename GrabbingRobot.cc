#include "GrabbingRobot.hh"

GrabbingRobot::GrabbingRobot(const std::string& host,
                             short unsigned int port):
Robot(host, port), currState(ROAM)
{
}

void GrabbingRobot::Update()
{
    switch(currState) {
        case ROAM:
            Robot::Update();
            return;

        default:
            Robot::Panic();
    }
}
