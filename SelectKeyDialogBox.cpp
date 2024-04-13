#include "SelectKeyDialogBox.h"
#include "KeyStoreTestApp.h"
#include <Key.h>
#include <cstdio>

// struct APIKeyListStruct {
    // BString id;
    // BString key;
// } mfSettings;

SelectKeyDialogBox::SelectKeyDialogBox(BRect frame, const char* name)
: BWindow(frame, name, B_FLOATING_WINDOW_LOOK, B_FLOATING_ALL_WINDOW_FEEL,
    B_NOT_ZOOMABLE|B_NOT_RESIZABLE|B_AUTO_UPDATE_SIZE_LIMITS),
    wantsdefault(false)
{
    BStringView* desc = new BStringView(NULL, "One or more API key were found "
        "but none was selected as the default key. \nPlease select the key you "
        "want to use.");

    keymenu = new BPopUpMenu("Select key");
    keyselection = new BMenuField("mf_selkey", "API Key:", keymenu);

    setAsDefaultCheck = new BCheckBox("cb_def", "Set as default key",
        new BMessage('sdef'));
    setAsDefaultCheck->SetValue(B_CONTROL_OFF);

    selButton = new BButton("bt_selkey", "Select key", new BMessage('selk'));
    selButton->SetEnabled(false);
    quitButton = new BButton("bt_quit", "Quit", new BMessage('quit'));

    BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
        .SetInsets(B_USE_WINDOW_INSETS)
        .Add(desc)
        .Add(keyselection)
        .Add(setAsDefaultCheck)
        .AddStrut(4.0f)
        .AddGroup(B_HORIZONTAL)
            .Add(selButton)
            .Add(quitButton)
        .End()
    .End();

    CenterOnScreen();

    _InitData();
}


void SelectKeyDialogBox::MessageReceived(BMessage* msg)
{
    switch(msg->what)
    {
        case 'this':
        {
            if(msg->FindString("id", &currentkeyid) == B_OK) {
            fprintf(stderr, "id is: %s\n", currentkeyid.String());
                selButton->SetEnabled(true);
            }
            break;
        }
        case 'selk':
        {
            if(!currentkeyid.IsEmpty()) {
                BMessage datmsg(M_USER_SELECTED_KEY);
                BString str;
                datmsg.AddString("id", currentkeyid.String());
                datmsg.AddBool("setasdefault", wantsdefault);
                be_app->PostMessage(&datmsg);
                Quit();
            }
            break;
        }
        case 'sdef':
            if(setAsDefaultCheck->Value() == B_CONTROL_ON)
                wantsdefault = true;
            else if(setAsDefaultCheck->Value() == B_CONTROL_OFF)
                wantsdefault = false;
            break;
        case 'quit':
            be_app->PostMessage(B_QUIT_REQUESTED);
            Quit();
            break;
        default:
            BWindow::MessageReceived(msg);
            break;
    }
}

void SelectKeyDialogBox::_InitData()
{

    ((KeyStoreTestApp*)be_app)->CheckForAPIKeys(retrievedkeylist, NULL);

    for(int i = 0; i < retrievedkeylist.CountItems(); i++) {
        BString idstring = retrievedkeylist.ItemAt(i)->Identifier();
        BMessage thismsg('this');
        thismsg.AddString("id", idstring.String());
        keymenu->AddItem(new BMenuItem(idstring, new BMessage(thismsg)));
    }
}
