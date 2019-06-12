#ifndef ENTITY_HPP_INCLUDED
#define ENTITY_HPP_INCLUDED

#include "Environment.hpp"
#include <cmath>

#define NAMELEN (20 + 1)
#define MAX_HEALTH (1000)

class Entity: public sf::Drawable
{
public:
    Entity(int x_ = 100, int y_ = 100)
        : sf::Drawable()
        , sprite()
        , area()
        , ID(0)
        , x (x_)
        , y (y_)
        , velocity(5)
        , vx    (0.f)
        , vy    (0.f)
        , blocked ({})
        , pressedUp     (false)
        , pressedLeft   (false)
        , pressedDown   (false)
        , pressedRight  (false)
        , tick(0)
        , health(MAX_HEALTH)
    {

    }

    void handleKeyboard(sf::Keyboard::Key key, bool isPressed);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::RectangleShape rect(sf::Vector2f(area.width, area.height));
        rect.setPosition(area.left, area.top);
        rect.setFillColor(sf::Color::Green);
        //target.draw(rect);

        target.draw(sprite);

        rect.setSize(sf::Vector2f(20, 2));
        rect.setPosition(x, area.top - 3);
        rect.setOrigin(10, 1);
        rect.setFillColor(sf::Color::Black);
        target.draw(rect);

        rect.setSize(sf::Vector2f(health * 20 / MAX_HEALTH, 2));
        rect.setFillColor(sf::Color::Red);
        target.draw(rect);

        #ifndef NDEBUG
        Dump();
        #endif // NDEBUG
    }

    virtual void move(sf::Time elapsedTime);

    void handleBlocksCollisions(Environment& world, int autoJump_height);

    float getX()    { return area.left; }
    float getY()    { return area.top; }
    float getVx()   { return vx; }
    float getVy()   { return vy; }
    float getWidth()    { return area.width; }
    float getHeight()   { return area.height; }

    //--- Debugging tools ---//
    #ifndef NDEBUG
    virtual void Dump() const
    {
        //printf("\n\n\n\n\n");
        printf("DUMPING \"Entity\" at [%p]:\n", this);

        printf("\tID = %d\n", ID);
        printf("\tcoordinates: x = %f\n"
               "\t             y = %f\n", x, y);
        printf("\tvelocity: cx = %f\n"
               "\t          vy = %f\n", vx, vy);

        printf("\tarea: x = %f\n"
               "\t      y = %f\n", area.left, area.top);

        printf("\tblocked:\n"
               "\t| up:    %d\n"
               "\t| down:  %d\n"
               "\t| left:  %d\n"
               "\t| right: %d\n", blocked.up, blocked.down, blocked.left, blocked.right);

        printf("\tpressed:\n"
               "\t| up:    %d\n"
               "\t| down:  %d\n"
               "\t| left:  %d\n"
               "\t| right: %d\n", pressedUp, pressedDown, pressedLeft, pressedRight);

        puts("//~Entity");
    }
    #endif // NDEBUG

protected:
public:
    sf::Sprite      sprite;
    sf::FloatRect   area;

    unsigned int ID;
    float x,      y;
    int velocity;
    float vx,     vy;

    struct
    {
        bool left, up, right, down;
    } blocked;

    bool pressedUp,
         pressedLeft,
         pressedDown,
         pressedRight;

    bool autojumped;

    unsigned int tick;
    int health;
};

void Entity::handleKeyboard(sf::Keyboard::Key key, bool isPressed)
{
    if(health <= 0)
    {
        pressedUp = pressedLeft = pressedRight = pressedDown = false;
        vx = 0;
        return;
    }

    autojumped = false;
    switch(key)
    {
    case sf::Keyboard::Space:
    case sf::Keyboard::W:
        pressedUp = isPressed;
        if(isPressed && blocked.down)
        {
            vy -= 30;
            blocked.down = false;
        }
        break;

    case sf::Keyboard::S:
        pressedDown = isPressed;
        if(isPressed && !blocked.down)
            vy += isPressed * 10;
        break;

    case sf::Keyboard::A:
        pressedLeft = isPressed;
        break;

    case sf::Keyboard::D:
        pressedRight = isPressed;
        break;

    default:
        break;
    }

    vx = (pressedRight - pressedLeft) * 10;
}

void Entity::move(sf::Time elapsedTime)
{
    if(!((blocked.left && vx < 0)|| (blocked.right && vx > 0)))
        x += elapsedTime.asSeconds() * vx * velocity;
    if(!((blocked.up   && vy < 0) || (blocked.down  && vy > 0)))
        y += elapsedTime.asSeconds() * vy * 10;
    else
        vy = 0;

    if(!blocked.down)
        vy += 1;

    if(vx < 0)
        sprite.setScale(1, 1);
    else if(vx > 0)
        sprite.setScale(-1, 1);

    sprite.setPosition(x, y);
    area = sprite.getGlobalBounds();
}

