#ifndef __KEYSTORETEST_APP_H_
#define __KEYSTORETEST_APP_H_

#include <AppKit.h>
#include <Key.h>
#include <KeyStore.h>
#include <SupportDefs.h>

#include "AddKeyDialogBox.h"
#include "KeyStoreTestWin.h"
#include "SelectKeyDialogBox.h"

#define _SETTINGS_DEFKEY "apikey.default"

class KeyStoreTestApp : public BApplication
{
public:
                        KeyStoreTestApp       (const char* apikey = NULL);
    virtual            ~KeyStoreTestApp       ();
    virtual void        ReadyToRun            ();
    virtual void        MessageReceived       (BMessage*);
    virtual bool        QuitRequested         ();
    status_t            AddAPIKey             (BKeyStore&, const char*, const uint8*, BString);
    status_t            RemoveAPIKey          (BKeyStore&, const char*, const char*);
    status_t            CheckForAPIKeys       (BObjectList<BKey>&, uint32*);
    void                PrepareKeyring        (BKeyStore&, const char*);
    BString             CurrentDefaultKey     ();
private:
    void                _StandardStart        ();
    status_t            _LoadSettings         ();
    status_t            _SaveSettings         ();
private:
    KeyStoreTestWin    *window;
	AddKeyDialogBox    *adddlg;
    SelectKeyDialogBox *seldlg;
    BKeyStore           keystore;
    BMessage            currentSettings;
    BString             defaultkeyid;
    const char*         providedkey;
};

#endif /* __KEYSTORETEST_APP_H_ */
