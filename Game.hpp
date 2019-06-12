#ifndef GAME_HPP_INCLUDED
#define GAME_HPP_INCLUDED

#include "Player.hpp"
#include "Environment.hpp"


#define INVENTORY_SIZE (6)

class Menu: public sf::Drawable
{
public:
    Menu()
    {
        icon_pos[BACK]    = sf::Vector2i(64, 00);
        icon_pos[NONE]    = sf::Vector2i(00, 00);
        icon_pos[CHOSEN]  = sf::Vector2i(32, 00);

        icon_pos[PICKAXE] = sf::Vector2i(00, 64);
        icon_pos[BOW]     = sf::Vector2i(32, 64);

        icon_pos[EARTH]   = sf::Vector2i(00, 32);
        icon_pos[DIRT]    = sf::Vector2i(32, 32);
        icon_pos[STONE]   = sf::Vector2i(64, 32);
        icon_pos[WATER]   = sf::Vector2i(64, 64);
    }

    enum {BACK, NONE, CHOSEN, PICKAXE, BOW, EARTH, DIRT, STONE, WATER};

    sf::Vector2i icon_pos[50] = {};

    int chosen = 0;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::Sprite icon(*texture_base.getIcons());
        sf::Vector2i v_size(32, 32);

        icon.setTextureRect(sf::IntRect(icon_pos[BACK], v_size));
        icon.setPosition(0, 5);
        for(int i = 0; i < INVENTORY_SIZE; i++)
        {
            target.draw(icon, sf::BlendMultiply);
            icon.move(40, 0);
        }

        icon.setTextureRect(sf::IntRect(icon_pos[NONE], v_size));
        icon.setPosition(0, 5);
        for(int i = 0; i < chosen; i++)
        {
            target.draw(icon);
            icon.move(40, 0);
        }
        icon.move(40, 0);
        for(int i = chosen + 1; i < INVENTORY_SIZE; i++)
        {
            target.draw(icon);
            icon.move(40, 0);
        }

        icon.setTextureRect(sf::IntRect(icon_pos[CHOSEN], v_size));
        icon.setPosition(40 * chosen, 5);
        target.draw(icon);

        icon.setPosition(0, 5);
        icon.setTextureRect(sf::IntRect(icon_pos[PICKAXE], v_size));
        target.draw(icon);

        icon.move(40, 0);
        icon.setTextureRect(sf::IntRect(icon_pos[BOW], v_size));
        target.draw(icon);

        icon.move(40, 0);
        icon.setTextureRect(sf::IntRect(icon_pos[EARTH], v_size));
        target.draw(icon);

        icon.move(40, 0);
        icon.setTextureRect(sf::IntRect(icon_pos[DIRT], v_size));
        target.draw(icon);

        icon.move(40, 0);
        icon.setTextureRect(sf::IntRect(icon_pos[STONE], v_size));
        target.draw(icon);

        icon.move(40, 0);
        icon.setTextureRect(sf::IntRect(icon_pos[WATER], v_size));
        target.draw(icon);

        icon.move(40, 0);
        icon.setTexture(*texture_base.getButton(TextureBase::PAUSE), true);
        target.draw(icon);
    }
};





























#define MAX_ANIMALS (1)
#define MAX_ARROWS  (20)

class Game
{
public:
    Game(sf::RenderWindow* newWindow)
        : mWindow(newWindow)
        , menu()
        , mWorld(_Window_Width, _Window_Height)
        , mPlayer()
        , game_running(false)
        , last_targeting(false)
        , active_bears(0)
        , active_impalas(0)
        , active_arrows(0)
    {
        for(int i = 0; i < MAX_ARROWS; i++)
            arrows[i] = Arrow(0, 0);

        for(int i = 0; i < active_bears; i++)
        {
            bears[i] = Mob_bear(i * 100 + 50, 100);
            bears[i].velocity = i * 2 + 1;
        }
        for(int i = 0; i < active_impalas; i++)
        {
            impalas[i] = Mob_impala(i * 100 + 50, 100);
            impalas[i].velocity = i * 2 + 1;
        }
    }
    void run();

private:
    sf::RenderWindow*   mWindow;

    Environment mWorld;
    Player      mPlayer;
    bool last_targeting;

    int active_bears;
    Mob_bear   bears[MAX_ANIMALS];
    int active_impalas;
    Mob_impala impalas[MAX_ANIMALS];
    int active_arrows;
    Arrow      arrows[MAX_ARROWS];

    Menu menu;

