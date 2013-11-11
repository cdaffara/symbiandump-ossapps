/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cradioenginelogger.h"

#include "cradiopropertyobserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRadioPropertyObserver::CRadioPropertyObserver( MRadioPropertyChangeObserver& aObserver,
                                                const TUid& aCategory,
                                                const TUint aKey,
                                                const TRadioPropertyType aPropertyType )
    : CActive( CActive::EPriorityStandard )
    , iObserver( aObserver )
    , iCategory( aCategory )
    , iKey( aKey )
    , iPropertyType( aPropertyType )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRadioPropertyObserver::ConstructL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    switch ( iPropertyType )
        {
        case ERadioPropertyInt:
            {
            break;
            }
        case ERadioPropertyByteArray:
            {
            iValueByteArray = HBufC8::NewL( RProperty::KMaxPropertySize );
            break;
            }
        case ERadioPropertyText:
            {
            // Max size in bytes, length is size / 2
            iValueText = HBufC::NewL( RProperty::KMaxPropertySize / 2 );
            break;
            }
        default:
            {
            break;
            }
        }

    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CRadioPropertyObserver* CRadioPropertyObserver::NewL( MRadioPropertyChangeObserver& aObserver,
                                                               const TUid& aCategory,
                                                               const TUint aKey,
                                                               const TRadioPropertyType aPropertyType )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioPropertyObserver* self = new( ELeave )CRadioPropertyObserver( aObserver,
                                            aCategory, aKey, aPropertyType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRadioPropertyObserver::~CRadioPropertyObserver()
    {
    LEVEL3( LOG_METHOD_AUTO );
    Cancel();
    iProperty.Close();
    delete iValueByteArray;
    delete iValueText;
    }

// ---------------------------------------------------------------------------
// Subscribes to a property and reads the value, if not already active.
// ---------------------------------------------------------------------------
//
EXPORT_C void CRadioPropertyObserver::ActivateL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    if ( !IsActive() )
        {
        RunL();
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRadioPropertyObserver::RunL()
    {
    LEVEL3( LOG_METHOD_AUTO );

    iProperty.Subscribe( iStatus );
    SetActive();

    TInt err( KErrNone );

    switch ( iPropertyType )
        {
        case ERadioPropertyInt:
            {
            err = iProperty.Get( iValueInt );
            if ( !err )
                {
                iObserver.HandlePropertyChangeL( iCategory, iKey, iValueInt );
                }
            break;
            }
        case ERadioPropertyByteArray:
            {
            TPtr8 ptr8( iValueByteArray->Des() );
            err = iProperty.Get( ptr8 );
            if ( !err )
                {
                iObserver.HandlePropertyChangeL( iCategory, iKey, *iValueByteArray );
                }
            break;
            }
        case ERadioPropertyText:
            {
            TPtr ptr( iValueText->Des() );
            err = iProperty.Get( ptr );
            if ( !err )
                {
                iObserver.HandlePropertyChangeL( iCategory, iKey, *iValueText );
                }
            break;
            }
        default:
            {
            break;
            }
        }

    if ( err )
        {
        iObserver.HandlePropertyChangeErrorL( iCategory, iKey, err );
        }
    }

// -----------------------------------------------------------------------------
// Cancels an outstanding active request
// -----------------------------------------------------------------------------
//
void CRadioPropertyObserver::DoCancel()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iProperty.Cancel();
    }

// -----------------------------------------------------------------------------
// Getter for integer value
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CRadioPropertyObserver::ValueInt() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iValueInt;
    }

// -----------------------------------------------------------------------------
// Getter for byte array value
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CRadioPropertyObserver::ValueDes8() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return *iValueByteArray;
    }

// -----------------------------------------------------------------------------
// Getter for text value
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CRadioPropertyObserver::ValueDes() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return *iValueText;
    }
