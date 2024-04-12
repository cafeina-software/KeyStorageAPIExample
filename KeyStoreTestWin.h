/*
 * Copyright 2018, Your Name <your@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef __KEYSTORETEST_WIN_H_
#define __KEYSTORETEST_WIN_H_

#include <InterfaceKit.h>
#include <SupportDefs.h>
#include "KeyStoreTestDefs.h"
#include "ManageKeysDialogBox.h"

class KeyStoreTestWin : public BWindow
{
public:
	KeyStoreTestWin(BKey);
    virtual ~KeyStoreTestWin();
    virtual void MessageReceived(BMessage*);
    // status_t             LoadSettings        (BMessage* indata);
    // status_t             SaveSettings        (BMessage* outdata);
private:
    ManageKeysDialogBox* keymanager;
    BMenuBar* menubar;
    BView* keyView;
    BKey key;
    // BString defaultkeyid;
};


#endif /* __KEYSTORETEST_WIN_H_ */
