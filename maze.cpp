#include <stack>
#include <tuple>
#include <array>
#include <random>

enum class Cell : uint8_t
{
    left =1, 
    right=2, 
    up   =4, 
    down =8, 
    visited=16
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

template <size_t H, size_t W>
class Grid
{
public:
    constexpr auto height() const
    {
        return H;
    }

    constexpr auto width() const
    {
        return W;
    }

    auto numVisited() const
    {
        return numVisitedCells;
    }
    
    bool visited(Pos const& pos) const
    {
        return !(at(pos) & Cell::visited);
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
    int                     numVisitedCells{};
    std::array<Cell,H*W>    grid{};
};


Direction chooseDirection(auto const &grid, Pos const &pos)
{
    static    std::mt19937            rng{ std::random_device{}() };

    std::vector<Direction> choices;

    if(    pos.col != 0
        && !grid.visited(pos.row,pos.col-1))
    {
        choices.push_back(Direction::left);
    }


    if (choices.empty())
    {
        return Direction::none;
    }

}


int main()
{
    Grid<10,10>     grid;
    std::stack<Pos> path;
    Pos             walk{0,0};        


    while(grid.numVisited() < grid.height()*grid.width())
    {
        grid.visit(walk);
        path.push(walk);



    }


}