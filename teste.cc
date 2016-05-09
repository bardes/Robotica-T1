#include "Robot.hh"

int main(int argc, char* argv[])
{
    std::vector<player_point_2d> route;
    player_point_2d p;

    p.px = 5; p.py = 0;
    route.push_back(p);
    p.px = 10; p.py = 0;
    route.push_back(p);

    Robot MyRobot("localhost");
    MyRobot.set_route(std::move(route));

    for(unsigned i = 0; ; ++i) {
        MyRobot.Update();
#ifdef DEBUG
        MyRobot.ReportStatus(stderr);
#endif
    }

    return 0;
}