void Entity::handleBlocksCollisions(Environment& world, int autoJump_height)
{
    if(health <= 0)
        autoJump_height = 0;

    float X = area.left;
    float Y = area.top;
    const int W  = area.width;
    const int H  = area.height;
    int step = world.getBlockWidth() / 2;

    blocked = {};

    if(vy <= 0)
    {
        bool found = false;
        for(int i = 0; i < W; i += step)
            if(world.isBlock(X + i, Y - 1))
            {
                found = true;
                blocked.up = true;
                break;
            }

        if(found)
            //--- Shifting downstairs ---//
            for(int i = 0; i < W; i += step)
                if(world.isBlock(X + i, Y))
                {
                    Y = world.ceilY(Y);
                    break;
                }
    }
    if(vy >= 0)
    {
        bool found = false;
        for(int i = 0; i < W; i += step)
            if(world.isBlock(X + i, Y + H))
            {
                found = true;
                blocked.down = true;
                break;
            }

        if(found)
            //--- Shifting upstairs ---//
            Y = world.floorY(Y + H) - H; //----------------------------------------------- ?
    }

    if(vx <= 0)
    {
        bool found = false;
        bool autoJump = !autojumped;
        for(int i = 0; i < H; i += step)
            if(world.isBlock(X - 1, Y + i))
            {
                found = true;
                blocked.left = true;
                if(i < H - BLOCKSIZE * autoJump_height)
                    autoJump = false;
                break;
            }

        if(found && autoJump && !blocked.up && blocked.down)
        {
            vy -= 10 + BLOCKSIZE * autoJump_height;
            blocked.down = false;
            autojumped = true;
        }

        if(found)
            //--- Shifting to right ---//
            for(int i = 0; i < H; i += step)
                if(world.isBlock(X, Y + i))
                {
                    X = world.ceilX(X);
                    break;
                }
    }
    if(vx >= 0)
    {
        bool found = false;
        bool autoJump = !autojumped;
        for(int i = 0; i < H; i += step)
            if(world.isBlock(X + W, Y + i))
            {
                found = true;
                blocked.right = true;
                if(i < H - BLOCKSIZE * autoJump_height)
                    autoJump = false;
                break;
            }

        if(found && autoJump && !blocked.up && blocked.down)
        {
            vy -= 10 + BLOCKSIZE * autoJump_height;
            blocked.down = false;
            autojumped = true;
        }

        if(found)
            //--- Shifting to left ---//
            X = world.floorX(X + W) - W; //----------------------------------------------- ?
    }

    x = X + (x - area.left);
    y = Y + (y - area.top);

    area.left = X;
    area.top  = Y;
}























class Arrow: public Entity
{
public:
    Arrow(int x = 0, int y = 0)
        : Entity(x, y)
    {
        sprite.setTexture(*texture_base.getArrow(), true);
        sprite.setOrigin(2, 2);
    }
    void handleBlocksCollisions(Environment& world, int autoJump_height);
    void move(sf::Time elapsedTime);
    bool is_holded;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(sprite);

        sf::RectangleShape rect(sf::Vector2f(area.width, area.height));
        rect.setPosition(area.left, area.top);
        rect.setFillColor(sf::Color::Green);
        //target.draw(rect);

        #ifndef NDEBUG
        Dump();
        #endif // NDEBUG
    }
};



void Arrow::handleBlocksCollisions(Environment& world, int autoJump_height)
{
    if(is_holded)
        return;

    float X = area.left;
    float Y = area.top;
    const int W  = area.width;
    const int H  = area.height;
    int step = world.getBlockWidth() / 2;

    blocked = {};

    if(vy <= 0)
    {
        for(int i = 0; i < W; i += step)
            if(world.isBlock(X + i, Y - 1))
            {
                blocked.up = true;
                break;
            }
    }
    if(vy >= 0)
    {
        for(int i = 0; i < W; i += step)
            if(world.isBlock(X + i, Y + H))
            {
                blocked.down = true;
                break;
            }
    }

    if(vx <= 0)
    {
        for(int i = 0; i < H; i += step)
            if(world.isBlock(X - 1, Y + i))
            {
                blocked.left = true;
                break;
            }
    }
    if(vx >= 0)
    {
        for(int i = 0; i < H; i += step)
            if(world.isBlock(X + W, Y + i))
            {
                blocked.right = true;
                break;
            }
    }

    x = X + (x - area.left);
    y = Y + (y - area.top);

    area.left = X;
    area.top  = Y;
}



void Arrow::move(sf::Time elapsedTime)
{
    if(is_holded)
    {
        sprite.setOrigin(2, 20);
        return;
    }
    sprite.setOrigin(2, 2);

    if(!blocked.down && !blocked.up && !blocked.left && !blocked.right)
    {
        x += elapsedTime.asSeconds() * vx * velocity;
        y += elapsedTime.asSeconds() * vy * 10;

        vy += 0.2;
        if(vx != 0)
            sprite.setRotation(atan(vy/vx) /3.14 * 180 + 90);
        else
            sprite.setRotation(90);
    }
    else
        vx = vy = 0;

    if(vx < 0)
        sprite.setScale(1, -1);
    else if(vx < 0)
        sprite.setScale(1, 1);

    sprite.setPosition(x, y);
    //area = sprite.getGlobalBounds();
    area = sf::FloatRect(x - 2, y - 2, 5 , 5);
}




















