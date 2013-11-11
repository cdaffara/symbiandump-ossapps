/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of TPhoneCmdParamQuery class.
*
*/


// INCLUDE FILES
#include "tphonecmdparamquery.h"
#include "phoneui.pan"

const TInt KUninitializedTimeOutCmd = -1;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// TPhoneCmdParamQuery::TPhoneCmdParamQuery
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamQuery::TPhoneCmdParamQuery():
    TPhoneCommandParam(),
    iDialogResourceId( 0 ),
    iDialogPrompt( KNullDesC ),
    iDefaultCbaResourceId( 0 ), 
    iContentCbaResourceId( 0 ), 
    iDataText( NULL ),
    iQueryType( EPhoneQueryDialog ),
    iSendKeyEnabled( EFalse ),
    iTimeOut( 0 ),
    iTone( 0 ),
    iCustomCmdForTimeOut( KUninitializedTimeOutCmd ),
    iCommandObserver( NULL ),
    iItemTextArray( NULL )
    {
    iParamId = EPhoneParamIdQuery;
    }

// -----------------------------------------------------------------------------
// TPhoneCmdParamQuery::TPhoneCmdParamQuery
// C++ copy constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamQuery::TPhoneCmdParamQuery( 
        const TPhoneCmdParamQuery& aParam )
    {
    iParamId = aParam.iParamId;
    iDialogResourceId = aParam.iDialogResourceId;
    iDialogPrompt.Set( aParam.iDialogPrompt );
    iDefaultCbaResourceId = aParam.iDefaultCbaResourceId;
    iContentCbaResourceId = aParam.iContentCbaResourceId;
    iDataText = aParam.iDataText;
    iQueryType= aParam.iQueryType;
    iSendKeyEnabled = aParam.iSendKeyEnabled;
    iTimeOut = aParam.iTimeOut;
    iTone = aParam.iTone;
    iCustomCmdForTimeOut = aParam.iCustomCmdForTimeOut;
    iCommandObserver = aParam.iCommandObserver;
    iItemTextArray = aParam.iItemTextArray;
    for ( TInt i = aParam.iCbaCommandMappings.Count() - 1; 0 <= i; i-- )
        {
        iCbaCommandMappings[i] = aParam.iCbaCommandMappings[i];
        }
    }


// -----------------------------------------------------------------------------
// TPhoneCmdParamQuery::operator=
// -----------------------------------------------------------------------------
//
EXPORT_C TPhoneCmdParamQuery& TPhoneCmdParamQuery::operator=( 
        const TPhoneCmdParamQuery& aParam )
    {
    if ( this != &aParam )
        {
        iParamId = aParam.iParamId;
        iDialogResourceId = aParam.iDialogResourceId;
        iDialogPrompt.Set( aParam.iDialogPrompt );
        iDefaultCbaResourceId = aParam.iDefaultCbaResourceId;
        iContentCbaResourceId = aParam.iContentCbaResourceId;
        iDataText = aParam.iDataText;
        iQueryType= aParam.iQueryType;
        iSendKeyEnabled = aParam.iSendKeyEnabled;
        iTimeOut = aParam.iTimeOut;
        iTone = aParam.iTone;
        iCustomCmdForTimeOut = aParam.iCustomCmdForTimeOut;
        iCommandObserver = aParam.iCommandObserver;
        iItemTextArray = aParam.iItemTextArray;
        for ( TInt i = aParam.iCbaCommandMappings.Count() - 1; 0 <= i; i-- )
            {
            iCbaCommandMappings[i] = aParam.iCbaCommandMappings[i];
            }
        }
    
    return *this;
    }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetQueryResourceId
// Sets the query dialog resource id
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetQueryResourceId( TInt aDialogResourceId )
    {
    __ASSERT_DEBUG( aDialogResourceId, 
        Panic( EPhoneUtilsParameterNotInitialized ) );
    iDialogResourceId = aDialogResourceId;   
    }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::QueryResourceId
// Returns the query dialog's resource id
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamQuery::QueryResourceId() const
    {   
    return iDialogResourceId;
    }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetQueryPrompt
// Sets the query dialog's prompt text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetQueryPrompt( const TDesC& aDialogPrompt )
   {
   iDialogPrompt.Set( aDialogPrompt );
   }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::QueryPrompt
// Returns the query dialog's prompt text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C const TDesC& TPhoneCmdParamQuery::QueryPrompt() const
   {
   return iDialogPrompt;
   }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetDefaultCba
