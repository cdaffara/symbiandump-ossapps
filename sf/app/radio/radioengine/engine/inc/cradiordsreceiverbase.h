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

#ifndef CRADIORDSRECEIVERBASE_H
#define CRADIORDSRECEIVERBASE_H

// System includes
#include <RadioRdsUtility.h>

// User includes
#include "mradioenginesettings.h"
#include "mradiordsdataobserver.h"
#include "mradiordsreceiver.h"

// Forward declarations
class CRadioUtility;

// Class declaration
NONSHARABLE_CLASS( CRadioRdsReceiverBase ) : public CBase
                                           , public MRadioRdsReceiver
                                           , public MRadioRdsObserver
    {
public:

    /**
     * The destructor
     */
    virtual ~CRadioRdsReceiverBase();

    /**
     * Adds an observer wich will be notified of the new RDS data
     * If observer already exists, it is not added
     *
     * @param aObserver pointer of the observer instance.
     */
    void AddObserverL( MRadioRdsDataObserver* aObserver );

    /**
     * Removes the rds observer.
     */
    void RemoveObserver( MRadioRdsDataObserver* aObserver );

    /**
     * Getter for automatic switching state of alternate frequencies
     *
     * @return ETrue if automatic switching is enabled
     */
    TBool AutomaticSwitchingEnabled() const;

    /**
     * Getter for the programme service name
     *
     * @return Programme service name
     */
    const TDesC& ProgrammeService() const;

    /**
     * Getter for the radio text.
     *
     * @return Radio text.
     */
    const TDesC& RadioText() const;

    /**
     * Getter for the RDS signal availability
     *
     * @return ETrue if RDS signal is available, otherwise EFalse
     */
    TBool SignalAvailable() const;

    /**
     * Setter for automatic switching of alternate frequencies
     * If this method is overridden, the base implementation should be called
     *
     * @param aEnable ETrue, if automatic swithing is to be enabled
     */
    virtual void SetAutomaticSwitchingL( TBool aEnable );

    /**
     * Clears the RDS information
     */
    void ClearRdsInformation();

protected:

    CRadioRdsReceiverBase( MRadioEngineSettings& aSettings );

    void BaseConstructL();

// from base class MRadioRdsObserver

    void MrroStationSeekByPTYComplete( TInt aError, TInt aFrequency );
    void MrroStationSeekByTAComplete( TInt aError, TInt aFrequency );
    void MrroStationSeekByTPComplete( TInt aError, TInt aFrequency );
    void MrroGetFreqByPTYComplete( TInt aError, RArray<TInt>& aFreqList );
    void MrroGetFreqByTAComplete( TInt aError, RArray<TInt>& aFreqList );
    void MrroGetPSByPTYComplete( TInt aError, RArray<TRdsPSName>& aPsList );
    void MrroGetPSByTAComplete( TInt aError, RArray<TRdsPSName>& aPsList );
    void MrroRdsDataPI( TInt aPi );
    void MrroRdsDataPTY( TRdsProgrammeType aPty );
    void MrroRdsDataPS( TRdsPSName& aPs );
    void MrroRdsDataRT( TRdsRadioText& aRt );
    void MrroRdsDataCT( TDateTime& aCt );
    void MrroRdsDataTA( TBool aTaOn );
    void MrroRdsDataRTplus( TRdsRTplusClass aRtPlusClass, TRdsRadioText& aRtPlusData );
    void MrroRdsSearchBeginAF();
    void MrroRdsSearchEndAF( TInt aError, TInt aFrequency );
    void MrroRdsStationChangeTA( TInt aFrequency );
    void MrroRdsEventAutomaticSwitchingChange( TBool aAuto );
    void MrroRdsEventAutomaticTrafficAnnouncement( TBool aAuto );
    void MrroRdsEventSignalChange( TBool aSignal );

protected: // data

    /**
     * The radio settings
     */
    MRadioEngineSettings&               iSettings;

private: // data

    /**
     * Array of RDS observers.
     */
    RPointerArray<MRadioRdsDataObserver> iObservers;

    /**
     * The programme service name cache.
     * Own.
     */
    HBufC*                              iPsName;

    /**
     * The RDS radio text
     * Own.
     */
    RBuf                                iRadioText;

    /**
     * ETrue if the RDS signal is available
     */
    TBool                               iSignalAvailable;

    /**
     * ETrue if AF is enabled
     */
    TBool                               iAfEnabled;

    };

#endif // CRADIORDSRECEIVERBASE_H
