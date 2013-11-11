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

#ifndef MRADIORDSDATAOBSERVER_H
#define MRADIORDSDATAOBSERVER_H

// System includes
#include <e32std.h>

// Class declaration
NONSHARABLE_CLASS( MRadioRdsDataObserver )
    {
public:

    /**
     * Notifies of the status of RDS signal
     */
    virtual void RdsAvailable( TUint32 aFrequency, TBool aAvailable ) = 0;

    /**
     * Notifies of the beginning of Alternate Frequency search
     */
    virtual void RdsAfSearchBegin() = 0;

    /**
     * Notifies of the completed Alternate Frequency search
     *
     * @param aFrequency The new frequency
     * @param aError A standard system error code
     */
    virtual void RdsAfSearchEnd( TUint32 aFrequency, TInt aError ) = 0;

    /**
     * Notifies of the changed Alternate Frequency search state
     *
     * @param aEnabled ETrue if AF search was enabled, otherwise EFalse
     */
    virtual void RdsAfSearchStateChange( TBool aEnabled ) = 0;

    /**
     * Programme Service data available
     *
     * @param aProgramService The new Programme Service data
     */
    virtual void RdsDataProgrammeService( TUint32 aFrequency, const TDesC& aProgramService ) = 0;

    /**
     * Radio text data available
     *
     * @param aRt The new radio text data
     */
    virtual void RdsDataRadioText( TUint32 aFrequency, const TDesC& aRt ) = 0;

    /**
     * Radio text plus data available.
     *
     * @param aRtPlusClass The class of the radio text plus data. Radio text plus classes are
     *                     specified in TRdsRTplusClass enumeration.
     * @param aRtPlusData The new radio text plus data.
     */
    virtual void RdsDataRadioTextPlus( TUint32 aFrequency, const TInt aRtPlusClass, const TDesC& aRtPlusData ) = 0;

    /**
     * Radio genre available
     */
    virtual void RdsDataGenre( TUint32 aFrequency, const TInt aGenre ) = 0;

    /**
     * Radio PI code available
     */
    virtual void RdsDataPiCode( TUint32 aFrequency, const TInt aPiCode ) = 0;

    };

#endif // MRADIORDSDATAOBSERVER_H
