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
* This class is the message utility class of DirectPrint
*
*/

#include <bautils.h>
#include <eikenv.h>
#include <s32file.h>
#include <aknnotewrappers.h>
#include <DirectPrintApp.rsg>

#include "clog.h"
#include "DirectPrintMsgUtil.h"

CDirectPrintMsgUtil::CDirectPrintMsgUtil()
	{
	}

CDirectPrintMsgUtil::~CDirectPrintMsgUtil()
	{
	}

CDirectPrintMsgUtil* CDirectPrintMsgUtil::NewL()
	{
	CDirectPrintMsgUtil* self = CDirectPrintMsgUtil::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CDirectPrintMsgUtil* CDirectPrintMsgUtil::NewLC()
	{
	CDirectPrintMsgUtil* self = new(ELeave) CDirectPrintMsgUtil();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CDirectPrintMsgUtil::ConstructL()
	{
	LOG("CDirectPrintMsgUtil::ConstructL BEGIN");

	LOG("CDirectPrintMsgUtil::ConstructL END");
	}

// -----------------------------------------------------------------------------
// Displays error note
// -----------------------------------------------------------------------------
//
void CDirectPrintMsgUtil::DisplayErrorNoteL( TInt aResourceId )
    {
    HBufC* noteBuf = CCoeEnv::Static()->AllocReadResourceLC( aResourceId );
    CAknErrorNote* errorNote = new( ELeave ) CAknErrorNote();
    errorNote->ExecuteLD( *noteBuf );
    CleanupStack::PopAndDestroy( noteBuf );
    }

// End of file
