#include <iostream>
using namespace std;

int main() {
    
    std::string console_nice = "";

    for(int i = 0; i<70; i++) {
        console_nice.append("=");
    }
    std::cout << console_nice;
    std::cout << "\nHello World" << ": \tthis is a very useful lecture and shit and stuff lmfao\n";
    std::cout << console_nice;

    return 0;
}
