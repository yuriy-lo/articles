#include <iostream>
#include <string>

int main(int argc, char** argv) {
    std::cout << "args_reflect v0.1\n";
    std::cout << "Received " << argc << " arguments:\n";
    for (int i = 0; i < argc; ++i) {
        std::cout << "  arg[" << i << "] = " << argv[i] << "\n";
    }
    return 0;
}
