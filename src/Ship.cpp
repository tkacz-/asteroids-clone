#include "../include/Ship.h"
#include <cmath>
#include <algorithm>

Ship::Ship()
{
    prevPosition = {400, 300};
    position = {400, 300};
    ship = new SDL_Point[5];
    thruster = new SDL_Point[3];
    colPoints = new SDL_Rect[3];
    coefArr = readConfig("config/Ship.txt");

    velocity[0] = 0.0;
    velocity[1] = 0.0;

    this->trace();
}

Ship::~Ship()
{
    delete[] ship;
    delete[] thruster;
    delete[] colPoints;
    delete[] coefArr;
}

void Ship::trace()
{
    cosA = cos(angle);
    sinA = sin(angle);

    int i = 0;
    for (; i < 5; i++) {
        ship[i] = baseFormula(coefArr[i][0], coefArr[i][1], coefArr[i][2], coefArr[i][3]);
    }
    i = 5;
    for (int j = 0; j < 3; j++) {
        thruster[j] = baseFormula(coefArr[i][0], coefArr[i][1], coefArr[i][2], coefArr[i][3]);
        i++;
    }
    for (int j = 0; j < 2; j++) {
        colPoints[j].x = ship[j].x;
        colPoints[j].y = ship[j].y;
        colPoints[j].w = 2;
        colPoints[j].h = 2;
    }
    colPoints[2].x = ship[3].x;
    colPoints[2].y = ship[3].y;
    colPoints[2].w = 2;
    colPoints[2].h = 2;
}

void Ship::updatePosition(const float& dt)
{
    if (thrust) {
        speed += 1;
        if (speed > 5) {
            speed = 5;
        }
        velocity[0] += speed * cosA;
        velocity[1] += speed * sinA;
    } else {
        speed = 0;
    }
    switch (rot) {
        case LEFT: {
            angle -= 0.175;
            break;
        }
        case RIGHT: {
            angle += 0.175;
            break;
        }
        case NONE: {
            break;
        }
    }

    position.x = prevPosition.x - velocity[0] * dt;
    position.y = prevPosition.y - velocity[1] * dt;

    if (position.x > 800) {
        position.x -= 800;
    }
    if (position.x < 0) {
        position.x += 800;
    }
    if (position.y > 600) {
        position.y -= 600;
    }
    if (position.y < 0) {
        position.y += 600;
    }
    for (auto i = bullets.begin(); i != bullets.end(); ) {
        if ((i -> second).isDead())
            i = bullets.erase(i);
        else
            ++i;
    }
    for (auto i = bullets.begin(); i != bullets.end(); i++) {
        i->second.updatePosition(dt, bullets);
    }
    prevPosition.x = position.x;
    prevPosition.y = position.y;
}

void Ship::interpolate(const float& dT, const float& i)
{
    position.x = prevPosition.x - (velocity[0] * dT) * i;
    position.y = prevPosition.y - (velocity[1] * dT) * i;
    if (alive) {
        this->trace();
    } else {
        this->deadAnimation();
    }

    for (auto j = bullets.begin(); j != bullets.end(); j++)
    {
        j->second.interpolate(dT, i);
    }
}

void Ship::draw(SDL_Renderer* r)
{
    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);

    if (alive)
    {
        if (thrust) {
            SDL_RenderDrawLines(r, ship, 5);
            SDL_SetRenderDrawColor(r, 255, 255, 0, 0);
            SDL_RenderDrawLines(r, thruster, 3);
        } else {
            SDL_RenderDrawLines(r, ship, 5);
        }
        for (auto i = bullets.begin(); i != bullets.end(); i++) {
            i->second.draw(r);
        }
    }
}

void Ship::fire()
{
    SDL_Point p = baseFormula(coefArr[0][0], coefArr[0][1], coefArr[0][2], coefArr[0][3]);

    float vX = velocity[0] + 500 * cosA;
    float vY = velocity[1] + 500 * sinA;

    std::stringstream ss;
    ss << bulletNum;
    std::string name = "bullet" + ss.str();

    Bullet sp = {p, vX, vY, BULLET, bulletNum};

    bullets.insert(std::pair<std::string, Bullet>(name, sp));
    bulletNum++;
}

void Ship::deadAnimation()
{
    velocity[0] = 0;
    velocity[1] = 0;
}

SDL_Point Ship::baseFormula(const int& a, const int& b, const int& c, const int& d)
{
    return {int(cosA * a - sinA * b + position.x),
                int(sinA * c + cosA * d + position.y)};
}

int** Ship::readConfig(const std::string& filename)
{
    std::ifstream config (filename);
    if (config.is_open()) {
        int countM;
        int countN;
        config >> countM;
        config >> countN;
        int **arr = new int*[countM];
        for (int i = 0; i < countM; i++) {
            arr[i] = new int[countN];
            for (int j = 0; j < countN; j++) {
                config >> arr[i][j];
            }
        }
        config.close();
        return arr;
    } else {
        std::cout << "Error: Not found config file at " << filename << std::endl;
        return nullptr;
    }
}

