#ifndef __ROBOT_HH__
#define __ROBOT_HH__

#include <cstdio>
#include <utility>

#include <libplayerc++/playerc++.h>

using namespace PlayerCc;

struct Item
{
    Item(player_point_2d p, bool av = true) : pos(p), available(av) {}
    player_point_2d pos;
    bool available;
};

class Robot
{
public:
    /**
     * Smmalest significant distance.
     */
    static constexpr double threshold = 0.25;

    Robot(const std::string &host, unsigned short port = 6665);
    virtual ~Robot();

    virtual void Update();
    virtual void ReportStatus(std::FILE *f);
    void PrintId(std::FILE *f)
    {
        std::fprintf(f, "[%s:%u]", client.GetHostname().c_str(), client.GetPort());
    }

    void set_max_speed(double s) {max_speed = s;}
    void set_route(const std::vector<player_point_2d> &r) {route = r;}
    void set_route(std::vector<player_point_2d> &&r) {route = std::move(r);}

protected:
    static std::vector<Item> items;

    void CollAvoid();
    void SpotItems();
    void GoToPoint(const player_point_2d &p);

    virtual void Panic(const char* reason = NULL);

private:
    double fspeed, tspeed;
    double max_speed;

    PlayerClient client;
    Position2dProxy p2d;
    BlobfinderProxy blob;
    RangerProxy laser;

    std::vector<player_point_2d> route;
    unsigned target_point;

};
#endif /* ifndef __ROBOT_HH__ */
