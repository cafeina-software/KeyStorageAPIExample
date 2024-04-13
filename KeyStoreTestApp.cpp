#include "KeyStoreTestApp.h"
#include "KeyStoreTestDefs.h"
#include <Path.h>
#include <cstdio>
#include <PathFinder.h>
// #include <thread>

KeyStoreTestApp::KeyStoreTestApp(const char* apikey)
: BApplication(kAppSignature), providedkey(apikey)
{
	fprintf(stderr, "App start...\n");

    _LoadSettings();

    adddlg = new AddKeyDialogBox(BRect(50, 50, 600, 500), "Add key");
    seldlg = new SelectKeyDialogBox(BRect(50, 50, 600, 500), "Select key");
}

KeyStoreTestApp::~KeyStoreTestApp()
{
    fprintf(stderr, "App end...\n");
    if(adddlg && adddlg->Lock())
        adddlg->Quit();
    if(seldlg && seldlg->Lock())
        seldlg->Quit();
}

void KeyStoreTestApp::ReadyToRun()
{
    if (providedkey != NULL) {
        BKeyStore keystore;
        BKey key;

        PrepareKeyring(keystore, kAppKeyring);
        AddAPIKey(keystore, kAppKeyring, reinterpret_cast<const uint8*>(providedkey),
            providedkey);
        keystore.GetKey(kAppKeyring, B_KEY_TYPE_GENERIC, providedkey,
            NULL, false, key);

        window = new KeyStoreTestWin(key);
        window->Show();
    }
    else
        _StandardStart();
}

void KeyStoreTestApp::MessageReceived(BMessage* msg)
{
    switch(msg->what)
    {
        case M_USER_PROVIDES_KEY:
        {
            BKey key;
            BString newkey, newid;
            if(msg->FindString("key", &newkey) == B_OK &&
            msg->FindString("id", &newid) == B_OK) {
                AddAPIKey(keystore, kAppKeyring,
                    reinterpret_cast<const uint8*>(newkey.String()), newid);
                keystore.GetKey(kAppKeyring, B_KEY_TYPE_GENERIC, newid.String(),
                    NULL, key);

                uint32 count;
                BObjectList<BKey> objlist;
                CheckForAPIKeys(objlist, &count);
                if(count == 1)
                    defaultkeyid = key.Identifier();

                window = new KeyStoreTestWin(key);
                window->Show();
            }
            break;
        }
        case M_USER_SELECTED_KEY:
        {
            BKey key;
            BString keyid = msg->GetString("id");
            keystore.GetKey(kAppKeyring, B_KEY_TYPE_GENERIC, keyid.String(), NULL, key);
            if(msg->GetBool("setasdefault", false) == true)
                defaultkeyid = key.Identifier();

            window = new KeyStoreTestWin(key);
            window->Show();
            break;
        }
        case M_USER_CANCELLED_ADD_KEY:
            PostMessage(B_QUIT_REQUESTED);
            break;
        case M_SAVE_DEFAULT:
            defaultkeyid = msg->GetString(_SETTINGS_DEFKEY);
            fprintf(stderr, "New default is %s\n", defaultkeyid.String());
            break;
        default:
            BApplication::MessageReceived(msg);
            break;
    }
}

bool KeyStoreTestApp::QuitRequested()
{
    _SaveSettings();
    return BApplication::QuitRequested();
}

status_t KeyStoreTestApp::AddAPIKey(BKeyStore& keystore, const char* keyring,
    const uint8* data, BString primId = "mainkey")
{
    size_t datalength = strlen(reinterpret_cast<const char*>(data));
    return keystore.AddKey(keyring, BKey(B_KEY_PURPOSE_GENERIC, primId.String(),
        NULL, data, datalength));
}

status_t KeyStoreTestApp::RemoveAPIKey(BKeyStore& keystore, const char* keyring,
    const char* keyid)
{
    BKey key;
    keystore.GetKey(keyring, B_KEY_TYPE_GENERIC, keyid, key);
    return keystore.RemoveKey(keyring, key);
}

status_t KeyStoreTestApp::CheckForAPIKeys(BObjectList<BKey>& keylist, uint32* i)
{
    status_t status = B_OK;
    // BKeyStore keystore;
    uint32 cookie = 0;
    BKey currentkey;
    bool next = true;

    PrepareKeyring(keystore, kAppKeyring);

    while(next) {
        status = keystore.GetNextKey(kAppKeyring, B_KEY_TYPE_GENERIC,
            B_KEY_PURPOSE_GENERIC, cookie, currentkey);
        switch(status) {
            case B_OK:
                keylist.AddItem(new BKey(currentkey));
                break;
            case B_BAD_VALUE:
                // by the time this function is called the keyring
                // should already exist, but in case of error, we can try
                // to create it again
                PrepareKeyring(keystore, kAppKeyring);
                // flows below
            case B_ENTRY_NOT_FOUND:
                next = false;
                break;
            case B_NOT_ALLOWED:
            default:
                next = false;
                break;
        }
    }

    if(i != NULL)
        *i = cookie;

    return status;
}

