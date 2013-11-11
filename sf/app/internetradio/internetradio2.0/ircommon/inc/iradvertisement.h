/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef IRADVERTISEMENT_H
#define IRADVERTISEMENT_H

#include <e32base.h>

/**
 * This class is being used by xml parser.
 * code is ready to support advertisement
 * not being used in current release
 */
class CIRAdvertisement : public CBase
    {
    public :
        
    /**
     * CIRAdvertisement::NewL()
     * Static function
     * standard two phased constructor
     * @return CIRAdvertisement*
     */
    IMPORT_C static CIRAdvertisement* NewLC();
    
    /**
     * CIRAdvertisement::NewL()
     * Static function
     * standard two phased constructor
     * @return CIRAdvertisement*
     */
    IMPORT_C static CIRAdvertisement* NewL();
    
    /**
     * CIRAdvertisement::ConstructL()
     * standard second phase constructor
     */
    IMPORT_C void ConstructL() const;
    
    /**
     *CIRAdvertisement::~CIRAdvertisement()
     *standard C++ destructor
     */
    ~CIRAdvertisement();
    
    /**
     *CIRAdvertisement::SetAdvInUse()
     *Function to set the advertisement in use
     *@param TDesC
     */
    IMPORT_C void SetAdvInUse( const TDesC& aAdvInUse );
    
    /**
     *CIRAdvertisement::SetUsage()
     *Function to set the url of the global advertisement
     *@param TDesC
     */
	IMPORT_C void SetUsage( const TDesC& aUsageVisibleTimes );
	
    /**
     * CIRAdvertisement::SetVisibleTime()
     * Function to set the duration of the advertisement to be displayed
     * @param TDesC
     */
    IMPORT_C void SetVisibleTime( const TDesC& aVisibleTime );
    
    /**
     * CIRAdvertisement::SetURL()
     * Function to set the url of the global advertisement
     * @param TDesC
     */
    IMPORT_C void SetURL( const TDesC& aUrl );
    
    private :
    // url to fetch the ad from		
    HBufC* iUrl;
    // url of the current ad in use
    HBufC* iAdvInUse; 
    // defines the time for which the ad is visible
    TInt iVisibleTime;
    // no of times the ad to be shown
    TInt iUsageVisibleTimes;
   };
    
#endif//IRADVERTISEMENT_H