class Mob_bear: public Entity
{
public:
    enum {STAND, AIR, STEP_1, STEP_2, ATTACK, ANGRY, N_OF_NAMES};
    sf::Vector2i texture_pos[20];
    sf::Vector2i texture_size;

    void setTexture(int name)
    {
        if(name < 0 || name > N_OF_NAMES)
            return;
        sprite.setTextureRect(sf::IntRect(texture_pos[name], texture_size));
    }

    Mob_bear(int x = 0, int y = 0)
        : Entity(x, y)
    {
        texture_size = sf::Vector2i(35, 23);
        {
            int x = texture_size.x,
                y = texture_size.y;
            texture_pos[STAND]  = sf::Vector2i(0*x, 0*y);
            texture_pos[AIR]    = sf::Vector2i(2*x, 1*y);
            texture_pos[STEP_1] = sf::Vector2i(1*x, 0*y);
            texture_pos[STEP_2] = sf::Vector2i(2*x, 0*y);
            texture_pos[ATTACK] = sf::Vector2i(0*x, 1*y);
            texture_pos[ANGRY]  = sf::Vector2i(1*x, 1*y);
        }

        sprite.setTexture(*texture_base.getBear(), true);

        setTexture(STAND);
        sprite.setOrigin(texture_size.x / 2, texture_size.y / 2);
    }
    void move(sf::Time elapsedTime);

    bool angry;
    bool attack;
};


void Mob_bear::move(sf::Time elapsedTime)
{
    static int step = 0;
    tick += elapsedTime.asMilliseconds();

    if(!((blocked.left  && vx < 0) ||
         (blocked.right && vx > 0)))
        x += elapsedTime.asSeconds() * vx * velocity;
    else vx = 0;

    if(!((blocked.up   && vy < 0) ||
         (blocked.down && vy > 0)))
        y += elapsedTime.asSeconds() * vy * 10;
    else vy = 0;

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
        setTexture(STAND);
    }

    if(attack)
        setTexture(ATTACK);
    else if(angry)
        setTexture(ANGRY);

    if(vx < 0)
        sprite.setScale(1, 1);
    else if(vx > 0)
        sprite.setScale(-1, 1);

    sprite.setPosition(x, y);
    area = sprite.getGlobalBounds();
}










class Mob_impala: public Entity
{
public:

    enum {STAND, AIR, STEP_1, STEP_2, EAT, N_OF_NAMES};
    sf::Vector2i texture_pos[20];
    sf::Vector2i texture_size;

    void setTexture(int name)
    {
        if(name < 0 || name > N_OF_NAMES)
            return;
        sprite.setTextureRect(sf::IntRect(texture_pos[name], texture_size));
    }

    Mob_impala(int x = 0, int y = 0)
        : Entity(x, y)
    {
        texture_size = sf::Vector2i(22, 28);
        {
            int x = texture_size.x,
                y = texture_size.y;
            texture_pos[STAND]  = sf::Vector2i(0*x, 0*y);
            texture_pos[AIR]    = sf::Vector2i(1*x, 0*y);
            texture_pos[STEP_1] = sf::Vector2i(0*x, 1*y);
            texture_pos[STEP_2] = sf::Vector2i(1*x, 1*y);
            texture_pos[EAT]    = sf::Vector2i(2*x, 0*y);
        }

        sprite.setTexture(*texture_base.getImpala(), true);

        setTexture(STAND);
        sprite.setOrigin(texture_size.x / 2, texture_size.y / 2);
    }
    void move(sf::Time elapsedTime);
    bool eating = false;
};


void Mob_impala::move(sf::Time elapsedTime)
{
    static int step = 0;
    tick += elapsedTime.asMilliseconds();

    if(!((blocked.left && vx < 0) || (blocked.right && vx > 0)))
        x += elapsedTime.asSeconds() * vx * velocity;
    else vx = 0;

    if(!((blocked.up   && vy < 0) || (blocked.down  && vy > 0)))
        y += elapsedTime.asSeconds() * vy * 10;
    else vy = 0;

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

            case 2:
                pos = AIR;
                break;

            case 3:
                pos = STEP_2;
                break;

            case 0:
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
        if(tick > 800)
        {
            if(!eating)
            {
                setTexture(EAT);
                eating = true;
            }
            else if(eating)
            {
                setTexture(STAND);
                eating = false;
            }
            tick = 0;
        }
    }

    if(vx < 0)
        sprite.setScale(1, 1);
    else if(vx > 0)
        sprite.setScale(-1, 1);

    sprite.setPosition(x, y);
    area = sprite.getGlobalBounds();
}

#endif // ENTITY_HPP_INCLUDED