void KeyStoreTestApp::PrepareKeyring(BKeyStore& keystore, const char* keyringname) {
    bool found = false;
    uint32 cookie = 0;
    BString curkeyringname;
    while (!found) {
        switch(keystore.GetNextKeyring(cookie, curkeyringname))
        {
            case B_OK:
                if(strcmp(keyringname, curkeyringname.String()) == 0) {
                    found = true;
                    fprintf(stderr, "Target keyring found.\n");
                }
                else
                    fprintf(stderr, "Keyring found, not the targetted one.\n");
                break;
            case B_ENTRY_NOT_FOUND:
                keystore.AddKeyring(keyringname);
                fprintf(stderr, "Keyring not found, creating it...\n");
                break;
            default:
                fprintf(stderr, "Error trying to access keyring.\n");
                break;
        }
    }
}

BString KeyStoreTestApp::CurrentDefaultKey  ()
{
    return defaultkeyid;
}

void KeyStoreTestApp::_StandardStart()
{
    // uint32 count;
    BObjectList<BKey> keylist;
    // std::thread keythread(&KeyStoreTestApp::CheckForAPIKeys, this, std::ref(keylist), &count);
    // keythread.join();
    status_t status = CheckForAPIKeys(keylist, NULL);
    bool hasdefault = !defaultkeyid.IsEmpty();

    // keystore permission denied
    if(status == B_NOT_ALLOWED) {
        BAlert* alert = new BAlert("Error",
            "Error: access to the keystore was not allowed. "
            "The application will be closed.", "OK");
        alert->Go();
        Quit();
    }
    // keystore access allowed, no keys
    else if(keylist.IsEmpty()) {
        adddlg->SetId("mainkey");
        adddlg->Show();
    }
    // keystore access allowed, has key(s)
    else {
        if(hasdefault) {
            BKey key;
            if(currentSettings.FindString(_SETTINGS_DEFKEY, &defaultkeyid) == B_OK) {
                keystore.GetKey(kAppKeyring, B_KEY_TYPE_GENERIC, defaultkeyid,
                    NULL, false, key);
                fprintf(stderr, "default keyid retrieved is %s\n", key.Identifier());
            }
            window = new KeyStoreTestWin(key);
            window->Show();
        }
        else {
            seldlg->Show();
        }
    }
}

status_t KeyStoreTestApp::_LoadSettings()
{
    status_t status = B_OK;
    BPath usrsetpath;
    if((status = find_directory(B_USER_SETTINGS_DIRECTORY, &usrsetpath)) != B_OK) {
        // __trace("User settings dir could not even be found\n");
        return status;
    }

    usrsetpath.Append(kAppName ".settings");
    BFile file(usrsetpath.Path(), B_READ_ONLY);
    if((status = file.InitCheck()) != B_OK) {
        // __trace("User settings file \"%s\" could not even be opened\n", usrsetpath.Path());
        return status;
    }

    if((status = currentSettings.Unflatten(&file)) != B_OK) {
        // __trace("Settings could not be unflattened from the file \n");
        return status;
    }

    if(currentSettings.FindString(_SETTINGS_DEFKEY, &defaultkeyid) != B_OK)
        return B_ERROR;

    fprintf(stderr, "Current default key is %s\n", defaultkeyid.String());
    return status;
}

status_t KeyStoreTestApp::_SaveSettings()
{
    status_t status = B_OK;

    if(currentSettings.ReplaceString(_SETTINGS_DEFKEY, defaultkeyid.String()) != B_OK)
        currentSettings.AddString(_SETTINGS_DEFKEY, defaultkeyid.String());

    BPath usrsetpath;
    if((status = find_directory(B_USER_SETTINGS_DIRECTORY, &usrsetpath)) != B_OK) {
        // __trace("User settings dir could not even be found\n");
        return status;
    }

    usrsetpath.Append(kAppName ".settings");
    BFile file(usrsetpath.Path(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
    if((status = file.InitCheck()) != B_OK) {
        // __trace("User settings file \"%s\" could not even be set\n", usrsetpath.Path());
        return status;
    }
    file.SetPermissions(DEFFILEMODE);

    if((status = currentSettings.Flatten(&file)) != B_OK) {
        // __trace("Settings could not be flattened to the file \n");
        return status;
    }

    return status;
}