    bool game_running;

    void processEvents();
    void handleKeyboard(sf::Keyboard::Key key, bool isPressed);
    void handleClick(int x, int y, bool isPressed);
    void update(sf::Time deltaTime);
    void render();
};

void Game::run()
{
    game_running = 1;

    sf::Time TimePerFrame = sf::seconds(1.f / 500.f);

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    //--- Game loop ---//
    while(mWindow->isOpen() && game_running)
    {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        //--- Processing and updating as many times as possible //
        // because there in no point in rendering too often ---//
        while(timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;
            processEvents();
            update(TimePerFrame);
        }
        render();
    }
    return;
}

void Game::processEvents()
{
    static bool pressed = false;
    sf::Event event;
    //--- Processing all the events in the queue ---//
    while(mWindow->pollEvent(event))
    {
        switch(event.type)
        {
        case sf::Event::KeyPressed:
            handleKeyboard(event.key.code, true);
            break;
        case sf::Event::KeyReleased:
            handleKeyboard(event.key.code, false);
            break;

        case sf::Event::MouseButtonPressed:
            handleClick(event.mouseButton.x, event.mouseButton.y, true);
            pressed = true;
            break;
        case sf::Event::MouseButtonReleased:
            handleClick(event.mouseButton.x, event.mouseButton.y, false);
            pressed = false;
            break;
        case sf::Event::MouseMoved:
            if(!pressed)
                break;
            handleClick(event.mouseMove.x, event.mouseMove.y, true);
            break;

        case sf::Event::Closed:
            mWindow->close();
            break;

        default:
            break;
        }
    }
}

void Game::handleKeyboard(sf::Keyboard::Key key, bool isPressed)
{
    if(last_targeting)
    {
        mPlayer.targeting = 0;
        last_targeting = false;
        active_arrows--;
    }

    switch(key)
    {
    case sf::Keyboard::Num1:
    case sf::Keyboard::Num2:
    case sf::Keyboard::Num3:
    case sf::Keyboard::Num4:
    case sf::Keyboard::Num5:
    case sf::Keyboard::Num6:
        menu.chosen = key - sf::Keyboard::Num1;
        break;

    case sf::Keyboard::W:
    case sf::Keyboard::A:
    case sf::Keyboard::S:
    case sf::Keyboard::D:
    case sf::Keyboard::Space:
        mPlayer.handleKeyboard(key, isPressed);
        break;

    case sf::Keyboard::Escape:
        game_running = 0;
        return;
        break;

    default:
        break;
    }
}

void Game::handleClick(int x, int y, bool isPressed)
{
    if(!last_targeting &&
       x < 40 * 6 + 90 && y < 40)
    {
        if(x < 40 * 6)
            menu.chosen = x / 40;
        else
            game_running = false;
        return;
    }

    if(mPlayer.health <= 0)
        return;

    if(last_targeting && !mPlayer.blocked.down)
    {
        mPlayer.targeting = 0;
        last_targeting = false;
        active_arrows--;
    }

    sf::Vector2i pixelPos = sf::Mouse::getPosition(*mWindow);
    sf::Vector2f worldPos = mWindow->mapPixelToCoords(pixelPos);

    x = worldPos.x;
    y = worldPos.y;

    if(mWorld.isBlock(x, y) && menu.chosen == 0 && isPressed)
        mWorld.putBlock(x, y, 0);

    if(mPlayer.blocked.down && mPlayer.vx == 0 && menu.chosen == 1 && isPressed)
    {
        if(!last_targeting)
        {
            if(active_arrows == MAX_ARROWS)
            {
                for(int i = 0; i < MAX_ARROWS; i++)
                {
                    arrows[i] = arrows[i + 1];
                }
            }
            else
                active_arrows++;
        }

        arrows[active_arrows - 1] = Arrow(mPlayer.x, mPlayer.y);
        arrows[active_arrows - 1].sprite.setPosition(mPlayer.x, mPlayer.y);

        float vx = (x - mPlayer.x);
        float vy = (y - mPlayer.y);

        if(vx != 0)
        {
            arrows[active_arrows - 1].sprite.setRotation(atan(vy/vx) /3.14 * 180 + 90);
            mPlayer.angle = atan(vy/vx) / 3.14 * 180 + 90;
        }
        else
        {
            arrows[active_arrows - 1].sprite.setRotation(0);
            mPlayer.angle = 0;
        }

        if(vx < 0)
        {
            arrows[active_arrows - 1].sprite.setScale(1, -1);
            mPlayer.angle = -mPlayer.angle;
        }
        else
            arrows[active_arrows - 1].sprite.setScale(1, 1);

        arrows[active_arrows - 1].is_holded = true;
        mPlayer.targeting = 1;
        last_targeting = true;
    }
    if(mPlayer.blocked.down && last_targeting && menu.chosen == 1 && !isPressed)
    {
        arrows[active_arrows - 1] = Arrow(mPlayer.x, mPlayer.y);

        arrows[active_arrows - 1].vx = (x - mPlayer.x);
        arrows[active_arrows - 1].vy = (y - mPlayer.y) / 3;
        arrows[active_arrows - 1].blocked = {};

        arrows[active_arrows - 1].is_holded = false;

        arrows[active_arrows - 1].move(sf::Time());
        mPlayer.targeting = 0;
        last_targeting = false;
    }


    if(mPlayer.area.contains(x, y))
        return;
    for(int i = 0; i < active_bears; i++)
        if(bears[i].area.contains(x, y))
            return;
    for(int i = 0; i < active_impalas; i++)
        if(impalas[i].area.contains(x, y))
            return;
    for(int i = 0; i < active_arrows; i++)
        if(arrows[i].area.contains(x, y))
            return;
    if(mWorld.isBlock(x, y) || mWorld.isLiquid(x, y))
        return;
    if(!isPressed)
        return;

    switch(menu.chosen)
    {
    case 2:
    case 3:
    case 4:
        mWorld.putBlock(x, y, menu.chosen);
        break;

    case 5:
        mWorld.addLiquid(mWorld.floorX(x) / mWorld.blockWidth, mWorld.floorY(y) / mWorld.blockHeight);
        mWorld.liquids[mWorld.num_of_liquids].V = 4;
        break;

    default:
        break;
    }
}

