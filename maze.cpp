#include <array>
#include <iostream>
#include <random>
#include <stack>
#include <tuple>
#include <windows.h>

enum class Cell : uint8_t {
    left = 1,
    right = 2,
    up = 4,
    down = 8,
    visited = 16
};

std::array< char8_t const *, 16 > glyphs = {
    u8" ",   //  0000
    u8"╡",   //  0001
    u8"╞",   //  0010
    u8"═",   //  0011
    u8"╨",   //  0100
    u8"╝",   //  0101
    u8"╚",   //  0110
    u8"╩",   //  0111
    u8"╥",   //  1000
    u8"╗",   //  1001
    u8"╔",   //  1010
    u8"╦",   //  1011
    u8"║",   //  1100
    u8"╣",   //  1101
    u8"╠",   //  1110
    u8"╬",   //  1111
};

enum class Direction {
    none,
    left,
    right,
    up,
    down,
};

struct Pos
{
    int row;
    int col;
};

class Grid
{
public:
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

Direction chooseDirection( auto const &grid, Pos const &pos )
{
    static std::mt19937 rng { std::random_device {}() };

    std::vector< Direction > choices;

    if( pos.col != 0 && !grid.isSet ( { pos.row, pos.col - 1 }, Cell::visited ) ) {
        choices.push_back( Direction::left );
    }

    if( pos.row != 0 && !grid.isSet( { pos.row - 1, pos.col },Cell::visited ) ) {
        choices.push_back( Direction::up );
    }

    if( pos.col != grid.width() - 1 && !grid.isSet( { pos.row, pos.col + 1 },Cell::visited ) ) {
        choices.push_back( Direction::right );
    }

    if( pos.row != grid.height() - 1 && !grid.isSet( { pos.row + 1, pos.col } ,Cell::visited) ) {
        choices.push_back( Direction::down );
    }

    if( choices.empty() ) {
        return Direction::none;
    } else {
        return choices[ std::uniform_int_distribution< size_t > { 0, choices.size() - 1 }( rng ) ];
    }
}

int main()
{
    SetConsoleOutputCP( CP_UTF8 );
    CONSOLE_SCREEN_BUFFER_INFO screen {};
    GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &screen );

    auto H = screen.srWindow.Bottom - screen.srWindow.Top;
    auto W = screen.srWindow.Right - screen.srWindow.Left;

    Grid grid( H - 5, W - 6 );
    std::stack< Pos > path;
    Pos walk { 0, 0 };

    grid.set( walk, Cell::visited );
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
                grid.set( walk, Cell::right );
                grid.set( next, Cell::left );
                break;

            case Direction::left:
                next.col--;
                grid.set( walk, Cell::left );
                grid.set( next, Cell::right );
                break;

            case Direction::down:
                next.row++;
                grid.set( walk, Cell::down );
                grid.set( next, Cell::up );
                break;

            case Direction::up:
                next.row--;
                grid.set( walk, Cell::up );
                grid.set( next, Cell::down );
                break;
        }

        walk = next;
        grid.set( walk, Cell::visited );
        path.push( walk );
    }

    std::cout << "\n\n\n";

    for( int row = 0; row < grid.height(); row++ ) {

        std::cout << "\n   ";

        for( int col = 0; col < grid.width(); col++ ) {
            auto glyph = static_cast< uint8_t >( grid.at( { row, col } ) ) & 0b1111;

            std::cout << reinterpret_cast< char const * >( glyphs[ glyph ] );
        }
    }

    std::cout << "\n\n\n";
}