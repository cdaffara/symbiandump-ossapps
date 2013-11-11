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
// $Workfile: SendAsStates.h $
// $Author: Simonk $
// $Revision: 1 $
// $Date: 28/01/02 17:02 $
// 
//

#ifndef __OBEXSETOBEXRECIPIENTSTATES_H__
#define __OBEXSETOBEXRECIPIENTSTATES_H__

#include "harness.h"        //MBaseTestState

//forward declarations

class CObexClientTest;

class CSetObexMessageRecipientState : public CBase, public MBaseTestState
/*
 *  A state class, to be added to a harness,  that encapsulates the addition of a recipient 
 *  to a message through the message server
 */
    {
    public:
        CSetObexMessageRecipientState(const TDesC& aRecipient, CObexClientTest& aClientTest);
	    void StartL(TRequestStatus& aStatus);
    private:
	    CObexClientTest& iClientTest;
        TBuf<256> iRecipient;       //TODO:-Change this attribute to a HBufC or find out what the max size of this attrib is
    };

class CSetObexMessageBTRecipientState : public CBase, public MBaseTestState
/*
 *  A state class, to be added to a harness,  that encapsulates the addition of a recipient 
 *  to a message through the message server
 */
    {
    public:
        CSetObexMessageBTRecipientState(const TDesC& aRecipient, CObexClientTest& aClientTest);
	    void StartL(TRequestStatus& aStatus);
    private:
	    CObexClientTest& iClientTest;
        TBuf<3> iRecipient;       // A Bluetooth address is 6 bytes, 3 unicode chars.
    };


#endif //__OBEXSENDASSTATES_H__
