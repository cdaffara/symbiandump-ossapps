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
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <cphcltussdint.h>

#include "ussdcomms.h"
#include "tflogger.h"

// The maximum length of editor lines.
const TInt KUssdEditorMaxLength = 182;

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// UssdComms::UssdComms
// Constructor.
// -----------------------------------------------------------------------------
//
UssdComms::UssdComms(QObject *parent)
    : QObject(parent),
      mUssd(NULL), 
      mErrorCode(static_cast<int>(EPhCltExitReasonUnknown))
{
    // Default value - ETrue
    TRAP_IGNORE(mUssd = CPhCltUssdInt::NewL());
    TFLOGSTRING("USSDEDITOR: UssdComms::UssdComms")
}

// -----------------------------------------------------------------------------
// UssdComms::~UssdComms
// ~UssdComms.
// -----------------------------------------------------------------------------
//
UssdComms::~UssdComms()
{
    if (mUssd){
        mErrorCode = mUssd->AppTerminating(
            static_cast<TPhCltUssdAppExitReason>(mErrorCode));
        delete mUssd;
        mUssd = NULL;
        TFLOGSTRING2("USSDEDITOR: UssdComms::~UssdComms %d", mErrorCode)
    }
    TFLOGSTRING("USSDEDITOR: UssdComms::~UssdComms")
}

// -----------------------------------------------------------------------------
// UssdComms::appStarting
// appStarting.
// -----------------------------------------------------------------------------
//
void UssdComms::appStarting()
{
    mErrorCode = mUssd->AppStarting();
    TFLOGSTRING("USSDEDITOR: UssdComms::appStarting")
}

// -----------------------------------------------------------------------------
// UssdComms::appTerminating
// appTerminating.
// -----------------------------------------------------------------------------
//
void UssdComms::appTerminating()
{
    mErrorCode = mUssd->AppTerminating(EPhCltUserExit);
    TFLOGSTRING("USSDEDITOR: UssdComms::appTerminating")
}

// -----------------------------------------------------------------------------
// UssdComms::appToBackground
// appToBackground.
// -----------------------------------------------------------------------------
//
void UssdComms::appToBackground()
{
    mErrorCode = mUssd->AppToBackground();
    TFLOGSTRING("USSDEDITOR: UssdComms::appToBackground")
}

// -----------------------------------------------------------------------------
// UssdComms::appToForeground
// appToForeground.
// -----------------------------------------------------------------------------
//
void UssdComms::appToForeground()
{
    mErrorCode = mUssd->AppToForeground();
    TFLOGSTRING("USSDEDITOR: UssdComms::appToForeground")
}

// -----------------------------------------------------------------------------
// UssdComms::send
// send.
// -----------------------------------------------------------------------------
//
int UssdComms::send( const QString& message )
{
    TBuf<KUssdEditorMaxLength> buf16;
    buf16.Copy((const TUint16*)message.constData(), message.length());
    mErrorCode = mUssd->SendUssd(buf16);
    TFLOGSTRING("USSDEDITOR: UssdComms::send")
    return mErrorCode;
}

// -----------------------------------------------------------------------------
// UssdComms::cancel
// cancel.
// -----------------------------------------------------------------------------
//
void UssdComms::cancel()
{
    mUssd->SendUssdCancel();
    TFLOGSTRING("USSDEDITOR: UssdComms::cancel")
}

// -----------------------------------------------------------------------------
// UssdComms::errorCode
// errorCode.
// -----------------------------------------------------------------------------
//
int UssdComms::errorCode()
{
    TFLOGSTRING("USSDEDITOR: UssdComms::send")
    return mErrorCode;
}

// -----------------------------------------------------------------------------
// UssdComms::informExitReason
// informExitReason.
// -----------------------------------------------------------------------------
//
void UssdComms::informExitReason(int aExitReason)
{
    TFLOGSTRING2("USSDEDITOR: UssdComms::informExitReason \
        %d", aExitReason) 
    mErrorCode = aExitReason;
}

// End of file
