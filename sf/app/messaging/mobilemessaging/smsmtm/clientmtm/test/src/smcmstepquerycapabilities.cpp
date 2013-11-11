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
// KUidMtmQuerySupportedBody, KUidMtmQueryCanSendMsg, KUidMtmQueryCanReceiveMsg, 
// KUidMtmQueryMaxRecipientCount, KUidMsvMtmQueryEditorUid, KUidMsvQuerySupportsBioMsg and 
// KUidMsvQuerySupportsScheduling should be supported, others should return KErrNotSupported.
// 
//

/**
 @SYMTestCaseID MSG-SMSCLIENTMTM-0243-3
 @SYMTestType UT
 @SYMTestPriority High
 @SYMPREQ unknown
 @SYMTestCaseDesc Query the capabilities of the SMS service (test ported from T_SMCM1)
 @SYMTestActions  Query different capabilities of the SMS service
 @SYMTestExpectedResults capabilities KUidMtmQueryMaxBodySize, KUidMtmQueryMaxTotalMsgSize, 
*/


#include "smcmstepquerycapabilities.h"

#include <mtmuids.h>

const TUid KUidSmcmTestNonSense = { 0x00011111 };

CSMCMStepQueryCapabilities::CSMCMStepQueryCapabilities( )
	{
	SetTestStepName( KSMCMStepQueryCapabilities );	
	}
	
CSMCMStepQueryCapabilities::~CSMCMStepQueryCapabilities( )
	{
	
	}
	
TVerdict CSMCMStepQueryCapabilities::doTestStepL( )
	{
	INFO_PRINTF1( _L( "CSMCMStepQueryCapabilities::doTestStepL( )" ) );
	
	SetTestStepResult( EPass );
		
    TInt response;
	TUint32 errorMask = 0;

	if( iSmsClientMtm->QueryCapability( KUidMtmQueryMaxBodySize, response ) != KErrNone )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x1;
		}
		
	if( iSmsClientMtm->QueryCapability( KUidMtmQueryMaxTotalMsgSize, response ) != KErrNone )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x2;
		}
		
	if( iSmsClientMtm->QueryCapability( KUidMtmQuerySupportedBody, response ) != KErrNone )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x4;
		}
	if( response != KMtm7BitBody + KMtm8BitBody + KMtm16BitBody )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x4;
		}

	if( iSmsClientMtm->QueryCapability( KUidMtmQuerySupportAttachments, response ) != KErrNotSupported )
		{
		SetTestStepResult( EFail ); 
		errorMask |= 0x8;
		}
	if( !response )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x8;
		}

	if( iSmsClientMtm->QueryCapability( KUidMtmQuerySupportSubject, response ) != KErrNotSupported )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x10;
		}
	if( !response )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x10;
		}

	if( iSmsClientMtm->QueryCapability( KUidMtmQuerySupportsFolder, response ) != KErrNotSupported )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x20;
		}
	if( !response )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x20;
		}

	if( iSmsClientMtm->QueryCapability( KUidMtmQueryOffLineAllowed, response ) != KErrNotSupported )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x40;
		}
	if( !response )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x40;
		}

	if( iSmsClientMtm->QueryCapability( KUidMtmQueryCanSendMsg, response ) != KErrNone )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x80;
		}
	if( !response )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x80;
		}

	if( iSmsClientMtm->QueryCapability( KUidMtmQueryCanReceiveMsg, response ) != KErrNone )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x100;
		}
	if( !response )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x100;
		}

	if( iSmsClientMtm->QueryCapability( KUidMtmQueryMaxRecipientCount, response ) != KErrNone )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x200;
		}
	if( response != -1 )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x200;
		}

	if( iSmsClientMtm->QueryCapability( KUidMtmQuerySendAsRequiresRenderedImage, response ) != KErrNotSupported )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x400;
		}
	if(!response)
		{
		SetTestStepResult( EFail );
		errorMask |= 0x400;
		}

	if( iSmsClientMtm->QueryCapability( KUidMtmQuerySendAsRenderingUid, response ) != KErrNotSupported )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x800;
		}
	if( !response )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x800;
		}

	if( iSmsClientMtm->QueryCapability( KUidMsvMtmQueryEditorUid, response ) != KErrNone )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x1000;
		}
	if( response != 268441151 ) // 0x1000163f in hex form
		{
		SetTestStepResult( EFail );
		errorMask |= 0x1000;
		}

	if( iSmsClientMtm->QueryCapability( KUidMsvQuerySupportsBioMsg, response ) != KErrNone )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x2000;
		}
	if( !response )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x2000;
		}

	if( iSmsClientMtm->QueryCapability( KUidMsvQuerySupportsScheduling, response ) != KErrNone )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x4000;
		}
	if( !response )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x4000;
		}

	if( iSmsClientMtm->QueryCapability( KUidSmcmTestNonSense, response ) != KErrNotSupported )
		{
		SetTestStepResult( EFail );
		errorMask |= 0x8000;
		}
	if( !response )	
		{
		SetTestStepResult( EFail );
		errorMask |= 0x8000;
		}

	if ( errorMask != 0 )
		{
		ERR_PRINTF2( _L( "SMCMStepQuery Failed. ErrorMask = %x" ),errorMask);
		INFO_PRINTF1( _L( "Bit 1:KUidMtmQueryMaxBodySize" ));
		INFO_PRINTF1( _L( "Bit 2:KUidMtmQueryMaxTotalMsgSize" ));
		INFO_PRINTF1( _L( "Bit 3:KUidMtmQuerySupportedBody" ));
		INFO_PRINTF1( _L( "Bit 4:KUidMtmQuerySupportAttachments" ));
		INFO_PRINTF1( _L( "Bit 5:KUidMtmQuerySupportSubject" ));
		INFO_PRINTF1( _L( "Bit 6:KUidMtmQuerySupportsFolder" ));
		INFO_PRINTF1( _L( "Bit 7:KUidMtmQueryOffLineAllowed" ));
		INFO_PRINTF1( _L( "Bit 8:KUidMtmQueryCanSendMsg" ));
		INFO_PRINTF1( _L( "Bit 9:KUidMtmQueryCanReceiveMsg" ));
		INFO_PRINTF1( _L( "Bit 10:KUidMtmQueryMaxRecipientCount" ));
		INFO_PRINTF1( _L( "Bit 11:KUidMtmQuerySendAsRequiresRenderedImage" ));
		INFO_PRINTF1( _L( "Bit 12:KUidMtmQuerySendAsRenderingUid" ));
		INFO_PRINTF1( _L( "Bit 13:KUidMsvMtmQueryEditorUid" ));
		INFO_PRINTF1( _L( "Bit 14:KUidMsvQuerySupportsBioMsg" ));
		INFO_PRINTF1( _L( "Bit 15:KUidMsvQuerySupportsScheduling" ));
		INFO_PRINTF1( _L( "Bit 16:KUidSmcmTestNonSense" ));
		}

	return TestStepResult( );
	
	}
