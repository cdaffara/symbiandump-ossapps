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

// User includes
#include "radiordslistener.h"
#include "radiostationhandlerif.h"
#include "radioenginewrapper_p.h"
#include "radioenginewrapperobserver.h"
#include "radiologger.h"

/*!
 * Converts a symbian descriptor to Qt string
 */
static QString convertString( const TDesC& aDesc )
{
    return QString::fromUtf16( aDesc.Ptr(), aDesc.Length() );
}

/*!
 *
 */
RadioRdsListener::RadioRdsListener( RadioStationHandlerIf& stationHandler, RadioEngineWrapperPrivate& wrapper ) :
    mStationHandler( stationHandler ),
    mWrapper( wrapper )
{
}

/*!
 * \reimp
 */
void RadioRdsListener::RdsAvailable( TUint32 /*aFrequency*/, TBool aAvailable )
{
    RUN_NOTIFY_LOOP( mWrapper.observers(), rdsAvailabilityChanged( aAvailable ) );
}

/*!
 * \reimp
 */
void RadioRdsListener::RdsAfSearchBegin()
{
}

/*!
 * \reimp
 */
void RadioRdsListener::RdsAfSearchEnd( TUint32 /*aFrequency*/, TInt /*aError*/ )
{
}

/*!
 * \reimp
 */
void RadioRdsListener::RdsAfSearchStateChange( TBool /*aEnabled*/ )
{
}

/*!
 * \reimp
 */
void RadioRdsListener::RdsDataProgrammeService( TUint32 aFrequency, const TDesC& aProgramService )
{
    if ( aProgramService.Length() > 0 ) {
        LOG_TIMESTAMP( "RdsName changed" );
        const uint frequency = static_cast<uint>( aFrequency );
        mStationHandler.setCurrentPsName( frequency, convertString( aProgramService ) );

#ifdef SHOW_CALLSIGN_IN_ANY_REGION
        RdsDataPiCode( frequency, 0x3BB7 ); //KQOL-FM - Las Vegas, NV
#endif
    }
}

/*!
 * \reimp
 */
void RadioRdsListener::RdsDataRadioText( TUint32 aFrequency, const TDesC& aRadioText )
{
    if ( aRadioText.Length() > 0 ) {
        const uint frequency = static_cast<uint>( aFrequency );
        mStationHandler.setCurrentRadioText( frequency, convertString( aRadioText ) );
    }
}

/*!
 * \reimp
 */
void RadioRdsListener::RdsDataRadioTextPlus( TUint32 aFrequency,
                                             const TInt aRadioTextPlusClass,
                                             const TDesC& aRadioText )
{
    TInt rtClass = aRadioTextPlusClass;
    if ( aRadioTextPlusClass == RtPlus::Band ) {
        rtClass = RtPlus::Artist;
    }

    const uint frequency = static_cast<uint>( aFrequency );
    mStationHandler.setCurrentRadioTextPlus( frequency, rtClass, convertString( aRadioText ) );
}

/*!
 * \reimp
 */
void RadioRdsListener::RdsDataGenre( TUint32 aFrequency, const TInt aGenre )
{
    LOG_FORMAT( "RadioRdsListener::RdsDataGenre: Genre: %d", aGenre );
    const uint frequency = static_cast<uint>( aFrequency );
    mStationHandler.setCurrentGenre( frequency, aGenre );
}

/*!
 * \reimp
 */
void RadioRdsListener::RdsDataPiCode( TUint32 aFrequency, const TInt aPiCode )
{
    LOG_FORMAT( "RadioRdsListener::RdsDataPiCode, PI: %d", aPiCode );
    const uint frequency = static_cast<uint>( aFrequency );
    mStationHandler.setCurrentPiCode( frequency, aPiCode );
}
