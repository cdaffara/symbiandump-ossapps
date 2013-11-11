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

#ifndef __SMCMSTEPREPLYEMAIL_H__
#define __SMCMSTEPREPLYEMAIL_H__

#include "smcmstepbase.h"

class CSMCMStepReplyEmail : public CSMCMStepBase
	{
public:
	CSMCMStepReplyEmail( );
	virtual ~CSMCMStepReplyEmail( );
	virtual TVerdict doTestStepL( );
	TVerdict DoTestReplyEmailL( const TDesC& aEmailMessageData, 
								const TDesC& aAddress, 
								const TDesC& aSubject );
	};

_LIT( KSMCMStepReplyEmail, "SMCMStepReplyEmail" );	

#endif // __SMCMSTEPREPLYEMAIL_H__