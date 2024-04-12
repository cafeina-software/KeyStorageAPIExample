#include "KeyStoreTestWin.h"
#include "KeyStoreTestDefs.h"
#include <Application.h>
#include <TranslationUtils.h>
#include <cstdio>

class KeyStoreTestView : public BView {
public:
    KeyStoreTestView(BRect frame, const char* name, const char* custom)
    : BView(frame, name, B_FOLLOW_ALL_SIDES, B_WILL_DRAW) {
        strcpy(text, custom);
    }
    virtual void Draw(BRect r) {
        BView::Draw(r);
        SetHighColor(0, 0, 0);
        DrawString(text, BPoint(r.Width() / 3 - (float)strlen(text) , r.Height() / 2));
        Invalidate();
    }
private:
    char text[256];
};

KeyStoreTestWin::KeyStoreTestWin(BKey _key)
: BWindow(BRect(50, 50, 640, 480), "KeyStoreTest", B_TITLED_WINDOW, 0),
    keymanager(nullptr), key(_key)
{
    menubar = new BMenuBar("mb_main", B_ITEMS_IN_ROW);
    BLayoutBuilder::Menu<>(menubar)
        .AddMenu(kAppName)
            .AddItem("Manage API keys", M_MANAGE_KEYS, 'M')
            .AddSeparator()
            .AddItem("Quit", M_QUIT, 'Q')
        .End()
        .AddMenu("View")
            .AddItem("Show current key", M_SHOW_CURRENT)
        .End()
    .End();
    menubar->FindItem(M_SHOW_CURRENT)->SetMarked(false);

    // fprintf(stderr, "key is: %s\n", key.Data());
    BView* mainView = new KeyStoreTestView(BRect(0, 0, 400, 100), "vw_main", "This is a test");
    char apidata[256];
    sprintf(apidata, "API Key: %s", reinterpret_cast<const char*>(key.Data()));
    keyView = new KeyStoreTestView(BRect(0, 0, 400, 100), "vw_key", apidata);
    keyView->Hide();

    BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
        .Add(menubar)
        .Add(keyView)
        .Add(mainView)
    .End();

    CenterOnScreen();
}

KeyStoreTestWin::~KeyStoreTestWin()
{
    if(keymanager && keymanager->Lock())
        keymanager->Quit();
}

void KeyStoreTestWin::MessageReceived(BMessage* msg)
{
    switch(msg->what)
    {
        case M_MANAGE_KEYS:
        {
            BKeyStore keystore;
            keymanager = new ManageKeysDialogBox(keystore);
            keymanager->Show();
            break;
        }
        case M_SHOW_CURRENT:
        {
            if(menubar->FindItem(M_SHOW_CURRENT)->IsMarked()) {
                menubar->FindItem(M_SHOW_CURRENT)->SetMarked(false);
                keyView->Hide();
            }
            else {
                menubar->FindItem(M_SHOW_CURRENT)->SetMarked(true);
                keyView->Show();
            }
            break;
        }
        case M_QUIT:
            be_app->PostMessage(B_QUIT_REQUESTED);
            break;
        default:
            BWindow::MessageReceived(msg);
            break;
    }
}
