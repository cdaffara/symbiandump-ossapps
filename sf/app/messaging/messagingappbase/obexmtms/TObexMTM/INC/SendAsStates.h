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

#ifndef __OBEXSENDASSTATES_H__
#define __OBEXSENDASSTATES_H__

#include "harness.h"        //MBaseTestState

//forward declarations

class CObexClientTest;

class CObexSendAsCreateState : public CBase, public MBaseTestState
/*
 *  A state class, to be added to a harness, that encapsulates the creation of a SendAs
 *  object and a corresponding message
 */
    {
    public:
        CObexSendAsCreateState(CObexClientTest& aClientTest);
	    void StartL(TRequestStatus& aStatus);
    private:
	    CObexClientTest& iClientTest;
    };

class CObexSendAsAddRecipientState : public CBase, public MBaseTestState
/*
 *  A state class, to be added to a harness,  that encapsulates the addition of a recipient 
 *  to a message through the SendAs interface
 */
    {
    public:
        CObexSendAsAddRecipientState(const TDesC& aRecipient, CObexClientTest& aClientTest);
	    void StartL(TRequestStatus& aStatus);
    private:
	    CObexClientTest& iClientTest;
        TBuf<256> iRecipient;       //TODO:-Change this attribute to a HBufC or find out what the max size of this attrib is
    };

class CObexSendAsAddBTRecipientState : public CBase, public MBaseTestState
/*
 *  A state class, to be added to a harness,  that encapsulates the addition of a recipient 
 *  to a message through the SendAs interface
 */
    {
    public:
        CObexSendAsAddBTRecipientState(const TDesC& aRecipient, CObexClientTest& aClientTest);
	    void StartL(TRequestStatus& aStatus);
		static TUint8 GetByteFromUnicodeHexTextChar(const TUint16 *input);
    private:
	    CObexClientTest& iClientTest;
        TBuf<3> iRecipient;       // A Bluetooth address is 6 bytes, 3 unicode chars.
    };

class CObexSendAsSetSubjectState : public CBase, public MBaseTestState
/*
 *  A state class, to be added to a harness,  that encapsulates the addition of a subject 
 *  to a message through the SendAs interface
 */
    {
    public:
        CObexSendAsSetSubjectState(const TDesC& aSubject, CObexClientTest& aClientTest);
	    void StartL(TRequestStatus& aStatus);
    private:
	    CObexClientTest& iClientTest;
        TBuf<256> iSubject;         //TODO:-Change this attribute to a HBufC or find out what the max size of this attrib is
    };

class CObexSendAsAddAttachmentState : public CBase, public MBaseTestState
/*
 *  A state class, to be added to a harness,  that encapsulates the addition of an attachment 
 *  to a message through the SendAs interface
 */
    {
    public:
        CObexSendAsAddAttachmentState(const TDesC& aFileName, CObexClientTest& aClientTest);
	    void StartL(TRequestStatus& aStatus);
    private:
	    CObexClientTest& iClientTest;
        TFileName iFileName;
    };
	
class CObexSendAsAbandonState : public CBase, public MBaseTestState
/*
 *  A state class, to be added to a harness,  that encapsulates the abandonment of a
 *  message through the SendAs interface
 */
    {
    public:
        CObexSendAsAbandonState(CObexClientTest& aClientTest);
	    void StartL(TRequestStatus& aStatus);
    private:
	    CObexClientTest& iClientTest;
    };

class CObexSendAsSaveState : public CBase, public MBaseTestState
/*
 *  A state class, to be added to a harness,  that encapsulates the saving of a
 *  message through the SendAs interface
 */
    {
    public:
        CObexSendAsSaveState(CObexClientTest& aClientTest);
	    void StartL(TRequestStatus& aStatus);
    private:
	    CObexClientTest& iClientTest;
    };

class CObexSendAsValidateState : public CBase, public MBaseTestState
/*
 *  A state class, to be added to a harness,  that encapsulates the validation of a
 *  message through the SendAs interface
 */
    {
    public:
        CObexSendAsValidateState(CObexClientTest& aClientTest);
	    void StartL(TRequestStatus& aStatus);
    private:
	    CObexClientTest& iClientTest;
    };


#endif //__OBEXSENDASSTATES_H__
