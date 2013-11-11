/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Provides Solar Term dates for Lunar Calendar. Reads and caches
*                binary data file that lists solar term dates. 
*
*/



#ifndef C_CALENSOLARTERMS_H
#define C_CALENSOLARTERMS_H

#include <badesca.h>
#include <e32base.h>

class RFs;

const TInt KSolarTermCount(24);


/**
 * Provides Solar term dates for Lunar Calendar. 
 *
 * Solar term dates are listed in special binary format file. 
 * CCalenSolarTerms reads and caches this file one year at a time.
 *
 *  @lib CalenLunar.dll
 *  @since S60 v3.1
 */
NONSHARABLE_CLASS( CCalenSolarTerms ) : public CBase
    {
public:
    IMPORT_C static CCalenSolarTerms* NewL(RFs& aFs);

    IMPORT_C virtual ~CCalenSolarTerms();

    /**
     * Checks if date is solar term date. If it is, returns index to particular solar term, otherwise returns error code
     * Index can be then passed to GetSolarTermNameL function. 
     *
     * @since S60 v3.1
     * @param aDateTime date that is checked 
     * @return index of solar term, if date is solar term date.
     *         KErrNone, if date is not solar term date
     *         KErrNotSupported, if date is outside of supported 
     *                           range for solar term data.
     */
    IMPORT_C TInt CheckSolarTermDateL( const TDateTime& aDateTime ); 


private:
    CCalenSolarTerms(RFs& aFs);

    void ConstructL();

    /** 
     * Returns ETrue, if date fits into date range of available solar item 
     * data.
     * @since 3.0
     */
    TBool HasSolarTermDataAvailable(const TDateTime& aDate) const;

    /**
     * Reads and caches 24 solar term dates from "SolarItems" file
     */
    void ReadSolarTermsL(TDateTime aDate);

private: // data
    /**
     * File server handle.
     */
    RFs& iFs;

    /**
     * Year that is currently cached from file to memory
     */
    TInt iCachedYear;

    /**
     * Table of solar term dates for currently cached year
     */
    TDateTime iSolarTermDates[KSolarTermCount];

    };

#endif // C_CALENSOLARTERMS_H
