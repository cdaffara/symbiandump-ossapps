// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: ObexHarness.inl $
// $Author: Simonk $
// $Revision: 4 $
// $Date: 29/01/02 17:21 $
// 
//


#ifndef __OBEXHARNESS_INL__
#define __OBEXHARNESS_INL__


inline CObexTestUtils& CObexClientTest::ObexTestUtils()
    {
    return iObexTestUtils;
    }

inline CSendAs* CObexClientTest::SendAs()
    {
    return iSendAs;
    }

inline TBool CObexClientTest::CapabilityOK(TUid /*aCapability*/, TInt /*aResponse*/)
    {
    return ETrue;
    }

inline void CObexClientTest::SetProgressGranularity(TInt aGranularity)
    {
    iProgressGranularity = aGranularity;
    }

inline TInt CObexClientTest::ProgressGranularity()
    {
    return iProgressGranularity;
    }

inline void CObexClientTest::SetCancelGranularity(TInt aGranularity)
    {
    iCancelGranularity = aGranularity;
    }

inline TInt CObexClientTest::CancelGranularity()
    {
    return iCancelGranularity;
    }

inline TBool CObexClientTest::IgnoreSendErrors()
    {
    return iIgnoreSendErrors;
    }
 
inline void CObexClientTest::SetIgnoreSendErrors(TBool aFlag)
    {
    iIgnoreSendErrors = aFlag;
    }

inline CObexHeaderList* CObexClientTest::CurrentHeaders()
	{
	return iCurrentHeaders;
	}

inline CObexTestHeaderList* CObexClientTest::VerifyHeaders()
	{
	return iVerifyHeaders;
	}

inline RPointerArray<CObexTestFilenameAndHeaders>& CObexClientTest::Attachments()
	{
	return iAttachmentsList;
	}

#endif // __OBEXHARNESS_INL__

