#include <AppKit.h>
#include <cstdio>
#include "KeyStoreTestApp.h"

int main(int argc, char** argv)
{
    const char* key = NULL;

    switch(argc) {
        case 2:
        {
            if(strcmp(argv[1], "--help") == 0) {
                printf("%s. Command line usage:\n"
                       "\t--help             Help (this message)\n"
                       "\t--use-key <key>    Use <key> as API key\n",
                       kAppName);
                return 0;
            }
            break;
        }
        case 3:
        {
            if(strcmp(argv[1], "--use-key") == 0)
                key = argv[2];
            break;
        }
        default:
            break;
    }

    KeyStoreTestApp* app = new KeyStoreTestApp(key);
    app->Run();
    delete app;
    return 0;
}

