#include "zipper.h"
#include <string>

int main(){
    std::string file_path = "../../data/trival.epub";
    Jepub::Zipper zip;
    zip.extractToFile(file_path);
    return 0;
}