#ifndef ENVIRONMENT_HPP_INCLUDED
#define ENVIRONMENT_HPP_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>

#include "TextureBase.hpp"

#define BLOCKSIZE (10)
#define SIZE_W (100)
#define SIZE_H (100)

class Environment: public sf::Drawable
{
public:
    Environment(unsigned int _width, unsigned int _height)
        : sf::Drawable()
        , width     (_width)
        , height    (_height)
        , blockWidth    (BLOCKSIZE)
        , blockHeight   (BLOCKSIZE)
        , blocks        ({})
        , liquids       ({})
        , num_of_liquids(0)
        , tick          (0)
    {
        block_pos[AIR]   = sf::Vector2i(30, 10);
        block_pos[ERROR] = sf::Vector2i(20, 10);
        block_pos[GRASS] = sf::Vector2i(00, 00);
        block_pos[DIRT]  = sf::Vector2i(00, 10);
        block_pos[STONE] = sf::Vector2i(10, 10);

        water_pos[0] = sf::Vector2i(20, 10);
        water_pos[1] = sf::Vector2i(40, 00);
        water_pos[2] = sf::Vector2i(30, 00);
        water_pos[3] = sf::Vector2i(20, 00);
        water_pos[4] = sf::Vector2i(10, 00);
        water_pos[5] = sf::Vector2i(40, 10);




        for(int i = 0; i < 1000; i++)
        {
            liquids[i].x = 0;
            liquids[i].y = 0;
            liquids[i].V = 0;
            liquids[i].dV = 0;
        }

        field.create(SIZE_W * blockWidth, SIZE_H * blockHeight);
        field.clear(sf::Color::Transparent);
        field.display();


        FILE* infile = fopen("map.txt", "r");
        int tmp;
        for(int i = 0; i < SIZE_H; i++)
        {
            for(int j = 0; j < SIZE_W; j++)
            {
                fscanf(infile, "%d", &tmp);

                if(tmp > 0)
                {
                    blocks[i][j].ID = tmp;
                    sf::Sprite tmp_spr(*texture_base.getBlocks(), sf::IntRect(getBlockPlace(tmp), sf::Vector2i(10, 10)));

                    tmp_spr.setPosition(j * blockWidth, i * blockHeight);

                    field.draw(tmp_spr);
                }
                else if(tmp < 0)
                {
                    addLiquid(j, i);
                    liquids[num_of_liquids].V = -tmp;
                }
            }
        }
        field.display();
        fclose(infile);
    }

    ~Environment()
    {
        FILE* outfile = fopen("map.txt", "w");

        for(int i = 0; i < SIZE_H; i++)
        {
            for(int j = 0; j < SIZE_W; j++)
            {
                if(blocks[i][j].ID != AIR)
                    fprintf(outfile, "%3d ", blocks[i][j].ID);
                else
                    fprintf(outfile, "%3d ", -liquids[blocks[i][j].pos].V);
            }
            fprintf(outfile, "\n");
        }

        fclose(outfile);
    }

    enum {AIR, ERROR, GRASS, DIRT, STONE};
    sf::Vector2i block_pos[50] = {};
    sf::Vector2i water_pos[50] = {};

    sf::Vector2i getBlockPlace(int ID);

    bool putBlock(float x, float y, unsigned char ID);

    bool isBlock(float x, float y) const;
    bool isBlock_idx(int x, int y) const;

    bool isLiquid(float x, float y) const;
    bool isLiquid_idx(int x, int y) const;