// Sets the default CBA resource id
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetDefaultCba( TInt aCbaResourceId )
   {
   iDefaultCbaResourceId = aCbaResourceId;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::DefaultCba
// Returns the default CBA resource id
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamQuery::DefaultCba() const
   {
   return iDefaultCbaResourceId;
   }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetContentCba
// Sets the content CBA resource id
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetContentCba( TInt aCbaResourceId )
   {
   iContentCbaResourceId = aCbaResourceId;   
   }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::ContentCba
// Returns the content CBA resource id
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamQuery::ContentCba() const
   {
   return iContentCbaResourceId;
   }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetDataText
// Sets the data text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetDataText( TDes* aDataText )
    {
    __ASSERT_DEBUG( aDataText, Panic( EPhoneUtilsParameterNotInitialized ) );
    iDataText = aDataText;   
    }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::DataText
// Returns the data text
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TDes* TPhoneCmdParamQuery::DataText() const
    {
    return iDataText;
    }
   
// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetQueryType
// Sets the query type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetQueryType( TPhoneQueryType aQueryType )
    {
    iQueryType = aQueryType;        
    }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::QueryType
// Returns the query type
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TPhoneQueryType TPhoneCmdParamQuery::QueryType() const
    {
    return iQueryType;    
    }
    
// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetSendKeyEnabled
// Sets the send key enabled status for the query
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetSendKeyEnabled( TBool aSendKeyEnabled )
    {
    iSendKeyEnabled = aSendKeyEnabled;
    }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::SendKeyEnabled
// Returns the send key enabled status for the query
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool TPhoneCmdParamQuery::SendKeyEnabled() const
    {
    return iSendKeyEnabled;    
    }
    
// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetCommandParamId
// Sets the query command param id
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetCommandParamId( TPhoneCommandParam::TPhoneParamId aParamId )
   {
   iParamId = aParamId;
   }
   
// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetTimeOut
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetTimeOut( TInt aTimeOut )
    {
    iTimeOut = aTimeOut;
    }   

// ---------------------------------------------------------
// TPhoneCmdParamQuery::TimeOut
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamQuery::TimeOut() const
    {
    return iTimeOut; 
    }


// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetTone
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetTone( TInt aTone )
    {
    __ASSERT_DEBUG( 0 <= aTone , Panic( EPhoneUtilsInvalidArgument ) );
    if ( 0 <= iTone )
        {
        iTone = aTone;
        }
    }


// ---------------------------------------------------------
// TPhoneCmdParamQuery::Tone
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamQuery::Tone() const
    {
    return iTone;
    }


// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetCbaCommandMapping
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetCbaCommandMapping( 
        TInt aCbaCommandId, TInt aCustomCommandId )
    {
    TCbaCommandMapping unallocatedMapping = TCbaCommandMapping();
    TBool mappingCreated = EFalse;
    TInt maxNumOfMappings = iCbaCommandMappings.Count();
    
    for ( TInt i = 0; i < maxNumOfMappings && !mappingCreated; i++ )
        {
        if ( iCbaCommandMappings[i] == unallocatedMapping || 
             iCbaCommandMappings[i].CbaCommandId() == aCbaCommandId )
            {
            mappingCreated = ETrue;
            TCbaCommandMapping mapping( aCbaCommandId, aCustomCommandId );
            iCbaCommandMappings[i] = mapping;
            }
        }
    
    __ASSERT_DEBUG( mappingCreated, Panic( EPhoneUtilsIndexOutOfBounds ) );
    }


// ---------------------------------------------------------
// TPhoneCmdParamQuery::CbaCommandMapping
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamQuery::CbaCommandMapping( 
        TInt aCbaCommandId ) const
    {
    TInt maxNumOfMappings = iCbaCommandMappings.Count(); 
    for ( TInt i = 0; i < maxNumOfMappings; i++ )
        {
        if ( iCbaCommandMappings[i].CbaCommandId() == aCbaCommandId )
            {
            return iCbaCommandMappings[i].CustomCommandId();
            }
        }
    return aCbaCommandId;
    }


// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetCustomCommandForTimeOut
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetCustomCommandForTimeOut( 
        TInt aCustomCommandId )
    {
    iCustomCmdForTimeOut = aCustomCommandId;
    }


// ---------------------------------------------------------
// TPhoneCmdParamQuery::GetCustomCommandForTimeOut
// ---------------------------------------------------------
//
EXPORT_C TInt TPhoneCmdParamQuery::GetCustomCommandForTimeOut( 
        TInt& aCustomCommandId ) const
    {
    if ( KUninitializedTimeOutCmd == iCustomCmdForTimeOut )
        {
        return KErrNotFound;
        }
    else
        {
        aCustomCommandId = iCustomCmdForTimeOut;
        return KErrNone;
        }
    }


// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetQueryCommandObserver
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetQueryCommandObserver( 
        MPhoneQueryCommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }


// ---------------------------------------------------------
// TPhoneCmdParamQuery::QueryCommandObserver
// ---------------------------------------------------------
//
EXPORT_C MPhoneQueryCommandObserver* 
    TPhoneCmdParamQuery::QueryCommandObserver()
    {
    return iCommandObserver;
    }

// ---------------------------------------------------------
// TPhoneCmdParamQuery::SetItemTextArrayForListQuery
// ---------------------------------------------------------
//
EXPORT_C void TPhoneCmdParamQuery::SetItemTextArrayForListQuery( 
        MDesCArray& aItemTextArray )
    {
    iItemTextArray = &aItemTextArray;
    }


// ---------------------------------------------------------
// TPhoneCmdParamQuery::ItemTextArrayForListQuery
// ---------------------------------------------------------
//
EXPORT_C MDesCArray* TPhoneCmdParamQuery::ItemTextArrayForListQuery() const
    {
    return iItemTextArray;
    }

//  End of File  
