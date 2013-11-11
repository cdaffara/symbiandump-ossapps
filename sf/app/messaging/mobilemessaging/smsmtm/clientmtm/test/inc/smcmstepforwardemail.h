// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SMCMSTEPFORWARDEMAIL_H__
#define __SMCMSTEPFORWARDEMAIL_H__

#include "smcmstepbase.h"

class CSMCMStepForwardEmail : public CSMCMStepBase
	{
public:
	CSMCMStepForwardEmail( );
	virtual ~CSMCMStepForwardEmail( );
	virtual TVerdict doTestStepL( );
	TVerdict DoTestForwardEmailL( const TDesC& aEmailMessageData, 
								  const TDesC& aSubject, 
								  const TDesC& aBody );
	TBool TestForwardBodyL( const CRichText& aBody, const TDesC& aExpected ) const;
	};

_LIT( KSMCMStepForwardEmail, "SMCMStepForwardEmail" );	


#endif //__SMCMSTEPFORWARDEMAIL_H__