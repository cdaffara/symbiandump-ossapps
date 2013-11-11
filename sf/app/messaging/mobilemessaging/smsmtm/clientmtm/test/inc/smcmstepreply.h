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

#ifndef __SMCMSTEPREPLY_H__
#define __SMCMSTEPREPLY_H__

#include "smcmstepbase.h"

class CSMCMStepReply : public CSMCMStepBase
	{
public:
	CSMCMStepReply( );
	virtual ~CSMCMStepReply( );
	virtual TVerdict doTestStepL( );
	};

_LIT( KSMCMStepReply, "SMCMStepReply" );	

#endif // __SMCMSTEPREPLY_H__