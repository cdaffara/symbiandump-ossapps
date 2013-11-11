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


#ifndef __CREATOR_MODULEBASE_H__
#define __CREATOR_MODULEBASE_H__

#include <e32base.h>

#include "engine.h"
#include "creator_traces.h"

// Dictionary uids for each Creator module.
// Dictionaries are for storing item identifiers created by Creator.
// Item identifiers are for enabling deletion of only items created by Creator. 
const TUid KUidDictionaryUidContacts         = { 0x00 };
const TUid KUidDictionaryUidContactGroups    = { 0x01 };
const TUid KUidDictionaryUidCalendar         = { 0x02 };
const TUid KUidDictionaryUidBrowserBookmarks = { 0x03 };
const TUid KUidDictionaryUidFiles            = { 0x04 };
const TUid KUidDictionaryUidLogs             = { 0x05 };
const TUid KUidDictionaryUidMessages         = { 0x06 };
const TUid KUidDictionaryUidMailbox          = { 0x07 };
const TUid KUidDictionaryUidIAP              = { 0x08 };
const TUid KUidDictionaryUidIMPS             = { 0x09 };
const TUid KUidDictionaryUidNotes            = { 0x0A };
const TUid KUidDictionaryUidLandmarks        = { 0x0B };
const TUid KUidDictionaryUidBrowserSavedPg   = { 0x0C };
const TUid KUidDictionaryUidBrowserBookmarkF = { 0x0D };
const TUid KUidDictionaryUidBrowserSavedPgF  = { 0x0E };

class CCreatorEngine;
class MCreatorModuleBaseParameters;
class CCommandParser;

_LIT(KSavingText, "Saving");
_LIT(KDeletingText, "Deleting");

class MCreatorModuleBase
    {
public:

private:
    // constructs the module, add "iEngine = aEngine" and other construction stuff to the body
    virtual void ConstructL( CCreatorEngine* aEngine ) = 0;

public:
    // this one is called when user select some features directly from menu, not running a script
    // should call CreateRandomData() function
    // returns ETrue when success, EFalse when user has cancelled
    virtual TBool AskDataFromUserL( TInt aCommand ) = 0;
    virtual void DeleteAllL() = 0;
    virtual void DeleteAllCreatedByCreatorL() = 0;
    };


class CCreatorModuleBase : public CBase, public MCreatorModuleBase, public MUIObserver
    {

public:
    enum TCreatorModuleStatus
        {
        ECreatorModuleDelete = 0, 
        ECreatorModuleStart
        };
    
    CCreatorModuleBase()
        {
        iEntriesToBeCreated = 1; 
        }
    
    virtual TBool AskDataFromUserL(TInt aCommand)
        {
        iCommand = aCommand;
        return EFalse;// will finish user interaction and engine will shutdown modules
        }
    
    virtual void QueryDialogClosedL(TBool aPositiveAction, TInt aUserData)
        {
        LOGSTRING("Creator: CCreatorModuleBase::QueryDialogClosedL");
        
        if( aPositiveAction == EFalse )
            {
            iEngine->ShutDownEnginesL();
            return;
            }
        const TDesC* showText = &KSavingText;
        TBool finished(EFalse);
        TBool retval(ETrue);
        switch(aUserData)
            {
            case ECreatorModuleDelete:
                showText = &KDeletingText;
                iEntriesToBeCreated = 1;
                finished = ETrue;
                break;
            case ECreatorModuleStart:
                finished = ETrue;
                break;
            default:
                //some error
                retval = EFalse;
                break;
            }
        if( retval == EFalse )
            {
            iEngine->ShutDownEnginesL();
            }
        else if( finished )
            {
            // add this command to command array
            iEngine->AppendToCommandArrayL(iCommand, NULL, iEntriesToBeCreated);
            // started exucuting commands
            iEngine->ExecuteFirstCommandL( *showText );
            }
        }

protected:
    // constructs the module, add "iEngine = aEngine" and other construction stuff to the body
    virtual void ConstructL(CCreatorEngine* aEngine)
        {
        iEngine = aEngine;
        };
    
protected:
    CCreatorEngine* iEngine;
    TInt iCommand;
    TInt iEntriesToBeCreated;
    TInt iDummy;
    };

class MCreatorModuleBaseParameters
    {
    // a base class for the parameters, no default implementation
    
    public:
    	enum TParseParams
    	{
    	EParamNone = 0,
    	ECalendarMeeting,
    	ECalendarMemo,
    	ECalendarAnniv,
    	ECalendarTodo
    	};
    
public:
	virtual void ParseL(CCommandParser* /*aParser*/, TParseParams /*aCase = 0*/) = 0;

	
    };
    
// Following struct represents the parameters that links for example contact groups
// to contac-sets.
typedef struct 
    {
    TInt iLinkId; // ID
    TInt iLinkAmount; // Amount of elements in linked set 
    } TLinkIdParam;

        
class CCreatorModuleBaseParameters : public CBase, public MCreatorModuleBaseParameters
        {
        public:
        virtual void ParseL(CCommandParser* /*aParser*/, TParseParams /*aCase = 0*/){};
        virtual TInt ScriptLinkId() const {return -1;};
        virtual void SetScriptLinkId(TInt) {};
        // a base class for the parameters, no default implementation
        };
        



#endif  // __CREATOR_MODULEBASE_H__
 
