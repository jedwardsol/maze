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

    if( pos.col != 0 && !grid.isSet( { pos.row, pos.col - 1 }, Grid::Cell::visited ) ) {
        choices.push_back( Direction::left );
    }

    if( pos.col != grid.width() - 1 && !grid.isSet( { pos.row, pos.col + 1 }, Grid::Cell::visited ) ) {
        choices.push_back( Direction::right );
    }

    if( pos.row != 0 && !grid.isSet( { pos.row - 1, pos.col }, Grid::Cell::visited ) ) {
        choices.push_back( Direction::up );
    }

    if( pos.row != grid.height() - 1 && !grid.isSet( { pos.row + 1, pos.col }, Grid::Cell::visited ) ) {
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

    grid.set( walk, Grid::Cell::visited );
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
                grid.set( walk, Grid::Cell::right );
                grid.set( next, Grid::Cell::left );
                break;

            case Direction::left:
                next.col--;
                grid.set( walk, Grid::Cell::left );
                grid.set( next, Grid::Cell::right );
                break;

            case Direction::down:
                next.row++;
                grid.set( walk, Grid::Cell::down );
                grid.set( next, Grid::Cell::up );
                break;

            case Direction::up:
                next.row--;
                grid.set( walk, Grid::Cell::up );
                grid.set( next, Grid::Cell::down );
                break;
        }

        walk = next;
        grid.set( walk, Grid::Cell::visited );
        path.push( walk );
    }
}
