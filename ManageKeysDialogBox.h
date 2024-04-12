/*
 * Copyright 2018, Your Name <your@email.address>
 * All rights reserved. Distributed under the terms of the MIT license.
 */
#ifndef __MANAGEKEYS_DLG_H_
#define __MANAGEKEYS_DLG_H_

#include "AddKeyDialogBox.h"
#include <InterfaceKit.h>
#include <Key.h>
#include <KeyStore.h>
#include <SupportDefs.h>
#include <private/interface/ColumnListView.h>

class ManageKeysDialogBox : public BWindow
{
public:
                  ManageKeysDialogBox(BKeyStore&);
                 ~ManageKeysDialogBox();
    virtual void  MessageReceived(BMessage*);
private:
    void          _InitData();
    void          _SetDefault();
private:
    BObjectList<BKey>  keylist;
    BColumnListView   *clv;
    BButton           *addButton,
                      *setDefaultButton,
                      *copyButton,
                      *removeButton,
                      *closeButton;
    BKeyStore          keystore;
    AddKeyDialogBox   *adddb;
    BString            defaultkeyid;
};

#endif /* __MANAGEKEYS_DLG_H_ */
