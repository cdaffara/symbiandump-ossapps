/*
* Copyright (c) 2002-2004,2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  It provides implementation for the CPhoneBlacklistAnim class. 
*
*/



// INCLUDE FILES
#include    <e32base.h>                     
#include    <mmtsy_names.h>                 
#include    "phoneanimdllstd.h"             
#include    "cphoneblacklistanim.h"

// CONSTANTS
// Amount of retries to be performed.
const TInt KPhoneRetryCount = 7;

// Timeout in microseconds
const TInt KPhoneRetryTimeout = 100000; 

// MODULE DATA STRUCTURES
/**
* Active object for clearing nlacklist.
*
* @since 2.0
*/
class CPhoneBlacklistAnim::CClearBlacklist
    : public CActive
    {
    public: // Constructors and destructors.
        
        /**
        * Constructor.
        */
        /*****************************************************
        *    Series 60 Customer / ETel
        *    Series 60  ETel API
        *****************************************************/
        CClearBlacklist( RMmCustomAPI& aCustom );

        /**
        * Destructor.
        */
        ~CClearBlacklist();

    public: // New functions

        /**
        * Clear blacklist.
        */
        void Clear();

    protected:

        /**
        * From CActive, handles completion of request.
        */
        void RunL();

        /**
        * From CActive, cancels pending request.
        */
        void DoCancel();

    private:

        // Ref to custom API.
        /*****************************************************
        *    Series 60 Customer / ETel
        *    Series 60  ETel API
        *****************************************************/
        RMmCustomAPI& iCustom;

    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneBlacklistAnim::CPhoneBlacklistAnim
// ---------------------------------------------------------
//
CPhoneBlacklistAnim::CPhoneBlacklistAnim()
    {
    }

// ---------------------------------------------------------
// CPhoneBlacklistAnim::~CPhoneBlacklistAnim
// ---------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / ETel
*    Series 60  ETel API
*****************************************************/
CPhoneBlacklistAnim::~CPhoneBlacklistAnim()
    {
    if ( iFunctions )
        {
        iFunctions->GetRawEvents( EFalse );
        }

    delete iActive;

    iCustom.Close();
    iPhone.Close();
    iServer.Close();
    }

// ---------------------------------------------------------
// CPhoneBlacklistAnim::CommandReplyL
// 
// Reply to command. Always returns KErrNone.
// Defined as pure virtual in baseclass, not actually needed
// here.
// ---------------------------------------------------------
//
TInt CPhoneBlacklistAnim::CommandReplyL( TInt /*aOpcode*/, TAny* /*aArgs*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------
// CPhoneBlacklistAnim::Command
// 
// Command handling, only one command supported.
// ---------------------------------------------------------
//
void CPhoneBlacklistAnim::Command( TInt aOpcode, TAny* /*aArgs*/ )
    {
    switch ( aOpcode )
        {
    case ECancelBlackList:
        iClearBlacklistOnNextEvent = ETrue;
        break;
    case EClearBlackListNow:
        iActive->Clear();
        break;
    default:
        break;
        }
    };

// ---------------------------------------------------------
// CPhoneBlacklistAnim::Animate
// 
// Defined as pure virtual in base class, so empty 
// implementation.
// ---------------------------------------------------------
//
void CPhoneBlacklistAnim::Animate( TDateTime* /*aDateTime*/ )
    {
    }

// ---------------------------------------------------------
// CPhoneBlacklistAnim::ConstructL
// 
// Construction.
// Activates also request to get all key events.
// ---------------------------------------------------------
//
/*****************************************************
*    Series 60 Customer / ETel
*    Series 60  ETel API
*****************************************************/
/*****************************************************
*    Series 60 Customer / TSY
*    Needs customer TSY implementation
*****************************************************/
void CPhoneBlacklistAnim::ConstructL( TAny* /*aArgs*/, TBool /*aHasFocus*/ )
    {
    // Connect to ETel.
    TInt err = KErrNone; 
        {
        for ( TInt a = 0 ; a < KPhoneRetryCount ; a++ )
            {
            err = iServer.Connect();
            
            if ( err == KErrNone )
                {
                break;
                }

            User::After( KPhoneRetryTimeout );
            }
        
        User::LeaveIfError( err );
        }

    // Load TSY module.
    err = iServer.LoadPhoneModule( KMmTsyModuleName );
    if ( err != KErrAlreadyExists )
        {
        // LoadPhoneModule may return also KErrAlreadyExists 
        // if some other is already loaded the tsy module. 
        // And that is not an error.
        User::LeaveIfError( err );
        }

    // Open phone and custom API.
    User::LeaveIfError( iPhone.Open( iServer, KMmTsyPhoneName ) );
    User::LeaveIfError( iCustom.Open( iPhone ) );

    iActive = new ( ELeave ) CClearBlacklist( iCustom );

    iFunctions->GetRawEvents( ETrue );
    }

// ---------------------------------------------------------
// CPhoneBlacklistAnim::Redraw
// 
// Not needed. No UI.
// Defined as pure virtual in baseclass, not actually needed
// here.
// ---------------------------------------------------------
//
void CPhoneBlacklistAnim::Redraw()
    {
    }

// ---------------------------------------------------------
// CPhoneBlacklistAnim::FocusChanged
// 
// Not needed. No UI.
// Defined as pure virtual in baseclass, not actually needed
// here.
// ---------------------------------------------------------
//
void CPhoneBlacklistAnim::FocusChanged( TBool /*aState*/ )
    {
    }

// ---------------------------------------------------------
// CPhoneBlacklistAnim::OfferRawEvent
// 
// Handle key event.
// ---------------------------------------------------------
//
TBool CPhoneBlacklistAnim::OfferRawEvent( const TRawEvent& aRawEvent )
    {
    if ( aRawEvent.Type() == TRawEvent::EKeyDown || 
         aRawEvent.Type() == TRawEvent::EButton1Down )
        {
        if ( iClearBlacklistOnNextEvent )
            {
            iClearBlacklistOnNextEvent = EFalse;
            iActive->Clear();
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CPhoneBlacklistAnim::CClearBlacklist::CClearBlacklist
// ---------------------------------------------------------
//
CPhoneBlacklistAnim::CClearBlacklist::CClearBlacklist( 
        RMmCustomAPI& aCustom )
    : CActive( CActive::EPriorityStandard ),
      iCustom( aCustom )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CPhoneBlacklistAnim::CClearBlacklist::~CClearBlacklist
// ---------------------------------------------------------
//
CPhoneBlacklistAnim::CClearBlacklist::~CClearBlacklist()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CPhoneBlacklistAnim::CClearBlacklist::Clear
// 
// Clear blacklist asynchronously. 
// ---------------------------------------------------------
// 
/*****************************************************
*    Series 60 Customer / ETel
*    Series 60  ETel API
*****************************************************/
void CPhoneBlacklistAnim::CClearBlacklist::Clear()
    {
    Cancel();

    iCustom.ClearCallBlackList( iStatus );
    SetActive();
    }    

// ---------------------------------------------------------
// CPhoneBlacklistAnim::CClearBlacklist::RunL
// 
// This is called when blacklist clearing completes. 
// Empty implementation, as nothing needs to be done.
// ---------------------------------------------------------
// 
void CPhoneBlacklistAnim::CClearBlacklist::RunL()
    {
    }

// ---------------------------------------------------------
// CPhoneBlacklistAnim::CClearBlacklist::DoCancel
// 
// Cancels pending request.
// ---------------------------------------------------------
// 
/*****************************************************
*    Series 60 Customer / ETel
*    Series 60  ETel API
*****************************************************/
void CPhoneBlacklistAnim::CClearBlacklist::DoCancel()
    {
    iCustom.CancelAsyncRequest( 
        ECustomClearCallBlackListIPC );
    }

// End of File