void Game::update(sf::Time elapsedTime)
{
    static int in_a_row = 0;
    in_a_row++;

    if(in_a_row == 1000)
    {
        if(active_bears < MAX_ANIMALS)
        {
            bears[active_bears] = Mob_bear(100, 100);
            active_bears++;
        }
        else if(active_impalas < MAX_ANIMALS)
        {
            impalas[active_impalas] = Mob_impala(300, 100);
            active_impalas++;

        }
        in_a_row = 0;
    }

    mWorld.handle_liquids(elapsedTime);

    for(int i = 0; i < active_bears; i++)
    {
        if(bears[i].health < MAX_HEALTH / 2)
        {
            bears[i].angry = bears[i].attack = false;

            if(mPlayer.x < bears[i].x)
            {
                if(mPlayer.x >= bears[i].x - 100)
                {
                    bears[i].handleKeyboard(sf::Keyboard::D, true);
                    bears[i].handleKeyboard(sf::Keyboard::A, false);
                }
                else
                {
                    bears[i].handleKeyboard(sf::Keyboard::D, false);
                    bears[i].handleKeyboard(sf::Keyboard::A, false);
                }
            }
            else
            {
                if(mPlayer.x <= bears[i].x + 100)
                {
                    bears[i].handleKeyboard(sf::Keyboard::D, false);
                    bears[i].handleKeyboard(sf::Keyboard::A, true);
                }
                else
                {
                    bears[i].handleKeyboard(sf::Keyboard::D, false);
                    bears[i].handleKeyboard(sf::Keyboard::A, false);
                }
            }
        }
        else
        {
            bears[i].angry = true;

            if(mPlayer.area.left + mPlayer.area.width <= bears[i].area.left)
            {
                bears[i].handleKeyboard(sf::Keyboard::A, true);
                bears[i].handleKeyboard(sf::Keyboard::D, false);
            }

            if(mPlayer.area.left >= bears[i].area.left + bears[i].area.width)
            {
                bears[i].handleKeyboard(sf::Keyboard::D, true);
                bears[i].handleKeyboard(sf::Keyboard::A, false);
            }

            if(mPlayer.area.contains(bears[i].x, bears[i].y))
            {
                bears[i].attack = true;
                mPlayer.health--;
            }
            else
            {
                bears[i].attack = false;
            }
        }

        bears[i].area = bears[i].sprite.getGlobalBounds();
    }

    for(int i = 0; i < active_impalas; i++)
    {
        if(mPlayer.x < impalas[i].x)
        {
            if(mPlayer.x >= impalas[i].x - 50)
            {
                impalas[i].handleKeyboard(sf::Keyboard::D, true);
                impalas[i].handleKeyboard(sf::Keyboard::A, false);
            }
            else
            {
                impalas[i].handleKeyboard(sf::Keyboard::D, false);
                impalas[i].handleKeyboard(sf::Keyboard::A, false);
            }
        }
        else
        {
            if(mPlayer.x <= impalas[i].x + 50)
            {
                impalas[i].handleKeyboard(sf::Keyboard::D, false);
                impalas[i].handleKeyboard(sf::Keyboard::A, true);
            }
            else
            {
                impalas[i].handleKeyboard(sf::Keyboard::D, false);
                impalas[i].handleKeyboard(sf::Keyboard::A, false);
            }
        }

        impalas[i].area = impalas[i].sprite.getGlobalBounds();
    }

    for(int i = 0; i < active_arrows; i++)
    {
        if(arrows[i].is_holded)
            continue;

        if(arrows[i].vx == 0 && arrows[i].vy == 0)
        {
            if(!arrows[i].sprite.getGlobalBounds().intersects(mPlayer.area))
                continue;

            if(i != active_arrows - 1)
            {
                arrows[i] = arrows[active_arrows - 1];
                i--;
            }
            active_arrows--;

            continue;
        }

        bool hit = false;

        for(int k = 0; k < active_bears; k++)
            if(arrows[i].area.intersects(bears[k].area))
            {
                bears[k].health -= 50;
                hit = true;

                if(i != active_arrows - 1)
                {
                    arrows[i] = arrows[active_arrows - 1];
                    i--;
                }
                active_arrows--;

                if(bears[k].health <= 0)
                {
                    if(k != active_bears - 1)
                    {
                        bears[k] = bears[active_bears - 1];
                        k--;
                    }
                    active_bears--;
                }

                break;
            }

        if(hit)
            continue;

        for(int k = 0; k < active_impalas; k++)
            if(arrows[i].area.intersects(impalas[k].area))
            {
                impalas[k].health -= 100;
                hit = true;

                if(i != active_arrows - 1)
                {
                    arrows[i] = arrows[active_arrows - 1];
                    i--;
                }
                active_arrows--;

                if(impalas[k].health <= 0)
                {
                    if(k != active_impalas - 1)
                    {
                        impalas[k] = impalas[active_impalas - 1];
                        k--;
                    }
                    active_impalas--;
                }

                break;
            }
    }


    mPlayer.handleBlocksCollisions(mWorld, 1);
    for(int i = 0; i < active_bears; i++)
        bears[i].handleBlocksCollisions(mWorld, 2);
    for(int i = 0; i < active_impalas; i++)
        impalas[i].handleBlocksCollisions(mWorld, 2);

    for(int i = 0; i < active_arrows; i++)
        arrows[i].handleBlocksCollisions(mWorld, 0);


    mPlayer.move(elapsedTime);
    for(int i = 0; i < active_bears; i++)
        bears[i].move(elapsedTime);
    for(int i = 0; i < active_impalas; i++)
        impalas[i].move(elapsedTime);

    for(int i = 0; i < active_arrows; i++)
        arrows[i].move(elapsedTime);
}

