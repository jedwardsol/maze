﻿#include <windows.h>
#include <iostream>
#include <array>
#include <format>
#include <string_view>
#include "grid.h"
#include <chrono>

namespace ch=std::chrono;
using Clock=ch::steady_clock;

void mazify(Grid    &grid);
void solve (Grid    &grid);


template< typename... ARGS >
void print( std::string_view format, ARGS &&...args )
{
    std::ostream_iterator< char > out { std::cout };

    std::vformat_to( out, format, std::make_format_args( args... ) );
}



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


void print(Grid const &grid)
{
    auto bold   = "\033[1m"; 
    auto red    = "\033[38;5;196m"; 
    auto normal = "\033[0m";
 
    print("\n\n\n");

    for( int row = 0; row < grid.height(); row++ ) {

        print("\n   ");

        for( int col = 0; col < grid.width(); col++ ) {

            auto onPath = grid.isSet( { row, col }, Grid::Cell::onPath ) ;
            auto index  = static_cast< uint8_t >( grid.at( { row, col } ) ) & 0b1111;
            auto glyph  = reinterpret_cast< char const * >( glyphs[ index ] ); 

            if(onPath)
            {
                print("{}{}{}",red,glyph,normal);
            }
            else
            {
                print("{}", glyph);
            }
        }
    }

    print("\n\n\n");
}



int main()
{
    auto conOut = GetStdHandle( STD_OUTPUT_HANDLE );

    SetConsoleOutputCP( CP_UTF8 );
    DWORD mode{};
    GetConsoleMode(conOut,&mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING ;
    SetConsoleMode(conOut,mode);

    CONSOLE_SCREEN_BUFFER_INFO screen {};
    GetConsoleScreenBufferInfo( conOut, &screen );

    auto H = screen.srWindow.Bottom - screen.srWindow.Top;
    auto W = screen.srWindow.Right - screen.srWindow.Left;

    Grid grid( H - 5, W - 6 );

    auto start = Clock::now();
    mazify(grid);

    auto made = Clock::now();

    solve(grid);

    auto solved = Clock::now();

    print(grid);

    auto printed = Clock::now();

    print("{}x{} create={} solve={} print={}\n",
    grid.height(), grid.width(),
    ch::duration_cast<ch::milliseconds>(made-start), 
    ch::duration_cast<ch::milliseconds>(solved-made), 
    ch::duration_cast<ch::milliseconds>(printed-solved));
}