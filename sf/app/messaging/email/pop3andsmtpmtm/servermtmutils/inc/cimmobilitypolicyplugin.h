// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __CIMMOBILITYPOLICYPLUGIN_H__
#define __CIMMOBILITYPOLICYPLUGIN_H__

#include <e32base.h>
//#include <ecom/ecom.h>
#include <badesca.h>
#include <msvstd.h>

#include "mimmobilitypolicyhandler.h"
#include "es_sock.h" // for TAccessPointInfo
#include "timmobilityaction.h" // for TImMigrateComplete

class MImMobilityPolicyHandler;

/**
ECOM interface definition for implementations of the messaging mobility plugin
policy provider.

@publishedPartner
@released
*/
class CImMobilityPolicyPlugin : public CBase
	{
public:
	static CImMobilityPolicyPlugin* NewL();
	virtual ~CImMobilityPolicyPlugin();

	virtual void PreferredCarrierAvailable(MImMobilityPolicyHandler& aMobilityManager,
	                                       TMsvId aService,
	                                       TUid aMtmUid,
                                           const TDesC8& aProgress,
	                                       TAccessPointInfo aOldIap,
	                                       TAccessPointInfo aNewIap,
	                                       TBool aIsUpgrade,
	                                       TBool aIsSeamless) = 0;
	virtual void Cancel(TMsvId aService) = 0;
	
	virtual void MigrationComplete(TMsvId aService, 
	                               TImMigrateComplete aMigrateCode, 
	                               TAccessPointInfo aNewIap) = 0;

protected:
	inline CImMobilityPolicyPlugin();

private:
	TUid iDtor_ID_Key;
	};

#include "cimmobilitypolicyplugin.inl"

#endif // __CIMMOBILITYPOLICYPLUGIN_H__
