import <iostream>;
import <vector>;

import Lateralus.Core;
using namespace Lateralus;

int main()
{
    std::vector<int32> numbers = { 1, 2, 3 };
    for (auto i : numbers)
    {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}