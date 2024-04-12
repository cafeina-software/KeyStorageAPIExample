#ifndef __ADDKEY_DLG_H_
#define __ADDKEY_DLG_H_

#include <InterfaceKit.h>

class AddKeyDialogBox : public BWindow
{
public:
    AddKeyDialogBox(BRect frame, const char* name, bool hasKeys = false);
    virtual void MessageReceived(BMessage* msg);
    virtual bool QuitRequested();
    void SetId(BString id);
private:
	BTextControl *apifield,
                 *idfield;
	BButton      *saveButton,
			     *quitButton;
    BString       currentapikey,
                  currentid;
    bool          _haskeys;
};

#endif /* __ADDKEY_DLG_H_ */
