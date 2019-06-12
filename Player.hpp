#ifndef PLAYER_HPP_INCLUDED
#define PLAYER_HPP_INCLUDED

#include "Entity.hpp"


class Player: public Entity
{
public:
    enum {STAND, AIR, STEP_1, STEP_2, TARGET_LOW, TARGET_MID, TARGET_HIGH, TARGET_UP, DEAD, N_OF_NAMES};
    sf::Vector2i texture_pos[20];
    sf::Vector2i texture_size;

    void setTexture(int name)
    {
        if(name < 0 || name > N_OF_NAMES)
            return;
        sprite.setTextureRect(sf::IntRect(texture_pos[name], texture_size));
    }

    Player()
        : Entity(150, 100)
        , name("Player")
        {
            texture_size = sf::Vector2i(19, 29);
            {
                int x = texture_size.x,
                    y = texture_size.y;
                texture_pos[STAND]  = sf::Vector2i(0*x, 0*y);
                texture_pos[AIR]    = sf::Vector2i(4*x, 0*y);
                texture_pos[STEP_1] = sf::Vector2i(1*x, 0*y);
                texture_pos[STEP_2] = sf::Vector2i(2*x, 0*y);
                texture_pos[TARGET_LOW]  = sf::Vector2i(3*x, 1*y);
                texture_pos[TARGET_MID]  = sf::Vector2i(0*x, 1*y);
                texture_pos[TARGET_HIGH] = sf::Vector2i(1*x, 1*y);
                texture_pos[TARGET_UP]   = sf::Vector2i(2*x, 1*y);
                texture_pos[DEAD]    = sf::Vector2i(3*x, 0*y);
            }

            velocity = 10;
            targeting = false;

            sprite.setTexture(*texture_base.getMan(), true);
            setTexture(STAND);
            sprite.setPosition(x, y);
            sprite.setOrigin(texture_size.x / 2, texture_size.y / 2);

            area = sprite.getGlobalBounds();
        }


    virtual void move(sf::Time elapsedTime);

    //--- Debugging tools ---//
    #ifndef NDEBUG
    virtual void Dump() const
    {
        printf("DUMPING \"Player\" at [%p]:\n", this);

        Entity::Dump();

        puts("// Player:");

        printf("\tname: \"%s\"\n", name);

        puts("//~Player");
    }
    #endif // NDEBUG

private:
public:
    char name[NAMELEN];
    bool targeting;
    int angle;
};















void Player::move(sf::Time elapsedTime)
{
    static int step = 0;
    tick += elapsedTime.asMilliseconds();
    if(!((blocked.left && vx < 0) || (blocked.right && vx > 0)))
        x += elapsedTime.asSeconds() * vx * velocity;
    if(!((blocked.up   && vy < 0) || (blocked.down  && vy > 0)))
        y += elapsedTime.asSeconds() * vy * 10;
    else
        vy = 0;

    if(!blocked.down)
        vy += 0.2;

    if(!blocked.down)
        setTexture(AIR);
    else if(vx != 0)
    {
        if(tick > 200)
        {
            int pos = 0;
            switch(step)
            {
            case 1:
                pos = STEP_1;
                break;

            case 3:
                pos = STEP_2;
                break;

            case 0:
            case 2:
            default:
                pos = STAND;
                break;
            }
            setTexture(pos);
            step++;
            tick = 0;
            if(step >= 4)
                step = 0;
        }
    }
    else
    {
        if(targeting)
        {
            int a = angle;
            if(angle < 0)
            {
                sprite.setScale(-1, 1);
                a = 180 + a;
            }
            else
                sprite.setScale(1, 1);

            if(abs(a) < 22)
                setTexture(TARGET_UP);
            else if(abs(a) < 68)
                setTexture(TARGET_HIGH);
            else if(abs(a) < 122)
                setTexture(TARGET_MID);
            else
                setTexture(TARGET_LOW);
        }
        else
            setTexture(STAND);
    }



    if(vx > 0)
        sprite.setScale(1, 1);
    else if(vx < 0)
        sprite.setScale(-1, 1);

    if(health <= 0)
    {
        setTexture(DEAD);
        health = 0;
    }


    sprite.setPosition(x, y);
    area = sprite.getGlobalBounds();
}

#endif // PLAYER_HPP_INCLUDED
