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
#ifndef C_CIRSETTINGS_H
#define C_CIRSETTINGS_H

#include <f32file.h>

class CRepository;

// CLEANUP REQUIRED

const TInt KMaxLengthIRAppVersion = 50;
const TInt KMaxLengthIrId = 32;
const TInt KMaxLengthSilencePeriod = 32;
const TInt KMaxLengthUrl = 500;
const TInt KMaxLengthFileName = 500;

// END CLEANUP

NONSHARABLE_CLASS( CIRSettings ) : public CObject
    {

public:

    IMPORT_C static CIRSettings* OpenL();
    ~CIRSettings();

    IMPORT_C TBool IsFlagTermsAndConditionsL();
    IMPORT_C void SetFlagTermsAndConditionsL();
    IMPORT_C void ReSetFlagTermsAndConditionsL();
    IMPORT_C const TDesC& GetIrappVersionL() const;
    IMPORT_C void SetIRIDL(const TDesC& aIrid);
    IMPORT_C const TDesC& GetIRIDL() const;
    IMPORT_C const  TDesC& GetISDSBaseUrlL() const;
    IMPORT_C void SetISDSBaseUrlL(const TDesC& aIsdsUrl);
    IMPORT_C TInt GetVolumeSetting() const;
    IMPORT_C void SetVolumeSettingL(TInt aPlayVolume);
    IMPORT_C TBool IsUpdateAvailableL();
    IMPORT_C void SetUpdateAvailableL();
	IMPORT_C void ReSetUpdateAvailableL();
    IMPORT_C TInt GetLogRunningNo() const;
    IMPORT_C void SetLogRunningNoL();
    IMPORT_C void SetTimeOutL(TInt aTimeOut);
    IMPORT_C TInt GetTimeOut() const;
    IMPORT_C void SetSilencePeriodL(TUint aSilencePeriod);
    IMPORT_C TUint GetSilencePeriodL() const;
    IMPORT_C void SetSilencePeriodStartL(const TDesC& aSilencePeriodStart);
    IMPORT_C const TDesC& GetSilencePeriodStartL() const;
    IMPORT_C void SetPreferredQualityL(TInt aQualityValue);
    IMPORT_C TInt GetPreferredQuality() const; 
    IMPORT_C void SetSongHisotryShowL(TInt aShowFlag);
    IMPORT_C TInt GetSongHistoryShow() const;
    IMPORT_C void SetMaxPresetCountL(TInt aMaxCount);
    IMPORT_C TInt MaxPresetCount();
    IMPORT_C void SetTimeCorrectionL(TInt aCorrection);
    IMPORT_C TInt TimeCorrection() const;
    IMPORT_C const TDesC& PrivatePath() const;

	// Methods for starting view
    /**
     * SetStartingViewIdL()
     * Sets the starting view Id in cenrep
     */
    IMPORT_C void SetStartingViewIdL(TUint32 aStartingViewId);
   
    /**
     * GetStartingViewIdL()
     * Gets the starting view Id from cenrep
     */
    IMPORT_C TUint32 GetStartingViewIdL() const;
   
	/**
	 * GetGlobalAdvFlagL()
	 * Gets the global advertisement flag from cenrep
	 */
	IMPORT_C TBool GetGlobalAdvFlagL();

	/**
	 * GetManuallyInputtedStationUrlL()
	 * Gets the url inputted in Go to station view from cenrep
	 */
    IMPORT_C const  TDesC& GetManuallyInputtedStationUrlL() const;
    /**
     * SetManuallyInputtedStationUrlL()
     * Sets the url in Go to station view from cenrep
     */
    IMPORT_C void SetManuallyInputtedStationUrlL(const TDesC& aUrl);

    /**
     * GetManuallyInputtedStationNameL()
     * Gets the name inputted in Go to station view from cenrep
     */
    IMPORT_C const  TDesC& GetManuallyInputtedStationNameL() const;
    /**
     * SetManuallyInputtedStationNameL()
     * Sets the name in Go to station view from cenrep
     */
    IMPORT_C void SetManuallyInputtedStationNameL(const TDesC& aName);
private:

    CIRSettings();
    void ConstructL();

private:

	CRepository *iRepository;
    RFs iFs;

	//key KIrappVersion
    TBuf<KMaxLengthIRAppVersion> iIrappVersion;//! irapp version number, 
    //key KIRID
    TBuf<KMaxLengthIrId> iIRID;//! ir-id supplied by isds. cannot change
    // key KSilencePeriodStartTime
    TBuf<KMaxLengthSilencePeriod> iSilencePeriodStartTime; //!Universal timestamp for the time of decline of an Update
    //key KISDSBaseUrl
    TBuf<KMaxLengthUrl> iISDSBaseUrl;//!the base url of isds
    TBuf<KMaxLengthFileName> iPrivatePath;
    TBuf<KMaxLengthUrl> iStationUrl;
    TBuf<KMaxLengthFileName> iStationName;
    };
    
#endif // C_CIRSETTINGS_H
