/*
* Copyright (c) 2010 Kanrikogaku Kenkyusho, Ltd.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Kanrikogaku Kenkyusho, Ltd. - Initial contribution
*
* Contributors:
*
* Description:
* {Description of the file}
*
*/

// Protection against nested includes
#ifndef __DIRECTPRINTBEARERMGR_H__
#define __DIRECTPRINTBEARERMGR_H__

// System includes
#include <e32base.h>
 
// User includes
#include "directprintbearermngobserver.h"
#include "directprintclient.h"
#include "directprintjobguarddata.h"

// Forward declarations
class CDirectPrintModel;
class CDirectPrintPrintSettings;

// Class declaration
/**
 *  Bearer manager class
 *  more_complete_description
 */
class CDirectPrintBearerMgr : public CActive
	{
public:
	/** Status of bearer manager */
	enum TBearerMngStatus
		{
		EBearerMngNotConnected = 0,	// (0)
		EBearerMngConnected,		// (1)
		EBearerMngCreatingJob,		// (2)
		EBearerMngPrinting,			// (3)
		EBearerMngClosing,			// (4)
		EBearerMngCanceling,		// (5)
		};

public:
	/** Constructors */
	static CDirectPrintBearerMgr* NewL(MDirectPrintBearerMngObserver* aObserver,
										CDirectPrintModel& aModel);
	static CDirectPrintBearerMgr* NewLC(MDirectPrintBearerMngObserver* aObserver,
										CDirectPrintModel& aModel);
	/** Destructor */
	~CDirectPrintBearerMgr();

public:
	/**
	 * Create print job.
	 */
	void CreateJobL();
	/**
	 * Submit print job.
	 */
	void SubmitPrintJobL();
	/**
	 * Close session.
	 */
	void CloseSession();

	/**
	 * Get status of bearer manager.
	 *
	 * @return Status of bearer manager
	 */
	TBearerMngStatus BearerStatus();

	TInt SupportedProtocols();

	TInt GetProtocolNames(RSelectItemArray& aNames);

	void InitCapabilitiesL(TInt aIndex);

	TInt GetJobSetting(TInt aCapabilityID, TInt& aValue);

	TInt SetJobSetting(TInt aCapabilityID, TInt aValue, TInt& aAffectedCapability);

	void SetJobSettings();

	CDirectPrintPrintSettings* PrinterSettingsL();

protected:
	CDirectPrintBearerMgr(MDirectPrintBearerMngObserver* aObserver,
							CDirectPrintModel& aModel);
	void ConstructL();

private: // from CActive

	void RunL();
	void DoCancel();

private:
	void SetBearerStatus(TBearerMngStatus aStatus);

	TInt CreatePrinterUid(TInt aIndex);

private:
	/** Pointer to bearer manager observer. */
	MDirectPrintBearerMngObserver* iObserver;
	/** Bearer object. */
	RDirectPrintClient iBearer;
	/** Job guard data. */
	TDirectPrintJobGuardData iJobGuardData;
	/** Reference to model class. */
	CDirectPrintModel& iModel;

	/** Status of bearer manager. */
	TBearerMngStatus iBearerStatus;

	/** Printer settings. */
	CDirectPrintPrintSettings* iPrintSettings;
	};

#endif // __DIRECTPRINTBEARERMGR_H__
