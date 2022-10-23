#include "grid.h"
#include <array>
#include <chrono>
#include <format>
#include <iostream>
#include <string_view>
#include <windows.h>

namespace ch = std::chrono;
using Clock = ch::steady_clock;

void mazify( Grid &grid, Algorithm algorithm );
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

    std::string buffer;

    buffer+= "\n\n\n" ;

    for( size_t row = 0; row < grid.height(); row++ ) {
        buffer+=  "\n   " ;

        for( size_t col = 0; col < grid.width(); col++ ) {
            auto onPath = grid.at({row,col}).onPath;
            auto explored = grid.at( { row, col }).explored;
            auto index = grid.junctions( { row, col } );
            auto glyph = reinterpret_cast< char const * >( glyphs[ index ] );

            if( onPath ) {
                buffer+= std::format( "{}{}{}", red, glyph, normal );
            }
            else if( explored ) {
                buffer+= std::format( "{}{}{}", blue, glyph, normal );
            } else {
                buffer+= std::format( "{}", glyph );
            }
        }
    }

    buffer+= "\n\n\n";

    std::cout << buffer;
}


struct MazeParams
{
    size_t      height{};
    size_t      width{};
    int         deadendPercent{};
    Algorithm   algorithm;
};

auto getMazeParams(int argc, char *argv[])
{
    std::vector<std::string>  args(argv+1,argv+argc);
    
    MazeParams params;


    for(auto &arg : args)
    {
        if(arg.starts_with("-H="))
        {
            params.height=stoi(arg.substr(3));    
        }

        if(arg.starts_with("-W="))
        {
            params.width=stoi(arg.substr(3));    
        }

        if(arg.starts_with("-D="))
        {
            params.deadendPercent=stoi(arg.substr(3));    
        }

        if(arg.starts_with("-A="))
        {
            params.algorithm=static_cast<Algorithm>(stoi(arg.substr(3)));    
        }
    }

    CONSOLE_SCREEN_BUFFER_INFO screen {};
    GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &screen );

    if(params.height==0)
    {
        params.height = screen.srWindow.Bottom - screen.srWindow.Top - 5;
    }

    if(params.width==0)
    {
        params.width = screen.srWindow.Right - screen.srWindow.Left - 6;
    }

    return params;
}

int main(int argc, char *argv[])
{
    auto conOut = GetStdHandle( STD_OUTPUT_HANDLE );

    SetConsoleOutputCP( CP_UTF8 );
    DWORD mode {};
    GetConsoleMode( conOut, &mode );
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode( conOut, mode );

    auto maze=getMazeParams(argc,argv);

    Grid grid( maze.height , maze.width );

    auto startMake = Clock::now();
    mazify(grid,maze.algorithm );

    if(maze.deadendPercent)
    {
        removeSomeDeadEnds(grid,maze.deadendPercent);
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