void Game::render()
{
    mWindow->clear();

    //int sx = 640, sy = 480;
    int sx = _Window_Width / 2, sy = _Window_Height / 2;

    float cx = mPlayer.x - sx/2, cy = mPlayer.y - sy/2;
    if(mPlayer.x < sx/2)
        cx = 0;
    if(mPlayer.y < sy/2)
        cy = 0;
    if(mPlayer.x > mWorld.field.getSize().x - sx/2)
        cx = mWorld.field.getSize().x - sx;
    if(mPlayer.y > mWorld.field.getSize().y - sy/2)
        cy = mWorld.field.getSize().y - sy;

    sf::View view(sf::FloatRect(cx, cy, sx, sy));
    mWindow->setView(view);

    mWindow->draw(mWorld);

    for(int i = 0; i < active_bears; i++)
        mWindow->draw(bears[i]);
    for(int i = 0; i < active_impalas; i++)
        mWindow->draw(impalas[i]);

    mWindow->draw(mPlayer);

    for(int i = 0; i < active_arrows; i++)
        mWindow->draw(arrows[i]);

    mWorld.draw_liquid(*mWindow, sf::BlendMultiply);

    mWindow->setView(mWindow->getDefaultView());
    mWindow->draw(menu);
    mWindow->setView(view);

    mWindow->display();
}



















