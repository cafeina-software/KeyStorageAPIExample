#include <AppKit.h>
#include "KeyStoreTestApp.h"

int main(int, char**)
{
    KeyStoreTestApp* app = new KeyStoreTestApp();
    app->Run();
    delete app;
    return 0;
}

