/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles version fetching
*
*/


#ifndef SCONVERSIONINFO_H_
#define SCONVERSIONINFO_H_

#include <e32base.h>
#include <sysversioninfo.h>
#include <versioninfo.h>
#include <etelmm.h>
#include <sysutil.h>

const TInt KSconSWVersionSize = 50;
const TInt KSconDateSize = 16;
const TInt KMaxLangSize = 5;

NONSHARABLE_CLASS( CSconVersionInfo ) : public CBase
{
public:
	CSconVersionInfo();
	~CSconVersionInfo();
	
	static CSconVersionInfo* NewL();
	
	/**
	 * 
	 * @param aFs RFs session
	 * @return none.
	 */
	void FetchInfoL( RFs& aFs );
	
	/**
	 *  Is all data ready.
	 *  @return ETrue if FetchInfoL has been called succesfully.
	 */
	TBool IsReady();
	
	/**
	 * Get symbian version. FetchInfoL must be called first
	 * @param aSymbianVersion symbian version
	 * @return error code.
	 */
	TInt GetSymbianVersion( SysVersionInfo::TSymbianOSVersion& aSymbianVersion );
	
	/**
     * Get S60 platform version. FetchInfoL must be called first
     * @param aS60Version S60 platform version
     * @return error code.
     */
    TInt GetS60Version( VersionInfo::TPlatformVersion& aS60Version );
    
public: // data
    
    /**  Contains the manufacturer name. For example "Nokia". */
    HBufC* iManufacturer;

    /**  Contains the phone model sales name. For example "N01". */
    HBufC* iModel;

    /**  Contains the product code name. For example "RM-1". */
    HBufC* iProduct;

    /**  Contains the product revision. For example "01". */
    HBufC* iRevision;
    
    TBuf< KSconDateSize > iDate;
    
    HBufC* iSWVersion;
    
    /** Phone serial number (IMEI or ESN), in character string format. */
    HBufC* iSerialNumber;
    
    TBuf<KMaxLangSize> iLanguage;
    
    HBufC* iSysVersionInfo;
    
    HBufC* iLangVersion;
    
    HBufC* iLangSWVersion;
    
    HBufC* iOPVersion;
    
    TSize iScreenSize;
    
    HBufC* iProductCode;
    
    TInt iDesktopSyncError;
    TInt iDesktopSync;
    
private:
    TBool iInfoFetched; // ETrue if FetchInfoL has been called succesfully.
    
    TInt iSymbianVersionError;
    SysVersionInfo::TSymbianOSVersion iSymbianVersion;
    
    TInt iS60VersionError;
    VersionInfo::TPlatformVersion iS60Version;
    
};

#endif /*SCONVERSIONINFO_H_*/