    void handle_liquids(sf::Time elapsedTime)
    {
        tick += elapsedTime.asMilliseconds();
        if(tick > 100)
        {
            tick -= 100;
        }
        else
            return;


        int c = num_of_liquids;
        for(int i = 1; i <= c; i++)
        {
            bool block_down  = isBlock_idx(liquids[i].x, liquids[i].y + 1),
                 block_left  = isBlock_idx(liquids[i].x - 1, liquids[i].y),
                 block_right = isBlock_idx(liquids[i].x + 1, liquids[i].y);

            bool liquid_down  = isLiquid_idx(liquids[i].x, liquids[i].y + 1),
                 liquid_left  = isLiquid_idx(liquids[i].x - 1, liquids[i].y),
                 liquid_right = isLiquid_idx(liquids[i].x + 1, liquids[i].y);

            int pos_down  = blocks[liquids[i].y + 1][liquids[i].x].pos,
                pos_left  = blocks[liquids[i].y][liquids[i].x - 1].pos,
                pos_right = blocks[liquids[i].y][liquids[i].x + 1].pos;

            if(liquids[i].V > 4)
            {
                if(!isBlock_idx(liquids[i].x, liquids[i].y - 1))
                {
                    int pos_up = blocks[liquids[i].y - 1][liquids[i].x].pos;
                    if(!isLiquid_idx(liquids[i].x, liquids[i].y - 1))
                        pos_up = addLiquid(liquids[i].x, liquids[i].y - 1);
                    liquids[i].dV--;
                    liquids[pos_up].dV++;
                    continue;
                }
            }


            if(!block_down && (!liquid_down || liquids[pos_down].V < 4))
            {
                if(!liquid_down)
                    pos_down = addLiquid(liquids[i].x, liquids[i].y + 1);
                liquids[i].dV--;
                liquids[pos_down].dV++;
                continue;
            }

            if(!block_left && !liquids[i].from_old.left && (!liquid_left || (liquids[pos_left].V < liquids[i].V && liquids[pos_left].V + liquids[pos_left].dV < 4)/* || liquids[pos_left].V == liquids[i].V && liquids[i].from_old.right*/))
            {
                if(!liquid_left)
                    pos_left = addLiquid(liquids[i].x - 1, liquids[i].y);
                liquids[i].dV--;
                liquids[pos_left].dV++;
                liquids[pos_left].from_new.right = true;
            }
            else if(!block_right && !liquids[i].from_old.right && (!liquid_right || (liquids[pos_right].V < liquids[i].V && liquids[pos_right].V + liquids[pos_right].dV < 4)/*|| liquids[pos_right].V == liquids[i].V && liquids[i].from_old.left*/))
            {
                if(!liquid_right)
                    pos_right = addLiquid(liquids[i].x + 1, liquids[i].y);
                liquids[i].dV--;
                liquids[pos_right].dV++;
                liquids[pos_right].from_new.left = true;
            }
        }

        #ifndef NDEBUG
        for(int i = 0; i <= c; i++)
            printf("[%d] %d(%2d), ", i, liquids[i].V, liquids[i].dV);
        printf("\n");
        #endif

        for(int i = 1; i <= num_of_liquids; i++)
        {
            liquids[i].V += liquids[i].dV;
            liquids[i].dV = 0;

            liquids[i].from_old = liquids[i].from_new;
            liquids[i].from_new.left = liquids[i].from_new.right = false;
        }

        #ifndef NDEBUG
        for(int i = 0; i <= num_of_liquids; i++)
            printf("[%d] %d(%2d), ", i, liquids[i].V, liquids[i].dV);
        printf("\n");
        #endif

        for(int i = 1; i <= num_of_liquids; i++)
        {
            if(liquids[i].V == 0)
            {
                blocks[liquids[i].y][liquids[i].x].pos = 0;
                if(i != num_of_liquids)
                {
                    blocks[liquids[num_of_liquids].y][liquids[num_of_liquids].x].pos = i;
                    liquids[i] = liquids[num_of_liquids];
                    i--;
                }
                num_of_liquids--;
            }
        }

        #ifndef NDEBUG
        for(int i = 0; i <= num_of_liquids; i++)
            printf("[%d] %d(%2d), ", i, liquids[i].V, liquids[i].dV);
        printf("\n\t\t=====\n");
        #endif
    }

    int floorX(float x);
    int floorY(float y);

    int ceilX(float x) { return floorX(x) + blockWidth; }
    int ceilY(float y) { return floorY(y) + blockHeight; }

    int getBlockWidth()  { return blockWidth; }
    int getBlockHeight() { return blockHeight; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::Sprite sprite(*texture_base.getBackground());
        target.draw(sprite);

        sprite.setTexture(field.getTexture(), true);
        target.draw(sprite);
    }

