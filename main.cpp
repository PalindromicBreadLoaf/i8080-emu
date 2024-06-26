#include <iostream>
#include <fstream>
#include <string>
#include <vector>

int dissasemble8080(unsigned char *codebuffer, int pc) {
    unsigned char *code = &codebuffer[pc];
    int opbytes = 1;
    std::cout << "%04x " << pc << std::endl;

    switch (*code)
    {

    }

    return opbytes;
}

int main(int argc, char* argv[])
{
    unsigned char *codebuffer;
    int pc = 0;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        std::cerr << "Could not open file " << filename << std::endl;
        return 1;
    }

    std::streampos fsize = file.tellg();
    file.seekg(0, std::ios::beg);

    codebuffer = new unsigned char[fsize];

    if (!file.read(reinterpret_cast<char*>(codebuffer), fsize))
    {
        std::cerr << "Error: Couldn't read the file " << filename << std::endl;
        delete[] codebuffer;
        return 1;
    }

    while (!file.eof())
    {
        pc = pc + dissasemble8080(codebuffer, pc);
    }

    file.close();

    return 0;
}