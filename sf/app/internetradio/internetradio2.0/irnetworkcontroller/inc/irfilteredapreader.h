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
* Description:  Used to read in only access points with the desired characteristics. 
*
*/


#ifndef C_IRFILTEREDAPREADER_H
#define C_IRFILTEREDAPREADER_H

#include <commsdattypesv1_1.h>

using namespace CommsDat;


/**
 *  Reads the access points for Network Controller component.
 * 
 *  Returns only those entries from IAP table that are referenced from 
 *  WAP_ACCESS_POINT table.
 * 
 *  In addition to that an instance of class performs filtering based on
 *  configured criteria.   
  *
 */
NONSHARABLE_CLASS( CIRFilteredApReader ): public CBase
    {
public:
    // The filtering flags
    static const TInt KIRFilterNone         = 0x0;
    static const TInt KIRFilterWAPOnly      = 0x1;
    static const TInt KIRFilterWAPMandatory = 0x2;
    static const TInt KIRFilterEasyWLAN     = 0x4;
    
public:
    /**
     * Two-phased constructor.
     * @param aFilterFlags Defines the filtering criteria applied to entries
     * @return Instance of CIRFilteredApReader
     */
    static CIRFilteredApReader* NewL( TInt aFilterFlags );

    /**
     * Two-phased constructor.
     * @param aFilterFlags Defines the filtering criteria applied to entries
     * @return Instance of CIRFilteredApReader
     */
    static CIRFilteredApReader* NewLC( TInt aFilterFlags );

    /**
     * Gets the first appropriate IAP record.
     * Initialises the required objects for iteration.
     * 
     * Only those records that have the desired characteristics are returned.
     *
     * @return The suitable record, if found. NULL, if no record was found
     */
    CCDIAPRecord* FirstRecordL();    

    /**
     * Gets the next appropriate IAP record.
     * Only those records that have the desired characteristics are returned.
     *
     * @return The suitable record, if found. NULL, if no record was found
     */
    CCDIAPRecord* NextRecordL();

private:
    /**
     * C++ Default constructor.
     * 
     * @param aFilterFlags Defines the filtering criteria applied to entries 
     */
    CIRFilteredApReader( TInt aFilterFlags );

    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * Destructor.
     */
    ~CIRFilteredApReader();
    
    /**
     * Returns the current record.
     * 
     * @return The current record.
     */
    CCDWAPAccessPointRecord* CurrentRecordL() const;

    /**
     * Moves the iterator to next record in WAPAccessPoint table.
     * 
     * @return ETrue if there was more records, EFalse if not.
     */
    TBool MoveToNextRecord();

    /**
     * Loads items from WAPIPBearer and IAP tables that are
     * referenced by the current record.
     */
    TBool LoadCurrentApDetailsL();
    
    /**
     * Performs the filtering.
     * 
     * Applies the different filtering criteria according to configured
     * policy (flags).
     *
     * @return ETrue if the record was let through, EFalse if the record was filtered out.
     */
    TBool FilterL() const;

    /**
     * Filters the access point entries that have the mandatory WAP parameters.
     *
     * Currently only GatewayAddress of WAPIPBearer is checked.
     * 
     * Examining the StartPage field of WAPAccessPoint table proved to be 
     * too strict filter.  
     * 
     * @return ETrue if the record was let through, EFalse if the record was filtered out.
     */
    TBool FilterWAPMandatoryL() const;
    
    /**
     * Filters the access point entries that use IAPService, whose type is WAPOnly.
     * 
     * @return ETrue if the record was let through, EFalse if the record was filtered out.
     */
    TBool FilterWAPOnlyL() const;

    /**
     * Filters the access point entries whose name is "EasyWlan".
     * 
     * This filtering should be done in more clever way.
     * 
     * @return ETrue if the record was let through, EFalse if the record was filtered out.
     */
    TBool FilterEasyWlanL() const;

private: // data

    /**
     * The database accessor.
     * Owned.
     */
    CMDBSession* iDbSession;
    
    /**
     * All the records in WAPAccessPoint table. 
     * Owned.
     */
    CMDBRecordSet<CCDWAPAccessPointRecord>* iWapTable;
    
    /**
     * Index of the current record.
     */
    TInt iWapTableIndex;
    
    /**
     * WAPIPBearer entry associated with current record.
     * Owned.
     */
    CCDWAPIPBearerRecord* iWapIpBearerRecord;

    /**
     * IAP record associated with current record.
     * Owned.
     */
    CCDIAPRecord* iIapRecord;
    
    /**
     * Flags that define the applied filtering criteria.
     */
    TInt iFilterFlags;
    };

#endif // C_IRFILTEREDAPREADER_H