    void draw_liquid(sf::RenderTarget& target, sf::RenderStates states = sf::RenderStates::Default) const
    {
        sf::Sprite sprite(*texture_base.getBlocks());
        sf::Vector2i v_size(10, 10);
        for(int i = 1; i <= num_of_liquids; i++)
        {
            if(liquids[i].V <= 4)
                sprite.setTextureRect(sf::IntRect(water_pos[liquids[i].V], v_size));
            else
                sprite.setTextureRect(sf::IntRect(water_pos[5], v_size));

            sprite.setPosition(liquids[i].x * blockWidth, liquids[i].y * blockHeight);
            target.draw(sprite, sf::BlendMultiply);
        }
    }

    int addLiquid(int x, int y);

private:
public:
    sf::RenderTexture   field;

    unsigned int width, height;
    unsigned int blockWidth, blockHeight;
    struct
    {
        unsigned char ID;
        int pos;
    } blocks[SIZE_H][SIZE_W] = {};

    struct
    {
        int x, y;
        int V, dV;
        struct
        {
            bool left, right;
        } from_old, from_new;
    } liquids[100000];

    int num_of_liquids;

    unsigned int tick;
};

int Environment::floorX(float x)
{
    int posx = floor(x);
    return posx - posx % blockWidth;
}

int Environment::floorY(float y)
{
    int posy = floor(y);
    return posy - posy % blockHeight;
}

bool Environment::putBlock(float x, float y, unsigned char ID)
{
    if(x < 0 || y < 0 || x > field.getSize().x || y > field.getSize().y)
        return false;

    int posx = (int) x;
        posx -= posx % blockWidth;
    int posy = (int) y;
        posy -= posy % blockHeight;

    blocks[posy / blockHeight][posx / blockWidth].ID = ID;

    if(ID == 0)
    {
        sf::Sprite tmp_spr(*texture_base.getBackground(), sf::IntRect(posx, posy, 10, 10));
        tmp_spr.setPosition(posx, posy);

        field.draw(tmp_spr);
        field.display();
    }
    else
    {
        //sf::Sprite tmp_spr(block_texture, sf::IntRect(10, 10, 10, 10));
        sf::Sprite tmp_spr(*texture_base.getBlocks(), sf::IntRect(getBlockPlace(ID), sf::Vector2i(10, 10)));
        tmp_spr.setPosition(posx, posy);

        field.draw(tmp_spr);
        field.display();
    }

    return true;
}

bool Environment::isBlock(float x, float y) const
{
    if(x < 0 || y < 0 || x >= field.getSize().x || y >= field.getSize().y)
        return true;

    int posx = floor(x);
        posx -= posx % blockWidth;
    int posy = floor(y);
        posy -= posy % blockHeight;

    return blocks[posy / blockHeight][posx / blockWidth].ID != 0;
}

bool Environment::isBlock_idx(int x, int y) const
{
    if(x < 0 || y < 0 || x >= SIZE_W || y >= SIZE_H)
        return true;
    return blocks[y][x].ID != 0;
}

bool Environment::isLiquid(float x, float y) const
{
    int posx = floor(x);
        posx -= posx % blockWidth;
    int posy = floor(y);
        posy -= posy % blockHeight;

    return blocks[posy / blockHeight][posx / blockWidth].pos != 0;
}

bool Environment::isLiquid_idx(int x, int y) const
{
    return blocks[y][x].pos != 0;
}

sf::Vector2i Environment::getBlockPlace(int ID)
{
    if(ID < 0 || ID > 4)
        return getBlockPlace(1);

    return sf::Vector2i(block_pos[ID].x, block_pos[ID].y);
}

int Environment::addLiquid(int x, int y)
{
    num_of_liquids++;
    liquids[num_of_liquids].x = x;
    liquids[num_of_liquids].y = y;
    liquids[num_of_liquids].V = 0;
    liquids[num_of_liquids].dV = 0;
    liquids[num_of_liquids].from_new.left  = false;
    liquids[num_of_liquids].from_new.right = false;
    liquids[num_of_liquids].from_old.left  = false;
    liquids[num_of_liquids].from_old.right = false;
    blocks[y][x].pos = num_of_liquids;
    return num_of_liquids;
}

#endif // ENVIRONMENT_HPP_INCLUDED
