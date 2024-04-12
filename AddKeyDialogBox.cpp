#include "AddKeyDialogBox.h"
// #include "URLView.h"
#include "KeyStoreTestDefs.h"
#include <AppKit.h>

AddKeyDialogBox::AddKeyDialogBox(BRect frame, const char* name, bool hasKeys)
: BWindow(frame, name, B_FLOATING_WINDOW_LOOK, B_FLOATING_ALL_WINDOW_FEEL,
    B_NOT_ZOOMABLE|B_NOT_RESIZABLE|B_AUTO_UPDATE_SIZE_LIMITS),
    currentapikey(""), currentid(""), _haskeys(hasKeys)
{
    BString description(
        "This application needs an API key to access the data from \n"
        "the data provider service %serviceProv% in order \n"
        "to work. Please write it down below. It will be saved in \n"
        "%location%."
    );
    description.ReplaceAll("%serviceProv%", "My Company Service");
    description.ReplaceAll("%location%", "the master keystore");

    BStringView* desc = new BStringView("sv_desc", description.String());

    BFont infont;
    float size = infont.Size();
    infont.SetFace(B_ITALIC_FACE);
    infont.SetSize(size * 0.9);

    BString providerdesc(
    "You can get a My Company Service API key by signing in at\n"
    "%providerUrl%"
    );
    providerdesc.ReplaceAll("%providerUrl%", "http://example.com/");

    BStringView* providerinfo = new BStringView("sv_prov", providerdesc.String());
    providerinfo->SetFont(&infont);

    apifield = new BTextControl("API Key", currentapikey, new BMessage('text'));
    idfield = new BTextControl("Identifier (for internal use)", currentid, new BMessage('id  '));

    saveButton = new BButton("bt_save", _haskeys ? "Save key" : "Save key and start Stocks",
        new BMessage('save'));
    quitButton = new BButton("bt_quit", _haskeys ? "Cancel" : "Quit Stocks",
        new BMessage('quit'));

    BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
        .SetInsets(B_USE_WINDOW_INSETS)
        .Add(desc)
        .AddStrut(4.0f)
        .Add(providerinfo)
        .AddStrut(6.0f)
        .AddGrid()
            .AddTextControl(apifield, 0, 0)
            .AddTextControl(idfield, 0, 1)
        .End()
        .AddStrut(8.0f)
        .AddGroup(B_HORIZONTAL)
            .Add(saveButton)
            .Add(quitButton)
        .End()
    .End();

    // if(_haskeys)
        // idfield->Hide();

    CenterOnScreen();
}

void AddKeyDialogBox::MessageReceived(BMessage* msg)
{
    switch(msg->what)
    {
        case 'text':
            currentapikey = apifield->Text();
            break;
        case 'id  ':
            currentid = idfield->Text();
            break;
        case 'save':
            if(!currentapikey.IsEmpty() && !currentid.IsEmpty()) {
                BMessage keymessage(M_USER_PROVIDES_KEY);
                keymessage.AddString("key", currentapikey.String());
                keymessage.AddString("id", currentid.String());
                be_app->PostMessage(&keymessage);
                QuitRequested();
            }
            break;
        case 'quit':
            if(!_haskeys)
                be_app->PostMessage(M_USER_CANCELLED_ADD_KEY);
            QuitRequested();
            break;
        default:
            BWindow::MessageReceived(msg);
            break;
    }
}

bool AddKeyDialogBox::QuitRequested()
{
    Quit();
    return true;
}

void AddKeyDialogBox::SetId(BString id)
{
    currentid = id;
    idfield->SetText(currentid.String());
}

