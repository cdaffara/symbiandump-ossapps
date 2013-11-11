// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// TImSmtpProgress() inline functions
// 
//



inline TMsgImOutboxSendState TImSmtpProgress::Status() const
	{
	return iStatus;	
	}

inline void TImSmtpProgress::SetStatus(TMsgImOutboxSendState aStatus)
	{
	iStatus = aStatus;
	}


inline void TImSmtpProgress::SetError(TInt anError)
	{
	iError=anError;
	}

inline TInt TImSmtpProgress::Error() const
	{
	return iError;
	}


inline TInt TImSmtpProgress::MsgNo() const
	{
	return iMsgNo;
	}

inline void TImSmtpProgress::SetMsgNo(TInt aMsgNo)
	{
	iMsgNo = aMsgNo;
	}


inline TInt TImSmtpProgress::Sent() const
	{
	return iSent;	
	}


inline TInt TImSmtpProgress::NotSent() const
	{
	return iNotSent;	
	}


inline TInt TImSmtpProgress::FailedToSend() const
	{
	return iFailedToSend;	
	}


inline TInt TImSmtpProgress::SendTotal() const
	{
	return iSendTotal;	
	}


inline void TImSmtpProgress::InitialiseTotal(const TInt& aTotal)
	{
	iNotSent=aTotal;
	iSendTotal=aTotal;
	iSent=0;
	iFailedToSend=0;
	iError=0;
	iMsgNo=0;
	iSendFileProgress.iBytesSent=0;
	iSendFileProgress.iBytesToSend=0;
	iSendFileProgress.iSessionState=EConnectingToSmtp;
	iStatus=EMsgOutboxProgressWaiting;
	}


inline void TImSmtpProgress::UpdateSent()
	{
	iNotSent--;
	iSent++;
	}


inline void TImSmtpProgress::UpdateFailedToSend()
	{
	iNotSent--;
	iFailedToSend++;	// only changed if message was couldn't be sent
	}

inline void TImSmtpProgress::DecrementSendTotal()
	{
	// used to lower send total when I discover one message in collection 
	// which has been deleted before I had a chance to send it.
	iSendTotal--;
	iNotSent--;
	}

