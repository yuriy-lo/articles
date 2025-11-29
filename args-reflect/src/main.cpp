#include "argparse.h"

#include <format>
#include <iostream>
#include <string_view>

int sum(int x, int y)
{
    auto result = x + y;
    std::cout << std::format("{}+{}={}", x, y, result);
    return 0;
}

int sqr(int v)
{
    auto result = v * v;
    std::cout << std::format("{}^2={}", v, result);
    return 0;
}

int cat(std::string_view prefix, std::string_view root, std::string_view suffix)
{
    auto result = std::format("{}{}{}", prefix, root, suffix);
    std::cout << std::format("\"{}\"+\"{}\"+\"{}\"=\"{}\"", prefix, root, suffix, result);
    return 0;
}

int main(int argc, char *argv[])
{
    return argparse::run<sum, sqr, cat>(argc, argv);
}
