#ifndef TEXTUREBASE_HPP_INCLUDED
#define TEXTUREBASE_HPP_INCLUDED


class TextureBase
{
public:
    TextureBase()
    {
        if(!t_b_play .loadFromFile("btn_play.png"))  std::terminate();
        if(!t_b_pause.loadFromFile("btn_pause.png")) std::terminate();
        if(!t_b_quit .loadFromFile("btn_quit.png"))  std::terminate();

        if(!t_cursor.loadFromFile("cursor.png"))   std::terminate();

        if(!t_icons.loadFromFile("icons.png"))     std::terminate();
        if(!t_blocks.loadFromFile("blocks.png"))   std::terminate();

        if(!t_background.loadFromFile("back2.png")) std::terminate();

        if(!t_man.loadFromFile("man.png"))         std::terminate();
        if(!t_bear.loadFromFile("bear.png"))       std::terminate();
        if(!t_impala.loadFromFile("impala.png"))   std::terminate();

        if(!t_rect.loadFromFile("yara.png"))       std::terminate();
        if(!t_arrow.loadFromFile("arrow.png"))     std::terminate();
    }

    sf::Texture* getCursor() { return &t_cursor; }

    sf::Texture* getIcons()  { return &t_icons; }
    sf::Texture* getBlocks() { return &t_blocks; }

    sf::Texture* getBackground() { return &t_background; }

    sf::Texture* getMan()    { return &t_man; }
    sf::Texture* getBear()   { return &t_bear; }
    sf::Texture* getImpala() { return &t_impala; }

    sf::Texture* getRect()   { return &t_rect; }
    sf::Texture* getArrow()  { return &t_arrow; }

    enum {PLAY, PAUSE, QUIT};
    sf::Texture* getButton(int name)
    {
        switch(name)
        {
        case PLAY:
            return &t_b_play;
            break;

        case PAUSE:
            return &t_b_pause;
            break;

        case QUIT:
            return &t_b_quit;
            break;

        default:
            return NULL;
            break;
        }
    }

private:
    sf::Texture t_cursor;
    sf::Texture t_icons, t_blocks;
    sf::Texture t_background;
    sf::Texture t_man, t_bear, t_impala;
    sf::Texture t_rect, t_arrow;
    sf::Texture t_b_play, t_b_pause, t_b_quit;
} texture_base;

#endif // TEXTUREBASE_HPP_INCLUDED
