#include "grid.h"
#include <random>
#include <stack>
#include <vector>


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

void mazify( Grid &grid )
{
    std::stack< Pos > path;
    Pos walk { 0, 0 };

    grid.at(walk).visited=true;
    path.push( walk );

    while( !path.empty() ) {
        Pos next { walk };
        auto direction = chooseDirection( grid, walk );

        switch( direction ) {
            case Direction::none:
                walk = path.top();
                path.pop();
                continue;

            case Direction::right:
                next.col++;
                grid.at( walk).right =true;
                grid.at( next).left =true;
                break;

            case Direction::left:
                next.col--;
                grid.at( walk).left =true;
                grid.at( next).right=true;
                break;

            case Direction::down:
                next.row++;
                grid.at( walk).down=true;
                grid.at( next).up  =true;
                break;

            case Direction::up:
                next.row--;
                grid.at( walk).up  =true;
                grid.at( next).down=true;
                break;
        }

        walk = next;
        grid.at( walk).visited =true;
        path.push( walk );
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
            if(   grid.junctions({row,col}) == 0b0010   // only right set
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
            if(   grid.junctions({row,col}) == 0b0001   // only left set
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
            if(   grid.junctions({row,col}) == 0b1000   // only down set
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
            if(   grid.junctions({row,col}) == 0b0100   // only up set
               && percent(rng) < deadendPercent)
            {
                grid.at({row,  col}).down =true;
                grid.at({row+1,col}).up   =true;
            }
        }
    }



}

