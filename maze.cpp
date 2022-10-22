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
