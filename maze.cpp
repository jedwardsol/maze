#include <stack>
#include <tuple>
#include <array>
#include <random>
#include <iostream>
#include <windows.h>

enum class Cell : uint8_t
{
    left =1, 
    right=2, 
    up   =4, 
    down =8, 
    visited=16
};


std::array<char8_t const *,16> glyphs=
{
    u8" ",      //  0000
    u8"╡",      //  0001
    u8"╞",      //  0010
    u8"═",      //  0011
    u8"╨",      //  0100
    u8"╝",      //  0101
    u8"╚",      //  0110
    u8"╩",      //  0111
    u8"╥",      //  1000
    u8"╗",      //  1001
    u8"╔",      //  1010
    u8"╦",      //  1011
    u8"║",      //  1100
    u8"╣",      //  1101
    u8"╠",      //  1110
    u8"╬",      //  1111
};


enum class Direction : uint8_t
{
    none,
    left,
    right,
    up,
    down,
};


Cell &operator|=(Cell &lhs, Cell const  &rhs)
{
    lhs = static_cast<Cell>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
    return lhs;        
}

auto operator&(Cell const &lhs, Cell const &rhs)
{
    return (static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}


struct Pos
{
    int row;
    int col;
};

class Grid
{
public:

    Grid(size_t H, size_t W) : H{H}, W{W}
    {
        grid.resize(H*W);
    }

    auto height() const
    {
        return H;
    }

    auto width() const
    {
        return W;
    }

    auto numVisited() const
    {
        return numVisitedCells;
    }
    
    bool visited(Pos const& pos) const
    {
        return (at(pos) & Cell::visited);
    }

    Cell const&at(Pos const& pos) const
    {
        return grid[pos.row * W + pos.col];
    }

    Cell& at(Pos const& pos)
    {
        return grid[pos.row * W + pos.col];
    }

    void visit(Pos const &pos)
    {
        if(!visited(pos))
        {
            at(pos) |= Cell::visited;
            numVisitedCells++;
        }
    }

private:
    size_t                  H;
    size_t                  W;
    int                     numVisitedCells{};
    std::vector<Cell>       grid{};
};


Direction chooseDirection(auto const &grid, Pos const &pos)
{
    static    std::mt19937            rng{ std::random_device{}() };

    std::vector<Direction> choices;

    if(    pos.col != 0
        && !grid.visited({pos.row,pos.col-1}))
    {
        choices.push_back(Direction::left);
    }

    if(   pos.row != 0
       && !grid.visited({pos.row-1, pos.col}))
    {
        choices.push_back(Direction::up);
    }

    if( pos.col != grid.width()-1
        && !grid.visited({pos.row, pos.col+1}))
    {
        choices.push_back(Direction::right);
    }

    if (pos.row != grid.height() - 1
        && !grid.visited({pos.row+1, pos.col}))
    {
        choices.push_back(Direction::down);
    }

    if (choices.empty())
    {
        return Direction::none;
    }
    else
    {
        return choices[ std::uniform_int_distribution<size_t>{0,choices.size()-1}(rng)];
    }
}


int main()
{
    SetConsoleOutputCP(CP_UTF8);
    CONSOLE_SCREEN_BUFFER_INFO  screen{};
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&screen);

    auto W = screen.srWindow.Right  - screen.srWindow.Left;
    auto H = screen.srWindow.Bottom - screen.srWindow.Top;

    Grid            grid(H-10,W-10);
    std::stack<Pos> path;
    Pos             walk{0,0};        

    grid.visit(walk);
    path.push(walk);

    while(grid.numVisited() < grid.height()*grid.width())
    {
        Pos next{walk};
        auto direction = chooseDirection(grid,walk);

        switch(direction)
        {
        case Direction::none:
            next=path.top();
            path.pop();
            break;

        case Direction::right:
            next.col++;
            grid.at(walk) |= Cell::right;
            grid.at(next) |= Cell::left;
            path.push(next);
            break;

        case Direction::left:
            next.col--;
            grid.at(walk) |= Cell::left;
            grid.at(next) |= Cell::right;
            path.push(next);
            break;

        case Direction::down:
            next.row++;
            grid.at(walk) |= Cell::down;
            grid.at(next) |= Cell::up;
            path.push(next);
            break;

        case Direction::up:
            next.row--;
            grid.at(walk) |= Cell::up;
            grid.at(next) |= Cell::down;
            path.push(next);
            break;
        }

        grid.visit(next);
        walk = next;
    }

    for(int row=0;row<grid.height();row++)
    {
        for (int col = 0; col < grid.width(); col++)
        {
            std::cout << reinterpret_cast<char const*>(glyphs[ grid.at({row,col}) & Cell{0b1111}]);
        }
        std::cout << "\n";
    }
}