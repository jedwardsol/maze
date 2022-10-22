#include "grid.h"
#include <array>
#include <chrono>
#include <format>
#include <iostream>
#include <string_view>
#include <windows.h>

namespace ch = std::chrono;
using Clock = ch::steady_clock;

void mazify( Grid &grid );
void removeSomeDeadEnds(Grid &grid,int deadendPercent);

int  solve( Grid &grid );

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

void print( Grid const &grid )
{
    auto bold   = "\033[1m";
    auto red    = "\033[38;5;196m";     // https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit
    auto blue   = "\033[38;5;159m";
    auto normal = "\033[0m";

    print( "\n\n\n" );

    for( size_t row = 0; row < grid.height(); row++ ) {
        print( "\n   " );

        for( size_t col = 0; col < grid.width(); col++ ) {
            auto onPath = grid.at({row,col}).onPath;
            auto explored = grid.at( { row, col }).explored;
            auto index = grid.junctions( { row, col } );
            auto glyph = reinterpret_cast< char const * >( glyphs[ index ] );

            if( onPath ) {
                print( "{}{}{}", red, glyph, normal );
            }
            else if( explored ) {
                print( "{}{}{}", blue, glyph, normal );
            } else {
                print( "{}", glyph );
            }
        }
    }

    print( "\n\n\n" );
}

int main(int argc, char *argv[])
{
    std::vector<std::string>  args(argv+1,argv+argc);

    auto conOut = GetStdHandle( STD_OUTPUT_HANDLE );

    SetConsoleOutputCP( CP_UTF8 );
    DWORD mode {};
    GetConsoleMode( conOut, &mode );
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode( conOut, mode );

    size_t  H{};
    size_t  W{};
    int     deadendPercent{};

    if(args.size()>=2)
    {
        H = stoi(args[0]);
        W = stoi(args[1]);
    }

    if(args.size()>=3)
    {
        deadendPercent=stoi(args[2]);
    }

    CONSOLE_SCREEN_BUFFER_INFO screen {};
    GetConsoleScreenBufferInfo( conOut, &screen );

    if(H==0)
    {
        H = screen.srWindow.Bottom - screen.srWindow.Top - 5;
    }

    if(W==0)
    {
        W = screen.srWindow.Right - screen.srWindow.Left - 6;
    }

    Grid grid( H , W );

    auto startMake = Clock::now();
    mazify( grid );

    if(deadendPercent)
    {
        removeSomeDeadEnds(grid,deadendPercent);
    }


    auto endMake= Clock::now();

    auto startPrint1 = Clock::now();
    print( grid );
    auto endPrint1 = Clock::now();

    print("Press enter to solve\n");
    std::string  dummy;
    std::getline(std::cin,dummy);


    auto startSolve = Clock::now();
    int pathLength=solve( grid );
    auto endSolve = Clock::now();

    auto startPrint2 = Clock::now();
    print( grid );
    auto endPrint2 = Clock::now();

    print( "{}x{} path={} steps.  create={} solve={} print={} & {}\n",
           grid.height(),
           grid.width(),
           pathLength,
           ch::duration_cast< ch::milliseconds >( endMake  - startMake ),
           ch::duration_cast< ch::milliseconds >( endSolve - startSolve ),
           ch::duration_cast< ch::milliseconds >( endPrint1  - startPrint1 ),
           ch::duration_cast< ch::milliseconds >( endPrint2  - startPrint2 ));
}