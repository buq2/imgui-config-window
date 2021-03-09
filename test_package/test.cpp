#include "config_window.h"
#include "config.pb.h"

// Main code
int main(int argc, char**)
{
    ConfigWindow conf;
    conf.Initialize();

    ConfigProto proto;
    conf.Draw(proto);

    if (argc > 1) {
        while(true) {
            conf.Draw(proto);
        }
    }
    return 0;
}
