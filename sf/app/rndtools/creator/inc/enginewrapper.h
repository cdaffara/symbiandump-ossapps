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


#ifndef ENGINEWRAPPER_H
#define ENGINEWRAPPER_H

#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
#include <engine.h>

#include <QObject> // for iProgressNote signal connect


class MainView;
class MemoryDetails;
class HbProgressDialog;
class HbPopup;
class HbCommonNote;
class HbAction;


/**
 * class that is used for communicating between Symbian and Qt code.
 */
class EngineWrapper : public QObject
 { 
	Q_OBJECT

public:
    
    /**
     * Constructor
     */
    EngineWrapper();
    
    /**
     * Destructor
     */
    ~EngineWrapper();
    
    /**
     * Initializes Engine Wrapper
     * @return true if engine was started successfully
     */
    bool init();
	
	/**
	* Get memory details
	*/
	MemoryDetails GetMemoryDetails();
	
	/**
	 * Get memory details list
	 */
	QList<MemoryDetails> GetMemoryDetailsList();
	
public: 
    
    /* Functions that are called from UI */
	bool ExecuteOptionsMenuCommand(int commandId);

    
public:

    /* Functions that are called from engine: */
    
	/** 
     * uses Notifications class to show error message 
     */
    void ShowErrorMessage(const TDesC& aErrorMessage);

    /**
     * uses Notifications class to show progressbar
     */
    void ShowProgressBar(const TDesC& aPrompt, int aMax);
	
	/**
	* uses Notifications class to show user note
	*/
	void ShowNote(const TDesC& aNoteMessage, TInt aResourceId = 0);
    
    /**
     * increments shown progressbar's value
     */
    void IncrementProgressbarValue();
    
    /**
     * closes progressbar
     */
    void CloseProgressbar();	
	
	/**
	* Create entries query dialog
	*/
	TBool EntriesQueryDialog(TInt* aNumberOfEntries, const TDesC& aPrompt, TBool aAcceptsZero, MUIObserver* aObserver, TInt aUserData);

	/**
	* Create time query dialog
	*/    
    TBool TimeQueryDialog(TTime* aTime, const TDesC& aPrompt, MUIObserver* aObserver, TInt aUserData);

	/**
	* Create yes or no query dialog
	*/    
    TBool YesNoQueryDialog(const TDesC& aPrompt, MUIObserver* aObserver, int userData);
	
	/**
	* Popup list dialog for selecting item from dialog list
	*/
	TBool PopupListDialog(const TDesC& aPrompt, const CDesCArray* aFileNameArray, TInt* aIndex, MUIObserver* aObserver, TInt aUserData); 
	
	/**
	* Directory query dialog
	*/
	TBool DirectoryQueryDialog(const TDesC& aPrompt, TDes& aDirectory, MUIObserver* aObserver, TInt aUserData);
	
	/**
	* Create list query single-selection dialog
	*/
	TBool ListQueryDialog(const TDesC& aPrompt, TListQueryId aId, TInt* aSeletedItem, MUIObserver* aObserver, TInt aUserData);

	/**
	* Create list query multi-selection dialog
	*/
	TBool ListQueryDialog(const TDesC& aPrompt, TListQueryId aId, CArrayFixFlat<TInt>* aSelectedItems, MUIObserver* aObserver, TInt aUserData);
	
	
	/**
	* Close application when started from command line for script run.
	*/
	void CloseCreatorApp();

private slots:
	
	/**
	* progress dialog cancelled
	*/
	void ProgressDialogCancelled();
	
private:
    
    
    /* Creator engine */
    CCreatorEngine* iEngine;
    
    /* progress dialog that is shown */
    HbProgressDialog* iProgressDialog;
    
    MCreatorModuleBase *iModule;
};

#endif //ENGINEWRAPPER_H