class Button: public sf::Drawable, public sf::Transformable
{
public:
    Button(int L, int T, int name)
        : texture(texture_base.getButton(name))
        , rect(L - texture->getSize().x / 2, T - texture->getSize().y / 2, texture->getSize().x, texture->getSize().y)
        , is_pressed(false)
    {

    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::Sprite sprite(*texture);
        sprite.setPosition(rect.left, rect.top);
        target.draw(sprite);
    }

    bool contains(int x, int y)
    {
        return rect.contains(x, y);
    }

    bool is_pressed;

private:
    sf::Texture* texture;
    sf::IntRect rect;
};



class Cursor: public sf::Drawable, public sf::Transformable
{
public:
    Cursor()
        : sf::Transformable()
        , texture(texture_base.getCursor())
    {

    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::Sprite sprite(*texture);
        sprite.setPosition(getPosition());
        sprite.setOrigin(texture->getSize().x / 2, texture->getSize().y / 2);
        sprite.setRotation(getRotation());
        sprite.setScale(getScale());

        target.draw(sprite);
    }

private:
    sf::Texture* texture;
};




class App
{
public:
    App()
        : mWindow(sf::VideoMode(_Window_Width, _Window_Height), "SFML Application"/*, sf::Style::Fullscreen*/)
        //mWindow(sf::VideoMode(s))
        , game(&mWindow)
        , cursor()
        , b_play(mWindow.getSize().x / 2, mWindow.getSize().y / 2 - 50, TextureBase::PLAY)
        , b_quit(mWindow.getSize().x / 2, mWindow.getSize().y / 2 + 50, TextureBase::QUIT)
    {

    }
    void run();

private:
    sf::RenderWindow mWindow;

    Button b_play, b_quit;
    Cursor cursor;

    public:Game game;

    void processEvents();
    void handleKeyboard(sf::Keyboard::Key key, bool isPressed);
    void handleClick(int x, int y, bool isPressed);
    void update(sf::Time deltaTime);
    void render();
};

void App::run()
{
    sf::Time TimePerFrame = sf::seconds(1.f / 60.f);

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    //--- Game loop ---//
    while(mWindow.isOpen())
    {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        //--- Processing and updating as many times as possible //
        // because there in no point in rendering too often ---//
        while(timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;
            processEvents();
            update(TimePerFrame);
        }
        render();
    }
    return;
}

void App::processEvents()
{
    static bool pressed = false;
    sf::Event event;
    //--- Processing all the events in the queue ---//
    while(mWindow.pollEvent(event))
    {
        switch(event.type)
        {
        case sf::Event::KeyPressed:
            handleKeyboard(event.key.code, true);
            break;
        case sf::Event::KeyReleased:
            handleKeyboard(event.key.code, false);
            break;

        case sf::Event::MouseButtonPressed:
            handleClick(event.mouseButton.x, event.mouseButton.y, true);
            pressed = true;
            break;
        case sf::Event::MouseButtonReleased:
            handleClick(event.mouseButton.x, event.mouseButton.y, false);
            pressed = false;
            break;
        case sf::Event::MouseMoved:
            cursor.setPosition(event.mouseMove.x, event.mouseMove.y);
            break;

        case sf::Event::Closed:
            mWindow.close();
            break;

        default:
            break;
        }
    }
}

void App::handleKeyboard(sf::Keyboard::Key key, bool isPressed)
{
    switch(key)
    {
    case sf::Keyboard::Q:
        mWindow.close();
        break;

    default:
        break;
    }
}

void App::handleClick(int x, int y, bool isPressed)
{
    cursor.setPosition(x, y);

    if(b_play.contains(x, y))
    {
        b_play.is_pressed = isPressed;
        if(isPressed == false)
            game.run();
    }
    else
        b_play.is_pressed = false;

    if(b_quit.contains(x, y))
    {
        b_quit.is_pressed = isPressed;
        if(isPressed == false)
            mWindow.close();
    }
    else
        b_quit.is_pressed = false;
}

void App::update(sf::Time elapsedTime)
{
    cursor.rotate(3.f);
    cursor.setScale((sin(3.14 * cursor.getRotation() / 360) + 1), (sin(3.14 * cursor.getRotation() / 360) + 1));
}

void App::render()
{
    mWindow.clear();

    mWindow.setView(mWindow.getDefaultView());
    mWindow.draw(b_play);
    mWindow.draw(b_quit);
    mWindow.draw(cursor);

    mWindow.display();
}

#endif // GAME_HPP_INCLUDED
