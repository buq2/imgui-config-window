#include "config_window.h"
#include "config.pb.h"

// Main code
int main(int, char**)
{
    ConfigWindow conf;
    conf.Initialize();

    ConfigProto proto;
    conf.Draw(proto);
    return 0;
}
