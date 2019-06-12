#ifndef CREATURE_HPP_INCLUDED
#define CREATURE_HPP_INCLUDED

class Creature: public Entity
{
public:
    Creature()
        : Entity()
        , name("Default_Creature")
        { }

    //--- Debugging tools ---//
    #ifndef NDEBUG
    virtual void Dump() const
    {
        printf("DUMPING \"Creature\" at [%p]:\n", this);

        Entity::Dump();

        puts("// Creature:");

        printf("\tname: \"%s\"\n", name);

        puts("//~Creature");
    }
    #endif // NDEBUG

private:
    char name[NAMELEN];
};

class MovingObject: public Entity
{
public:
    MovingObject()
        : Entity()
        { }

    //--- Debugging tools ---//
    #ifndef NDEBUG
    virtual void Dump() const
    {
        printf("DUMPING \"Moving object\" at [%p]:\n", this);

        Entity::Dump();

        puts("// Moving object:");

        puts("//~Moving object");
    }
    #endif // NDEBUG

private:

};

#endif // CREATURE_HPP_INCLUDED
