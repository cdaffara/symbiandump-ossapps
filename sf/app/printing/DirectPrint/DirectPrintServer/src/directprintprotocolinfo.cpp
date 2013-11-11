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
*
*/

#include "directprintprotocolinfo.h"

CDirectPrintProtocolInfo::CDirectPrintProtocolInfo()
	{
	}

CDirectPrintProtocolInfo::~CDirectPrintProtocolInfo()
	{
	delete iDisplayName;
	}

CDirectPrintProtocolInfo* CDirectPrintProtocolInfo::NewLC(const CImplementationInformation& aImplInfo)
	{
	CDirectPrintProtocolInfo* self = new (ELeave) CDirectPrintProtocolInfo();
	CleanupStack::PushL(self);

	self->iImplementationUid = aImplInfo.ImplementationUid();

	if(aImplInfo.DisplayName().Length() != 0)
		{
		self->iDisplayName = aImplInfo.DisplayName().AllocL();
		}

	return self;
	}

TUid CDirectPrintProtocolInfo::ImplementationUid() const
	{
	return iImplementationUid;
	}

const TDesC& CDirectPrintProtocolInfo::DisplayName() const
	{
	if(iDisplayName == NULL)
		return KNullDesC();
	else
		return *iDisplayName;
	}

