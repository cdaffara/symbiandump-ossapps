/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#ifndef NOTIFICATIONS_H_
#define NOTIFICATIONS_H_

#include <hbmessagebox.h>
#include <hbselectiondialog.h>
#include <hbinputdialog.h>

#include "engine.h"

#include <stdexcept>

class HbProgressDialog;
class HbPopup;

class Notifications : public HbPopup
    {

	Q_OBJECT 	
	
public:
    /**
     * shows massage box
     */
	static void showMessageBox(HbMessageBox::MessageBoxType type, const QString &text, const QString &label, int timeout );
	
    /**
     * shows about box
     */
    static void about();
    
    /** 
     * shows error message 
     */
    static void error(const QString& errorMessage);
	
	/**
     * to shows progressbar
     */
	static HbProgressDialog* showProgressBar(const QString& text, int max);
	
    /**
     * shows global HbGlobalCommonNote type note
     */
	static void showGlobalNote(const QString& text, HbMessageBox::MessageBoxType type, int timeout = 1000 );
    };

class CreatorDialog
{
    Q_DISABLE_COPY(CreatorDialog)
protected:
    CreatorDialog(MUIObserver* observer, int userData) throw(std::exception) : mObserver(observer), mUserData(userData)
        {
            if(!mObserver)
                throw std::invalid_argument("module cannot be the null!");
        };
protected:
    virtual void NotifyObserver(TBool aPositiveAction)
        {
        TRAPD( err, mObserver->QueryDialogClosedL(aPositiveAction, mUserData) );
        if(err)
            {
            Notifications::error( QString("Symbian Leave: %1 ").arg(err) );
            }
        }
    
protected:
    MUIObserver* mObserver;
    int mUserData;
};

class CreatorYesNoDialog : public HbMessageBox, public CreatorDialog
{
    Q_DISABLE_COPY(CreatorYesNoDialog)
    Q_OBJECT
public:
    static void launch(const QString& text, const QString& label, MUIObserver* observer, int userData) throw( std::exception );
protected:
    CreatorYesNoDialog(MUIObserver* observer, int userData);
protected slots:
    void DialogClosed(HbAction*);
};

class CreatorInputDialog : public HbInputDialog, public CreatorDialog
{
    Q_DISABLE_COPY(CreatorInputDialog)
    Q_OBJECT
public:
    static void launch(const QString& label, int* value, bool acceptZero,  MUIObserver* observer, int userData) throw( std::exception );
    static void launch(const QString& label, TDes& value,  MUIObserver* observer, int userData) throw( std::exception );
protected:
    CreatorInputDialog(int* value, MUIObserver* observer, int userData);
    CreatorInputDialog(TDes& value, MUIObserver* observer, int userData);
protected slots:
    void DialogClosed(HbAction*);
private:
    int *mIntValue;
    TDes& mStrValue;
    TBuf<1> mDummy;
};

class CreatorSelectionDialog : public HbSelectionDialog, public CreatorDialog
{
    Q_DISABLE_COPY(CreatorSelectionDialog)
    Q_OBJECT
public:
    static void launch(const QString& label, const QStringList& items, TInt* selectedItem, MUIObserver* observer, int userData) throw( std::exception );
    static void launch(const QString& label, const QStringList& items, CArrayFixFlat<TInt>* aSelectedItems, MUIObserver* observer, int userData) throw( std::exception );
protected:
    CreatorSelectionDialog(TInt* selectedItem, MUIObserver* observer, int userData) throw( std::exception );
    CreatorSelectionDialog(CArrayFixFlat<TInt>* aSelectedItems, MUIObserver* observer, int userData) throw( std::exception );
protected slots:
    void DialogClosed(HbAction*);
private:
    TInt* mSelectedItem;
    CArrayFixFlat<TInt>* mSelectedItems;
};

class CreatorDateTimeDialog : public HbDialog, public CreatorDialog
{
    Q_DISABLE_COPY(CreatorDateTimeDialog)
    Q_OBJECT
public:
    static void launch(const QString& label, TTime* value, MUIObserver* observer, int userData) throw( std::exception );
protected:
    CreatorDateTimeDialog(TTime* value, MUIObserver* observer, int userData);
protected slots:
    void DialogClosed(HbAction*);
private:
    TTime *mValue;
};
#endif // NOTIFICATIONS_H_
