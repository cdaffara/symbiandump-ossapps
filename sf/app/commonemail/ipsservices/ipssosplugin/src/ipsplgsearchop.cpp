/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Operation that searches string from email.
*
*/



#include "emailtrace.h"
#include "ipsplgheaders.h"

/** Snippet length, for HTML tags. */
const TInt KIpsPlgSnippetLen = 300;

// changed to one mail per one RunL - for cycle in RunL was removed
// Defines the number of mails handled in one scheduler round
// const TInt KIpsPlgSearchMailsInRound = 10;

// ======== CONSTRUCTORS & DESTRUCTOR ========

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::NewL()
// ---------------------------------------------------------------------------
//
CIpsPlgSearchOp* CIpsPlgSearchOp::NewL(
    CIpsPlgSearch& aObserver )
    {
    FUNC_LOG;
    CIpsPlgSearchOp* self = CIpsPlgSearchOp::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::NewLC()
// ---------------------------------------------------------------------------
//
CIpsPlgSearchOp* CIpsPlgSearchOp::NewLC(
    CIpsPlgSearch& aObserver )
    {
    FUNC_LOG;
    CIpsPlgSearchOp* self = new( ELeave ) CIpsPlgSearchOp( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::~CIpsPlgSearchOp()
// ---------------------------------------------------------------------------
//
CIpsPlgSearchOp::~CIpsPlgSearchOp()
    {
    FUNC_LOG;
    delete iMessage;
    iMessage = NULL;
    delete iHeader;
    iHeader = NULL;
    if ( iSearcher )
        {
        iSearcher->Cleanup();
        }
    delete iSearcher;
    iSearcher = NULL;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::CIpsPlgSearchOp()
// ---------------------------------------------------------------------------
//
CIpsPlgSearchOp::CIpsPlgSearchOp(
    CIpsPlgSearch& aObserver )
    :
    CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iMessage( NULL )
    {
    FUNC_LOG;
    iRequiredPriority = Priority(); // priority changes are enabled
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::ConstructL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearchOp::ConstructL()
    {
    FUNC_LOG;
    iState = EStarted;

    iSearcher = CIpsPlgTextSearcher::NewL( *this );
    iSearcher->SetParametersL(
        iObserver.SearchStringsL(),
        EIpsPlgCriteriaOperationOR,
        EFalse,
        KIpsPlgSnippetLen );

    ActivateAndComplete();
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// From class CIpsPlgBaseOperation.
// CIpsPlgSearchOp::DoCancel()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearchOp::DoCancel()
    {
    FUNC_LOG;
    iSearcher->Cleanup();
    iState = ECanceled;
    }

// ---------------------------------------------------------------------------
// From class CIpsPlgBaseOperation.
// CIpsPlgSearchOp::RunL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearchOp::RunL()
    {
    FUNC_LOG;
    switch ( iState )
        {
        case EStarted:
            {
            // Collect all message id's in the mailbox and sort them.
            iObserver.CollectMessagesL();
            iObserver.Sort();
            iState = ERunning;
            iRequiredPriority = EPriorityIdle; // priority changes may be decreased
            ActivateAndComplete();
            break;
            }
        case ERunning:
            {
                if ( NextMailL() )
                    {
                    // Mail found, read the content and search for the string.
                    // It must be made sure, that the search string contains valid 
                    // data to search the body, i.e. character set must match.
                    // The following line is commented out since it currently doesn't do anything
                    // HandleHtmlL(); 
                    SearchL();
                    }
                else
                    {
                    FinalizeL();
                    return;
                    }
            iState = ERunning;
// When client wants call to contact the priority must be decreased
// to enable search for contact which uses idle priority
            TInt clientRequiredPriority(iRequiredPriority);
            iObserver.ClientRequiredSearchPriority( &clientRequiredPriority );

            if (clientRequiredPriority>EPriorityStandard) {
			    clientRequiredPriority = EPriorityStandard;
			}
            else if(clientRequiredPriority<EPriorityIdle-1) {
			    clientRequiredPriority = EPriorityIdle-1;
			}
            iRequiredPriority = clientRequiredPriority;

            ActivateAndComplete();
            break;
            }
        default:
            {
            break;
            }
        }

    }

// ---------------------------------------------------------------------------
// From class CIpsPlgBaseOperation.
// CIpsPlgSearchOp::RunError()
// ---------------------------------------------------------------------------
//
TInt CIpsPlgSearchOp::RunError( TInt /* aError */ )
    {
    FUNC_LOG;
    return KErrGeneral;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::NextMailL()
// ---------------------------------------------------------------------------
//
TBool CIpsPlgSearchOp::NextMailL()
    {
    FUNC_LOG;
    // In case the operation is cancelled, do not try to any extra steps.
    if ( iState == ERunning )
        {
        // Delete object before creating new. Previous object is not needed.
        delete iMessage;
        iMessage = NULL;
        delete iHeader;
        iHeader = NULL;
        iMessage = iObserver.NextMessageL();

        // If message is not set, the search is finished.
        // Run down the operation.
        if ( !iMessage )
            {
            iState = EFinished;
            return EFalse;
            }

        // Retrieve the message header also.
        iHeader = iObserver.MessageHeaderL();
        }
    else
        {
        // Stop the loop, cancel should not continue.
        return EFalse;
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::HandleHtmlL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearchOp::HandleHtmlL()
    {
    FUNC_LOG;
    if ( iState == ERunning )
        {
        }
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::SearchBodyL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearchOp::SearchBodyL()
    {
    FUNC_LOG;
    if ( iState == ERunning )
        {
        // Just small amount of code to find the strings from the body.
        CParaFormatLayer* para = CParaFormatLayer::NewL();
        CleanupStack::PushL( para );
        CCharFormatLayer* chars = CCharFormatLayer::NewL();
        CleanupStack::PushL( chars );
        CRichText* txt = CRichText::NewL( para, chars );
        CleanupStack::PushL( txt );

        iMessage->GetBodyTextL(
            iObserver.CurrentMessage().Id(),
            CImEmailMessage::EThisMessageOnly,
            *txt,
            *para,
            *chars );

        RBuf temp;
        temp.CreateL( txt->DocumentLength() );
        CleanupClosePushL( temp );

        txt->Extract( temp );
        SearchFromStringL( temp );

        CleanupStack::PopAndDestroy( &temp );
        CleanupStack::PopAndDestroy( txt );
        CleanupStack::PopAndDestroy( chars );
        CleanupStack::PopAndDestroy( para );
        }
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::SearchFromArrayL()
// ---------------------------------------------------------------------------
//
TBool CIpsPlgSearchOp::SearchFromArrayL( const CDesCArray& aArray )
    {
    FUNC_LOG;
    TBool found( EFalse );
    for ( TInt item(0); iState == ERunning && item < aArray.Count() && !found; item++ )
        {
        found = SearchFromStringL( aArray[item] );
        }
    return found;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::SearchFromStringL()
// ---------------------------------------------------------------------------
//
TBool CIpsPlgSearchOp::SearchFromStringL( const TDesC& aString )
    {
    FUNC_LOG;
    TBool found( EFalse );
    if ( iState == ERunning )
        {
        found = iSearcher->SearchL( aString );
        }
    return found;
    }


// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::SearchL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearchOp::SearchL()
    {
    FUNC_LOG;
    if ( iState == ERunning )
        {
        iSearcher->Cleanup();

        // Mail is fetched, search from entire mail.
        if ( iHeader )
            {
            SearchFromEntireMailL();
            }
        // Mail not fetched, search only from subject.
        else
            {
            SearchFromSubjectL();
            }
        iState = ( iState == EHit ) ? ERunning : iState;
        }
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::HitL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearchOp::HitL(
    const TDesC& /* aSnippet */,
    TInt /* aSnippetCharPos */,
    TBool /* aStartIncomplete */,
    TBool /* aEndIncomplete */ )
    {
    FUNC_LOG;
    iObserver.MatchFoundL();
    iState = EHit;
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::FinalizeL()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearchOp::FinalizeL()
    {
    FUNC_LOG;
    // In case of cancel, the stop should not be indicated.
    if ( iState == EFinished )
        {
        iObserver.SearchFinishedL();
        iState = EStopped;
        }
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::ActivateAndComplete()
// ---------------------------------------------------------------------------
//
void CIpsPlgSearchOp::ActivateAndComplete( )
    {
    FUNC_LOG;
    iStatus = KRequestPending;
// When client wants call to contact the priority must be decreased
// to enable search for contact made with idle priority
    if ( Priority() != iRequiredPriority ) // <cmail>
    	SetPriority(iRequiredPriority);

    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::SearchFromEntireMailL( )
// ---------------------------------------------------------------------------
//
void CIpsPlgSearchOp::SearchFromEntireMailL( )
    {
    FUNC_LOG;
    if ( SearchFromStringL( iHeader->Subject() ) )
        {
        return;
        }
    if ( SearchFromStringL( iHeader->From() ) )
        {
        return;
        }
    if ( SearchFromArrayL( iHeader->ToRecipients() ) )
        {
        return;
        }
    if ( SearchFromArrayL( iHeader->CcRecipients() ) )
        {
        return;
        }
    SearchBodyL();
    }

// ---------------------------------------------------------------------------
// CIpsPlgSearchOp::SearchFromSubjectL( )
// ---------------------------------------------------------------------------
//
void CIpsPlgSearchOp::SearchFromSubjectL( )
    {
    FUNC_LOG;
    RBuf subject;
    CleanupClosePushL( subject );
    iObserver.GetSubjectL( subject );
    TBool found = SearchFromStringL( subject );
    CleanupStack::PopAndDestroy( &subject );

    if ( !found )
        {
        RBuf sender;
        CleanupClosePushL( sender );
        iObserver.GetSenderL( sender );
        found = SearchFromStringL( sender );
        CleanupStack::PopAndDestroy( &sender );
        }
    }

// End of File

