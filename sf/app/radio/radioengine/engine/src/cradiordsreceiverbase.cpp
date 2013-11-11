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
#include "cradiordsreceiverbase.h"
#include "cradioenginelogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiverBase::CRadioRdsReceiverBase( MRadioEngineSettings& aSettings )
    : iSettings( aSettings )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::BaseConstructL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iPsName = HBufC::NewL( TRdsPSName().MaxLength() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioRdsReceiverBase::~CRadioRdsReceiverBase()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iObservers.Close();
    delete iPsName;
    iRadioText.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::AddObserverL( MRadioRdsDataObserver* aObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt index = iObservers.FindInAddressOrder( aObserver );
    if ( index == KErrNotFound )
        {
        iObservers.InsertInAddressOrderL( aObserver );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::RemoveObserver( MRadioRdsDataObserver* aObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt index = iObservers.FindInAddressOrder( aObserver );

    if ( index >= 0 )
        {
        iObservers.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioRdsReceiverBase::AutomaticSwitchingEnabled() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iAfEnabled;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const TDesC& CRadioRdsReceiverBase::ProgrammeService() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return *iPsName;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const TDesC& CRadioRdsReceiverBase::RadioText() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iRadioText;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioRdsReceiverBase::SignalAvailable() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iSignalAvailable;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::SetAutomaticSwitchingL( TBool aEnable )
    {
    LEVEL3( LOG_METHOD_AUTO );
    iAfEnabled = aEnable;
    for ( TInt i = 0; i < iObservers.Count(); ++i )
        {
        iObservers[i]->RdsAfSearchStateChange( iAfEnabled );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::ClearRdsInformation()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iPsName->Des().Zero();
    iRadioText.Close();
    MrroRdsEventSignalChange( EFalse );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroStationSeekByPTYComplete( TInt DEBUGVAR3( aError ),
                                                          TInt DEBUGVAR3( aFrequency ) )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aError: %d, aFrequency: %d ", aError, aFrequency ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroStationSeekByTAComplete( TInt DEBUGVAR3( aError ),
                                                         TInt DEBUGVAR3( aFrequency ) )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aError: %d, aFrequency: %d ", aError, aFrequency ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroStationSeekByTPComplete( TInt DEBUGVAR3( aError ),
                                                         TInt DEBUGVAR3( aFrequency ) )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aError: %d, aFrequency: %d ", aError, aFrequency ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroGetFreqByPTYComplete( TInt DEBUGVAR3( aError ),
                                                      RArray<TInt>& /*aFreqList*/ )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aError: %d", aError ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroGetFreqByTAComplete( TInt DEBUGVAR3( aError ),
                                                     RArray<TInt>& /*aFreqList*/ )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aError: %d", aError ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroGetPSByPTYComplete( TInt DEBUGVAR3( aError ),
                                                    RArray<TRdsPSName>& /*aPsList*/ )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aError: %d", aError ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroGetPSByTAComplete( TInt DEBUGVAR3( aError ),
                                                   RArray<TRdsPSName>& /*aPsList*/ )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aError: %d", aError ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataPI( TInt aPi )
    {
    LEVEL2( LOG_METHOD_AUTO );
    LEVEL2( LOG_FORMAT( "aPi: %d", aPi ) );
    const TInt observerCount = iObservers.Count();
    const TUint32 frequency = iSettings.TunedFrequency();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsDataPiCode( frequency, aPi );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataPTY( TRdsProgrammeType aPty )
    {
    LEVEL2( LOG_METHOD_AUTO );
    LEVEL2( LOG_FORMAT( "aPty: %d", aPty ) );
    const TInt observerCount = iObservers.Count();
    const TUint32 frequency = iSettings.TunedFrequency();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsDataGenre( frequency, aPty );
        }
    }

// ---------------------------------------------------------------------------
// Notifies of the New Programme Service data
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataPS( TRdsPSName& aPs )
    {
    LEVEL2( LOG_METHOD_AUTO );
    LEVEL2( LOG_FORMAT( "aPs: %S", &aPs ) );
    const TUint32 frequency = iSettings.TunedFrequency();
    iPsName->Des().Copy( aPs );
    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsDataProgrammeService( frequency, *iPsName );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataRT( TRdsRadioText& aRt )
    {
    LEVEL2( LOG_METHOD_AUTO );
    LEVEL2( LOG_FORMAT( "aRt: %S", &aRt ) );
    iRadioText.Close();
    iRadioText.Create( aRt );

    const TUint32 frequency = iSettings.TunedFrequency();
    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsDataRadioText( frequency, iRadioText );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataCT( TDateTime& /*aCt*/ )
    {
    LEVEL2( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataTA( TBool DEBUGVAR2( aTaOn ) )
    {
    LEVEL2( LOG_METHOD_AUTO );
    LEVEL2( LOG_FORMAT( "aTaOn: %d", aTaOn ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsDataRTplus( TRdsRTplusClass aRtPlusClass,
                                               TRdsRadioText& aRtPlusData )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "Class: %d, data: %S", aRtPlusClass, &aRtPlusData );
    const TUint32 frequency = iSettings.TunedFrequency();
    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsDataRadioTextPlus( frequency, aRtPlusClass, aRtPlusData );
        }
    }

// ---------------------------------------------------------------------------
// Notifies of the start of Alternate Frequency search
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsSearchBeginAF()
    {
    LEVEL3( LOG_METHOD_AUTO );
    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsAfSearchBegin();
        }
    }

// ---------------------------------------------------------------------------
// Notifies of the end of Alternate Frequency search
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsSearchEndAF( TInt aError, TInt aFrequency )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aError: %d, aFrequency: %d", aError, aFrequency ) );
    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        // Radio utility uses Hz, UI uses kHz. It's ok to divide with 1000, possible lost fractions are useless.
        iObservers[i]->RdsAfSearchEnd( aFrequency, aError );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsStationChangeTA( TInt DEBUGVAR3( aFrequency ) )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aFrequency: %d", aFrequency ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsEventAutomaticSwitchingChange( TBool DEBUGVAR3( aAuto ) )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aAuto: %d", aAuto ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsEventAutomaticTrafficAnnouncement( TBool DEBUGVAR3( aAuto ) )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aAuto: %d", aAuto ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioRdsReceiverBase::MrroRdsEventSignalChange( TBool aSignal )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aSignal: %d", aSignal ) );
    iSignalAvailable = aSignal;
    const TUint32 frequency = iSettings.TunedFrequency();
    const TInt observerCount = iObservers.Count();
    for ( TInt i = 0; i < observerCount; ++i )
        {
        iObservers[i]->RdsAvailable( frequency, iSignalAvailable );
        }
    }
