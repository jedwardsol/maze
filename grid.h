#pragma once

#include <cstdint>
#include <vector>


struct Pos
{
    size_t row;
    size_t col;

    friend auto operator<=>(Pos const&,Pos const&)=default;

};



class Grid
{
public:

    union Cell {
        uint8_t u;
        struct {
            uint8_t left : 1;
            uint8_t right : 1;
            uint8_t up : 1;
            uint8_t down : 1;
            uint8_t visited :1;
            uint8_t onPath : 1;
            uint8_t explored : 1;
        };
    };


    Grid( size_t H, size_t W )
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

    uint8_t junctions( Pos const &pos ) const
    {
        return grid[ pos.row * W + pos.col ].u & 0b1111;
    }


private:
    size_t H;
    size_t W;
    std::vector< Cell > grid {};
};
