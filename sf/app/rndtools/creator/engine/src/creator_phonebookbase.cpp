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

#include "engine.h"
#include "enginewrapper.h"
#include "creator_phonebookbase.h"
#include "creator_traces.h"

void CCreatorPhonebookBase::QueryDialogClosedL(TBool aPositiveAction, TInt aUserData)
    {
    LOGSTRING("Creator: CCreatorPhonebookBase::QueryDialogClosedL");
    
    if(aPositiveAction == EFalse && aUserData!=ECreatorPhonebookGetContactFields && aUserData!=ECreatorPhonebookContactsAllFields )
        {
        iEngine->ShutDownEnginesL();
        return;
        }
    
    const TDesC* showText = &KSavingText;
    TBool finished(EFalse);
    TBool retval(ETrue);
    switch(aUserData)
        {
        case ECreatorPhonebookDelete:
            showText = &KDeletingText;
            finished = ETrue;
            iEntriesToBeCreated = 1;
            break;
        case ECreatorPhonebookStart:
        	if(iCommand == ECmdCreatePhoneBookEntryContacts )
        		{
				retval = iEngine->GetEngineWrapper()->ListQueryDialog(_L("Fields in contact"), R_CONTACT_CREATION_TYPE_QUERY, 
        	                &iDummy, this, ECreatorPhonebookGetContactFields );
        		}
        	else
        		{
				retval = iEngine->GetEngineWrapper()->ListQueryDialog(_L("Number of contacts "), R_GROUP_CREATION_TYPE_QUERY, 
        	       	                &iDummy, this, ECreatorPhonebookGetGroupFields );
        		}
            break;
        case ECreatorPhonebookGetContactFields:
            if(iDummy==0)// first item, use default fields
                    
                {
                iDefaultFieldsSelected = ETrue;
                finished = ETrue;
                }
            else
                {
                retval = iEngine->GetEngineWrapper()->EntriesQueryDialog(&iNumberOfPhoneNumberFields, _L("Amount of phone number fields in one contact?"), 
                    ETrue, this, ECreatorPhonebookGetPhoneNumbersCount 
                    );
                }
            break;
        case ECreatorPhonebookGetPhoneNumbersCount:
            retval = iEngine->GetEngineWrapper()->EntriesQueryDialog(&iNumberOfURLFields, _L("Amount of URL fields in one contact?"), 
                ETrue, this, ECreatorPhonebookGetUrlsCount 
                );
            break;
        case ECreatorPhonebookGetUrlsCount:
            retval = iEngine->GetEngineWrapper()->EntriesQueryDialog(&iNumberOfEmailAddressFields, _L("Amount of email fields in one contact?"), 
                ETrue, this, ECreatorPhonebookGetEmailsCount
                );
            break;
        case ECreatorPhonebookGetEmailsCount:
            retval = iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Add all the other fields to contacts?"), this, ECreatorPhonebookContactsAllFields);
            break;
        case ECreatorPhonebookContactsAllFields:
            iAddAllFields = aPositiveAction;
            // finaly we have all informations from user, start engine
            finished = ETrue;
            break;
        case ECreatorPhonebookGetGroupFields:
            iContactsInGroup = KCreateRandomAmountOfGroups;
            if(iDummy==0)// first item, use default fields
                {
                finished = ETrue;
                }
            else
                {
                retval = iEngine->GetEngineWrapper()->EntriesQueryDialog(&iContactsInGroup, _L("Amount of contacts in one group?"), 
                    ETrue, this, ECreatorPhonebookGetContactsInGroup);
                }
            break;
        case ECreatorPhonebookGetContactsInGroup:
            // finaly we have all informations from user, start engine
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
    
TBool CCreatorPhonebookBase::AskDataFromUserL(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorPhonebookBase::AskDataFromUserL");
    iCommand = aCommand;
    
    if( aCommand == ECmdDeleteContacts )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Delete all contacts?"), this, ECreatorPhonebookDelete );
        }
    if( aCommand == ECmdDeleteCreatorContacts )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Delete all contacts created with Creator?"), this, ECreatorPhonebookDelete ); 
        }
    if( aCommand == ECmdDeleteContactGroups )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Delete all contact groups?"), this, ECreatorPhonebookDelete );
        }
    if( aCommand == ECmdDeleteCreatorContactGroups )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Delete all contact groups created with Creator?"), this, ECreatorPhonebookDelete ); 
        }    
    // display queries
    return iEngine->GetEngineWrapper()->EntriesQueryDialog( &iEntriesToBeCreated, _L("How many entries to create?"), EFalse, this, ECreatorPhonebookStart); // ask number of entries to create
    }


void CCreatorPhonebookBase::SetDefaultParameters()
    {
    iNumberOfPhoneNumberFields = 1;
    iNumberOfURLFields = 0;
    iNumberOfEmailAddressFields = 0;
    iContactsInGroup = 50;
    iDefaultFieldsSelected = EFalse;
    }
