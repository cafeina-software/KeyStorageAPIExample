#include <cstdio>
#include <private/interface/ColumnTypes.h>
#include "ManageKeysDialogBox.h"
#include "KeyStoreTestApp.h"
#include "AddKeyDialogBox.h"

ManageKeysDialogBox::ManageKeysDialogBox(BKeyStore& ks)
: BWindow(BRect(50, 50, 625, 400), "API Key manager", B_FLOATING_WINDOW, 0, 0),
    keystore(ks)
{
    clv = new BColumnListView("clv_keylist", 0);
    clv->SetSelectionMode(B_SINGLE_SELECTION_LIST);
    clv->SetSelectionMessage(new BMessage('isel'));
    clv->AddColumn(new BStringColumn("Name", 95, 75, 300, 0, B_ALIGN_LEFT), 0);
    clv->AddColumn(new BStringColumn("API Key", 200, 150, 600, 0, B_ALIGN_LEFT), 1);
    clv->AddColumn(new BStringColumn("Default", 70, 30, 100, 0, B_ALIGN_LEFT), 2);

    _InitData();

    addButton = new BButton("bt_addk", "Add", new BMessage('addk'));
    setDefaultButton = new BButton("bt_defk", "Set default", new BMessage('defk'));
    setDefaultButton->SetEnabled(false);
    copyButton = new BButton("bt_copy", "Copy", new BMessage('copy'));
    copyButton->SetEnabled(false);
    removeButton = new BButton("bt_remk", "Remove", new BMessage('remk'));
    removeButton->SetEnabled(false);
    closeButton = new BButton("bt_clse", "Close", new BMessage('clse'));

    BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
        .SetInsets(B_USE_WINDOW_INSETS)
        .AddGroup(B_HORIZONTAL)
            .Add(clv)
            .AddGroup(B_VERTICAL)
                .Add(addButton)
                .Add(setDefaultButton)
                .Add(copyButton)
                .Add(removeButton)
                .AddGlue()
                .Add(closeButton)
            .End()
        .End()
        .AddStrut(4.0f)
    .End();

    CenterOnScreen();
}

ManageKeysDialogBox::~ManageKeysDialogBox()
{
    if(adddb && adddb->Lock())
        adddb->Quit();
}

void ManageKeysDialogBox::MessageReceived(BMessage* msg)
{
    switch(msg->what)
    {
        case 'isel':
            if(clv->CountRows() > 0) {
                copyButton->SetEnabled(true);
                removeButton->SetEnabled(true);
                setDefaultButton->SetEnabled(true);
            }
            else {
                copyButton->SetEnabled(false);
                removeButton->SetEnabled(false);
                setDefaultButton->SetEnabled(false);
            }
            break;
        case 'addk':
        {
            adddb = new AddKeyDialogBox(BRect(), "Add new key", true);
            adddb->Show();
            _InitData();
            break;
        }
        case 'defk':
        {
            _SetDefault();
            break;
        }
        case 'copy':
        {
            if(be_clipboard->Lock() && clv->CurrentSelection() != NULL) {
                be_clipboard->Clear();

                const char* keydata = ((BStringField*)clv->CurrentSelection()->GetField(1))->String();

                BMessage* clipmsg = be_clipboard->Data();
                clipmsg->AddData("text/plain", B_MIME_TYPE, keydata, strlen(keydata));
                status_t status = be_clipboard->Commit();
                if(status != B_OK)
                    fprintf(stderr, "Error saving data to clipboard.\n");

                be_clipboard->Unlock();
            }
            break;
        }
        case 'remk':
        {
            BAlert *alert = new BAlert("Remove key?", "Do you want to remove this key?",
                "Do not remove", "Remove", NULL, B_WIDTH_AS_USUAL, B_WARNING_ALERT);
            alert->SetShortcut(0, B_ESCAPE);
            int32 result = alert->Go();

            if(result == 1) {
                BKey key;
                const char* keyid = ((BStringField*)clv->CurrentSelection()->GetField(0))->String();
                keystore.GetKey("Stocks", B_KEY_TYPE_GENERIC, keyid, key);
                status_t status = keystore.RemoveKey("Stocks", key);
                if(status == B_OK) {
                    _InitData();
                    copyButton->SetEnabled(false);
                    removeButton->SetEnabled(false);
                    setDefaultButton->SetEnabled(false);
                }
                else {
                    BAlert *error2alert = new BAlert("Error", "The key could not be removed.",
                        "OK", NULL, NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT);
                    error2alert->Go();
                }
            }
            break;
        }
        case 'clse':
            Quit();
            break;
        default:
            BWindow::MessageReceived(msg);
            break;
    }
}

void ManageKeysDialogBox::_InitData()
{
    clv->Clear();
    keylist.MakeEmpty();
    ((KeyStoreTestApp*)be_app)->CheckForAPIKeys(keylist, NULL);
    defaultkeyid = ((KeyStoreTestApp*)be_app)->CurrentDefaultKey();

    for(int i = 0; i < keylist.CountItems(); i++) {
        bool isdefault = strcmp(keylist.ItemAt(i)->Identifier(), defaultkeyid.String()) == 0;

        BRow* row = new BRow();
        row->SetField(new BStringField(keylist.ItemAt(i)->Identifier()), 0);
        row->SetField(new BStringField(reinterpret_cast<const char*>(keylist.ItemAt(i)->Data())), 1);
        row->SetField(new BStringField(isdefault ? "Yes" : "No"), 2);
        clv->AddRow(row);
    }
}

void ManageKeysDialogBox::_SetDefault()
{
    defaultkeyid = ((BStringField*)clv->CurrentSelection()->GetField(0))->String();
    BMessage imsg(M_SAVE_DEFAULT);
    imsg.AddString(_SETTINGS_DEFKEY, defaultkeyid);
    be_app->PostMessage(&imsg);
    _InitData();
}
