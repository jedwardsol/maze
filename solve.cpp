#include "grid.h"
#include <queue>
#include <random>
#include <vector>

struct Candidate
{
    int distance;   // from beginning
    Pos pos;

    auto operator<( Candidate const &rhs ) const noexcept
    {
        return rhs.distance < distance;
    }
};

class GridMap
{
public:
    struct Cell
    {
        bool visited { false };
        int distance { std::numeric_limits< int >::max() };
        Pos previous;
    };

    GridMap( size_t H, size_t W )
            : H { H }
            , W { W }
    {
        grid.resize( H * W );
    }

    auto height() const
    {
        return H;
    }

    auto width() const
    {
        return W;
    }

    Cell const &at( Pos const &pos ) const
    {
        return grid[ pos.row * W + pos.col ];
    }

    Cell &at( Pos const &pos )
    {
        return grid[ pos.row * W + pos.col ];
    }

private:
    size_t H;
    size_t W;
    std::vector< Cell > grid {};
};

std::vector< Pos > getNeighbours( Grid const &grid, Pos pos )
{
    std::vector< Pos > neighbours;

    if( grid.isSet( pos, Grid::Cell::left ) ) {
        neighbours.emplace_back( pos.row, pos.col - 1 );
    }

    if( grid.isSet( pos, Grid::Cell::right ) ) {
        neighbours.emplace_back( pos.row, pos.col + 1 );
    }

    if( grid.isSet( pos, Grid::Cell::up ) ) {
        neighbours.emplace_back( pos.row - 1, pos.col );
    }

    if( grid.isSet( pos, Grid::Cell::down ) ) {
        neighbours.emplace_back( pos.row + 1, pos.col );
    }

    return neighbours;
}

int solve( Grid &grid )  // Dijkstra
{
    std::priority_queue< Candidate > fringe;

    GridMap gridmap { grid.height(), grid.width() };
    Pos const exit { static_cast< int >( grid.height() - 1 ), static_cast< int >( grid.width() - 1 ) };

    fringe.push( Candidate { 0, { 0, 0 } } );

    while( !fringe.empty() ) {
        auto const current = fringe.top();
        fringe.pop();

        if( gridmap.at( current.pos ).visited ) {
            continue;
        }

        gridmap.at( current.pos ).visited = true;
        grid.set(current.pos, Grid::Cell::considered);

        if( current.pos == exit ) {
            break;
        }

        auto neighbours = getNeighbours( grid, current.pos );
        auto newDistance = gridmap.at( current.pos ).distance + 1;

        for( auto neighbour : neighbours ) {
            if( newDistance < gridmap.at( neighbour ).distance ) {
                gridmap.at( neighbour ).distance = newDistance;
                gridmap.at( neighbour ).previous = current.pos;

                fringe.push( Candidate { newDistance, neighbour } );
            }
        }
    }

    int  walkLength{};
    auto walk { exit };

    while( walk != Pos { 0, 0 } ) {
        grid.set( walk, Grid::Cell::onPath );

        walk = gridmap.at( walk ).previous;
        walkLength++;
    }
    grid.set( walk, Grid::Cell::onPath );
    walkLength++;


    return walkLength;
}
