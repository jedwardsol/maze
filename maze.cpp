#include "grid.h"
#include <random>
#include <stack>
#include <vector>
#include <iostream>

enum class Direction {
    none,
    left,
    right,
    up,
    down,
};

Direction chooseDirection( auto const &grid, Pos const &pos )
{
    static std::mt19937 rng { std::random_device {}() };

    std::vector< Direction > choices;

    if( pos.col != 0 && !grid.at( { pos.row, pos.col - 1 }).visited ) {
        choices.push_back( Direction::left );
    }

    if( pos.col != grid.width() - 1 && !grid.at( { pos.row, pos.col + 1 }).visited ) {
        choices.push_back( Direction::right );
    }

    if( pos.row != 0 && !grid.at( { pos.row - 1, pos.col }).visited ) {
        choices.push_back( Direction::up );
    }

    if( pos.row != grid.height() - 1 && !grid.at( { pos.row + 1, pos.col }).visited ) {
        choices.push_back( Direction::down );
    }

    if( choices.empty() ) {
        return Direction::none;
    } else {
        return choices[ std::uniform_int_distribution< size_t > { 0, choices.size() - 1 }( rng ) ];
    }
}

void mazify( Grid &grid, Algorithm algorithm )
{
    static std::mt19937 rng { std::random_device {}() };

    std::vector<Pos>    fringe;
    Pos                 start{ 0, 0 };

    grid.at(start).visited=true;
    fringe.push_back( start);

    while( !fringe.empty() ) {

        if(algorithm==Algorithm::Prims)
        {
            std::uniform_int_distribution<size_t>   indices{0,fringe.size()-1};
            auto index=indices(rng);
            std::swap(fringe[index],fringe.back());
        }


        Pos cell{ fringe.back()};
        Pos neighbour{ cell};


        auto direction = chooseDirection( grid, cell );

        switch( direction ) {
            case Direction::none:
                fringe.pop_back();
                continue;

            case Direction::right:
                neighbour.col++;
                grid.at( cell).right =true;
                grid.at( neighbour).left =true;
                break;

            case Direction::left:
                neighbour.col--;
                grid.at( cell).left =true;
                grid.at( neighbour).right=true;
                break;

            case Direction::down:
                neighbour.row++;
                grid.at( cell).down=true;
                grid.at( neighbour).up  =true;
                break;

            case Direction::up:
                neighbour.row--;
                grid.at( cell).up  =true;
                grid.at( neighbour).down=true;
                break;
        }

        grid.at( neighbour).visited =true;
        fringe.push_back( neighbour );
    }
}


void removeSomeDeadEnds(Grid &grid,int deadendPercent)
{
    std::mt19937                    rng { std::random_device {}() };
    std::uniform_int_distribution   percent{0,99};

    // left deadends
    for(size_t row=0;row<grid.height();row++)
    {
        for(size_t col=1;col<grid.width();col++)
        {
            if(   grid.junctions({row,col}) == Grid::onlyRight 
               && percent(rng) < deadendPercent)
            {
                grid.at({row,col  }).left =true;
                grid.at({row,col-1}).right=true;
            }
        }
    }

    // right deadends
    for(size_t row=0;row<grid.height();row++)
    {
        for(size_t col=0;col<grid.width()-1;col++)
        {
            if(   grid.junctions({row,col}) == Grid::onlyLeft
               && percent(rng) < deadendPercent)
            {
                grid.at({row,col  }).right=true;
                grid.at({row,col+1}).left =true;
            }
        }
    }

    // up deadends
    for(size_t row=1;row<grid.height();row++)
    {
        for(size_t col=0;col<grid.width();col++)
        {
            if(   grid.junctions({row,col}) == Grid::onlyDown
               && percent(rng) < deadendPercent)
            {
                grid.at({row,  col}).up   =true;
                grid.at({row-1,col}).down =true;
            }
        }
    }

    // down deadends
    for(size_t row=0;row<grid.height()-1;row++)
    {
        for(size_t col=0;col<grid.width();col++)
        {
            if(   grid.junctions({row,col}) == Grid::onlyUp
               && percent(rng) < deadendPercent)
            {
                grid.at({row,  col}).down =true;
                grid.at({row+1,col}).up   =true;
            }
        }
    }
}

