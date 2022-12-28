#include "Server/Server.h"
#include "ProgramArgumentParser/lib/ArgParser.h"

int main(int argc, char** argv) {
    ArgumentParser::ArgParser parser("parser");
    parser.AddStringArgument('i', "input");
    parser.Parse(argc, argv);

    std::filesystem::path path(parser.GetStringValue("input"));
    Server server(path);
    server.Work();
    return 0;
}
