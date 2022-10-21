#pragma once

#include <cstdint>
#include <vector>


struct Pos
{
    int row;
    int col;

    friend auto operator<=>(Pos const&,Pos const&)=default;

};



class Grid
{
public:

    enum class Cell : uint8_t {
        left = 1,
        right = 2,
        up = 4,
        down = 8,
        visited = 16,
        onPath = 32,
        considered = 64
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

    void set( Pos const &pos, Cell flag )
    {
        at( pos ) = static_cast< Cell >( static_cast< uint8_t >( at( pos ) ) | static_cast< uint8_t >( flag ) );
    }

    bool isSet( Pos const &pos, Cell flag ) const
    {
        return ( static_cast< uint8_t >( at( pos ) ) & static_cast< uint8_t >( flag ) );
    }


private:
    size_t H;
    size_t W;
    std::vector< Cell > grid {};
};
