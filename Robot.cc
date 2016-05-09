#include "Robot.hh"

#include <cmath>

extern "C" {
#include "utils.h"
}

std::vector<Item> Robot::items = std::vector<Item>();

Robot::Robot(const std::string &host, short unsigned int port):
fspeed(0), tspeed(0), max_speed(1.2),
client(host, port), p2d(&client, 0), blob(&client, 0), laser(&client, 1),
target_point(0)
{
    p2d.SetSpeed(fspeed, tspeed); p2d.SetMotorEnable(1);
    DMSG("CREATING ROBOT [%s:%u]", host.c_str(), port);
}

void Robot::Panic(const char *reason)
{
    PrintId(stderr);
    std::fprintf(stderr, " PANIC");
    if(reason)
        std::fprintf(stderr, ": %s", reason);
    std::fputc('\n', stderr);

    p2d.SetSpeed(0, 0);
    p2d.SetMotorEnable(0);
}

Robot::~Robot()
{
    DMSG("DESTROYING [%s:%u]", client.GetHostname().c_str(), client.GetPort());
    p2d.SetSpeed(0, 0);
    p2d.SetMotorEnable(0);
}

static double distance(const player_point_2d &a, const player_point_2d &b)
{
    double dx = a.px - b.px;
    double dy = a.py - b.py;
    return std::sqrt(dx*dx + dy*dy);
}

void Robot::Update()
{
    client.Read();

    // Procura os items próximos
    SpotItems();

    // Tenta se mover em diração ao próximo destino
    GoToPoint(route[target_point]);

    // Aplica correções para evitar colisões
    CollAvoid();

    p2d.SetSpeed(fspeed, tspeed);
}

void Robot::SpotItems()
{
    static const float fov = M_PI/3;
    static const float scanw = 80;

    player_point_2d curr_pos;
    curr_pos.px = p2d.GetXPos();
    curr_pos.py = p2d.GetYPos();
    float teta = p2d.GetYaw();

    auto count = blob.GetCount();
    for(unsigned i = 0; i < count; ++i) if(blob[i].color == 0x00ff00u) {
        // Calcula a posição absoluta do item
        player_point_2d p;
        float phi = ((blob[i].x / scanw) - .5f) * fov;
        p.px = blob[i].range * std::cos(teta + phi) + curr_pos.px;
        p.py = blob[i].range * std::sin(teta + phi) + curr_pos.py;

        items.push_back(Item(p));
    }

    // Remove entradas duplicadas
    for(unsigned i = 0; i < items.size(); ++i) {
        for(unsigned j = i+1; j < count; ++j) {
            if(distance(items[i].pos, items[j].pos) <= threshold) {
                items[i].pos.px += items[j].pos.px;
                items[i].pos.py += items[j].pos.py;
                items[i].pos.px /= 2;
                items[i].pos.py /= 2;
                items.erase(items.begin() + j);
            }
         }
    }
}

void Robot::GoToPoint(const player_point_2d& target)
{
    player_point_2d curr_pos;
    curr_pos.px = p2d.GetXPos();
    curr_pos.py = p2d.GetYPos();

    // Checa se chegou no destino
    double target_distance = distance(route[target_point], curr_pos);
    if(target_distance < threshold) {
        target_point = (target_point + 1) % route.size();
        DMSG("[%s:%u] TARGET SET (%.3lg, %.3lg)", client.GetHostname().c_str(),
            client.GetPort(), route[target_point].px, route[target_point].py);
    }

    // Calcula o ângulo entre o robo e o alvo
    double target_angle = std::atan2(target.py - curr_pos.py,
                                     target.px - curr_pos.px);

    // Calcula a diferença entre os ângulos
    double ang_diff = target_angle - p2d.GetYaw();

    // Mapeia a diferença para o intervalo [-pi, pi]
    ang_diff = std::atan2(std::sin(ang_diff), std::cos(ang_diff));

    // Noramliza a diferença para o intervalo [-1, 1]
    ang_diff = ang_diff / M_PI;

    // Calcula a velocidade para fente com base na vel. angular
    fspeed = distance(target, curr_pos) < 5 * threshold ?
             (1 - std::pow(std::fabs(ang_diff), .15)) * max_speed : max_speed;
    DMSG("angdiff: %g, fspeed: %g", ang_diff, fspeed);

    // Aplica um expoente para deixar a correção inicial mais intensa, porém
    // reduzindo conforme os ângulos se aproximam
    tspeed = std::copysign(std::pow(std::fabs(ang_diff), .5), ang_diff);
}

static double p(double d)
{
    static const double gamma=5, tau = 8;
    d = 1 - std::pow(d, gamma);
    d /= std::pow(1 + d, tau);
    return d;
}

void Robot::CollAvoid()
{
    static const double dist_threshold = 1.2;
    static const double min_dist = .7;

    unsigned count = laser.GetRangeCount();
    double nearest_dist = laser.GetMaxRange();
    double nearest_ang = 0;
    double obstacle_score = 0;

    unsigned i;
    for(i = 0; i < count / 2; ++i) {
        if(laser[i] < nearest_dist) {
            nearest_dist = laser[i];
            nearest_ang = (i * laser.GetMaxAngle()) / (count - 1);
        }
        obstacle_score -= p(laser[i] / laser.GetMaxRange());
    }
    if(count % 2) {
        if(laser[i] < nearest_dist) {
            nearest_dist = laser[i];
            nearest_ang = (i * laser.GetMaxAngle()) / (count - 1);
        }
        ++i;
    }
    for(; i < count / 2; ++i) {
        if(laser[i] < nearest_dist) {
            nearest_dist = laser[i];
            nearest_ang = (i * laser.GetMaxAngle()) / (count - 1);
        }
        obstacle_score += std::pow(laser[i], -5);
    }

    nearest_ang -= (nearest_ang / 2);

    if(nearest_dist < dist_threshold) {
        tspeed = -std::copysign(1, obstacle_score);
        if(nearest_dist < min_dist) fspeed = 0;
        fspeed *= std::pow(nearest_ang / M_PI_2, 1.5);
        DMSG("!COLLISION AVOIDANCE\tnd: %g, na%g, sc:%g",
             nearest_dist, nearest_ang, obstacle_score);
    }
}

void Robot::ReportStatus(FILE* f)
{
    player_point_2d pos; pos.px = p2d.GetXPos(); pos.py = p2d.GetYPos();
    PrintId(f); std::fputc('\n', f);
    std::fprintf(f, "fspeed: %.3lg\ntspeed: %.3lg\nposition: (%.3lg, %.3lg)"
        "\norientation: %.3lg (%.3lg deg)\ntarget: %u (%.3lg dist)\n", fspeed,
        tspeed, pos.px, pos.py, p2d.GetYaw(), p2d.GetYaw() * (180/M_PI),
        target_point, distance(pos, route[target_point]));

    std::fprintf(f, "Items:");
    for(Item &i : items) std::fprintf(f, " (%.1f, %.1f)", i.pos.px, i.pos.py);
    std::fputc('\n', f);

}
