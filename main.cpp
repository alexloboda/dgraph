#include "EulerTourForest.h"
#include <iostream>

int main() {
    int n;
    std::cin >> n;
    dgraph::EulerTourForest forest(n);
    while(true){
        std::string cmd;
        std::cin >> cmd;
        int v, u;
        if(cmd == "link") {
            std::cin >> v >> u;
            forest.link(v, u);
        }
        if(cmd == "cut"){
            std::cin >> v;
            forest.cut(v);
        }
        if(cmd == "conn"){
            std::cin >> v >> u;
            bool is = forest.is_connected(v, u);
            std::cout << (is ? "Yes" : "No") << std::endl;
        }
    }
    return 0;
}