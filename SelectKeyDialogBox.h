#ifndef __SELECTKEYDLG_H_
#define __SELECTKEYDLG_H_

#include <InterfaceKit.h>
#include <Key.h>

class SelectKeyDialogBox : public BWindow
{
public:
    SelectKeyDialogBox(BRect frame, const char* name);
    virtual void MessageReceived(BMessage* msg);
    // virtual bool QuitRequested();
private:
    void _InitData();
private:
    BMenuField        *keyselection;
    BPopUpMenu        *keymenu;
    BCheckBox         *setAsDefaultCheck;
	BButton           *selButton,
			          *quitButton;
    BObjectList<BKey>  retrievedkeylist;
    BString            currentkeyid;
    bool               wantsdefault;
};

#endif /* __SELECTKEYDLG_H_ */
