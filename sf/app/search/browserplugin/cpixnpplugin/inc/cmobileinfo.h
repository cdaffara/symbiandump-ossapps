/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MOBILE_INFO_H__
#define __MOBILE_INFO_H__

#include <e32base.h>
#include <Etel3rdParty.h>
 
/*
 * Helper class. Fetches Mobile Country Code information in order to pass it to widget to server Nokia Ovi Store. 
 * Can be expanded with other Information functions, like fetching IMEI, Product Code etc.
 */

class CMobileInfo : public CActive
	{
	private:
		CMobileInfo(TDes& aMCC);
		virtual ~CMobileInfo();

		void ConstructL();
	public:	
		// Static function, to be called to get information
		static void GetInfoL(TDes& aMCC);
	
	private: // From CActive
		void RunL();
		void DoCancel();

	private: // member variables
		TDes& iMCC;
		CActiveSchedulerWait* iWait;

		CTelephony* iTelephony;
		CTelephony::TNetworkInfoV1 iMCCMNCV1;
		CTelephony::TNetworkInfoV1Pckg iMCCMNCV1Pkg;
	};

#endif
