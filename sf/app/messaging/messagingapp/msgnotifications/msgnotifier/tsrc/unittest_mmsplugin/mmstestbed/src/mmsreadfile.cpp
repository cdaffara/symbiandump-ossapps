/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:
 */

// INCLUDE FILES
#include    <e32base.h>
#include    <badesca.h>
#include    <e32def.h>
#include    <s32file.h>
#include    <msvids.h>
#include    <mtmdef.h>
//#include    <eikenv.h>
#include    <apgcli.h>
#include    <CMsvMimeHeaders.h>
#include    <AknNoteWrappers.h>

#include    <mmsservercommon.h>
#include    "mmsconst.h"
#include    "mmsclient.h"
#include    "mmsheaders.h"
#include    "mmsreadfile.h"
#include    "mmsmmboxviewheaders.h"
#include    "mmsmmboxmessageheaders.h"
#include    "mmsmmboxflags.h"
#include    "mmssettings.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KMmsGranularity = 8;
_LIT( K1970, "19700000:000000.000000" );    // 1-Jan 1970 0:00:00


// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================


// C++ default constructor can NOT contain any code, that
// might leave.
//
CMmsReadFile::CMmsReadFile()
    {
    // all member variables in a class derived from CBase
    // are automatically set to 0.
    }


//
void CMmsReadFile::ConstructL(RFs& aFs, RFileReadStream& aReadStream )
    {
    // iRowBuffer = HBufC8::NewMaxL( 500 ); // Max row length!!!!
    iFs = aFs;
    iReader = &aReadStream;
    iByteBuffer = HBufC8::NewL( DefaultBufLen );
    iAliasArray = new ( ELeave ) CDesCArrayFlat( KMmsGranularity );
    iAttaStructures = new ( ELeave ) CArrayPtrFlat<CMmsAttaStructure>( KMmsGranularity );
    Reset();
    }

// Two-phased constructor.
CMmsReadFile* CMmsReadFile::NewL(RFs& aFs, RFileReadStream& aReadStream )
    {
    CMmsReadFile* self = new ( ELeave ) CMmsReadFile();
    CleanupStack::PushL( self );
    self->ConstructL( aFs, aReadStream );
    CleanupStack::Pop();
    return self;
    }


// Destructor
CMmsReadFile::~CMmsReadFile()
    {
    Reset(); // resets and destroys all array data
    if (iAliasArray != NULL) iAliasArray->Reset();
    delete iByteBuffer;
    delete iAliasArray;
    delete iAttaStructures;
    }

void CMmsReadFile::Reset()
    {
    // reset all arrays
    if (iAttaStructures != NULL) iAttaStructures->ResetAndDestroy();
    iAttaCount = 0;
    iAttaRoot = 0;
    }

// ---------------------------------------------------------
// CMmsReadFile()::CompleteTest
// ---------------------------------------------------------
//
TInt CMmsReadFile::CompleteTestL( TInt aMessageCounter, CMmsHeaders& aMmsHeaders )
{
    Reset(); // new message

    TUint32 val;
    TUint   limit = 1000000;
    TUint   sizeLimit = 1000 * 1024; //max message size 1000 kB!
    TUint   allLimit = 999999;
    TInt    error;
    TRadix  radix = EDecimal;
    TLex16  lex;
    TTestReadStatus readStatus = ETestUnknown;
    CMmsAttaStructure* oneAtta = NULL;
	TInt    index;
    TTime y1970( K1970 );
    TTimeIntervalMicroSeconds interval;
    TTime date;
    TInt order = 0;
    TInt16 shortInteger = 0;

    //iso luuppi, joka kiertää ja lukee kunnes EOF tule vastaan
    //Big Loop, which passes around and read until the EOF

    iMessageType = iNextMessageType;
    while(readStatus != ETestEof)
        {
        readStatus = ReadRowL();
        if ( readStatus ==  ETestEof )
            {
            return(-1);
            }
        if ( readStatus == ETestUnknown )
            {
            // A line containing only comments, or an unknown tag
            // As using the scripts to test settings is no longer supported,
            // any old keywords used to test CMmsSettings class are ignored
            continue;
            }
        if( readStatus == ETestNewMessage )
            {
            iNextMessageType = readStatus;
            if(aMessageCounter++)
                {
                return(NULL);
                }
            else
                {
                iMessageType = iNextMessageType;
                continue;
                }
            }
        if ( iMessageType == ETestNewMessage)
            {
            switch(readStatus)
                {
                case ETestFrom:
                    aMmsHeaders.SetSenderL( iValueBuffer );
                    break;
                case ETestFromAlias:
					index = FindAlias(iValueBuffer);
					if(index >= 0)
    					{
                        aMmsHeaders.SetSenderL( iAliasArray->MdcaPoint(index).Mid(iAliasArray->MdcaPoint(index).Locate('=') + 1 ) );
	    				}
                    break;
                case ETestTo:
                    aMmsHeaders.AddTypedAddresseeL( iValueBuffer, EMsvRecipientTo );
                    break;
                case ETestToAlias:
					index = FindAlias(iValueBuffer);
					if(index >= 0)
    					{
                        aMmsHeaders.AddTypedAddresseeL( iAliasArray->MdcaPoint(index).Mid(iAliasArray->MdcaPoint(index).Locate('=') + 1 ), EMmsTo );
						}
                    break;
                case ETestCc:
                    aMmsHeaders.AddTypedAddresseeL( iValueBuffer, EMsvRecipientCc );
                    break;
                case ETestCcAlias:
					index = FindAlias(iValueBuffer);
					if(index >= 0)
    					{
                        aMmsHeaders.AddTypedAddresseeL( iAliasArray->MdcaPoint(index).Mid(iAliasArray->MdcaPoint(index).Locate('=') + 1 ), EMmsCc );
						}
                    break;
                case ETestBcc:
                    aMmsHeaders.AddTypedAddresseeL( iValueBuffer, EMsvRecipientBcc );
                    break;
                case ETestBccAlias:
					index = FindAlias(iValueBuffer);
					if(index >= 0)
    					{
                        aMmsHeaders.AddTypedAddresseeL( iAliasArray->MdcaPoint(index).Mid(iAliasArray->MdcaPoint(index).Locate('=') + 1 ), EMmsBcc );
						}
                    break;
                case ETestSubject:
                    aMmsHeaders.SetSubjectL( iValueBuffer );
                    break;
                case ETestExpiryRel:
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    if (error == KErrNone)
                        {
                        aMmsHeaders.SetExpiryInterval( val );
                        }
                    break;
                case ETestExpiryAbs:
                    error = iDate.Set(iValueBuffer);
                    interval = iDate.MicroSecondsFrom( y1970 );
                    // expiry date in seconds from 1.1.1970.
                    aMmsHeaders.SetExpiryDate( (interval.Int64())/1000000 );
                    break;
                case ETestDeliveryTimeRel:
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    if (error == KErrNone)
                        {
                        aMmsHeaders.SetDeliveryTimeInterval( val );
                        }
                    break;
                case ETestDeliveryTimeAbs:
                    error = iDate.Set(iValueBuffer);
                    interval = iDate.MicroSecondsFrom( y1970 );
                    aMmsHeaders.SetDeliveryDate( (interval.Int64())/1000000 );
                    break;
                case ETestDate:
                    error = iDate.Set(iValueBuffer);
                    interval = iDate.MicroSecondsFrom( y1970 );
                    aMmsHeaders.SetDate( (interval.Int64())/1000000 );
                case ETestPriority:
                    val = 0;
                    if ((iValueBuffer.CompareF(KLow)) == 0)
                        {
                        val = EMmsPriorityLow;
                        }
                    else if ((iValueBuffer.CompareF(KNormal)) == 0)
                        {
                        val = EMmsPriorityNormal;
                        }
                    else if ((iValueBuffer.CompareF(KHigh)) == 0)
                        {
                        val = EMmsPriorityHigh;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetMessagePriority( val );
                    break;
                case ETestSenderVisibility:
                    val = 0;
                    if ((iValueBuffer.CompareF(KHide)) == 0)
                        {
                        val = EMmsSenderVisibilityHide;
                        }
                    else if ((iValueBuffer.CompareF(KShow)) == 0)
                        {
                        val = EMmsSenderVisibilityShow;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetSenderVisibility( val );
                    break;
                case ETestDeliveryReport:
                    val = 0;
                    if ((iValueBuffer.CompareF(KYes)) == 0)
                        {
                        val = EMmsYes;
                        }
                    else if ((iValueBuffer.CompareF(KNo)) == 0)
                        {
                        val = EMmsNo;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetDeliveryReport( val );
                    break;
                case ETestDelivReportSendAllow:
                    val = 0;
                    if ((iValueBuffer.CompareF(KYes)) == 0)
                        {
                        val = EMmsYes;
                        }
                    else if ((iValueBuffer.CompareF(KNo)) == 0)
                        {
                        val = EMmsNo;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetReportAllowed( val );
                    break;

                case ETestReadReply:
                    val = 0;
                    if ((iValueBuffer.CompareF(KYes)) == 0)
                        {
                        val = EMmsYes;
                        }
                    else if ((iValueBuffer.CompareF(KNo)) == 0)
                        {
                        val = EMmsNo;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetReadReply( val );
                    break;
                case ETestNewAttachment:
                    oneAtta = CMmsAttaStructure::NewL();
                    iAttaStructures->AppendL(oneAtta);
                    oneAtta->iAtta->Des().Copy( iValueBuffer );
                    iAttaCount++;
                    break;
                case ETestAttachmentType:
                    if (oneAtta != NULL)
                        {
                        oneAtta->iAttaType->Des().Copy( iValueBuffer );
                        }
                    break;
                case ETestAttachmentContLoc:
                    if (oneAtta != NULL)
                        {
                        oneAtta->iAttaName->Des().Copy( iValueBuffer );
                        }
                    break;
                case ETestAttaRecommendedName:
                    if (oneAtta != NULL)
                        {
                        oneAtta->iAttaRecommendedName->Des().Copy( iValueBuffer );
                        }
                    break;
                case ETestAttachmentCharset:
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    if (error == KErrNone)
                        {
                        if (oneAtta != NULL)
                            {
                            oneAtta->iAttaCharset = val;
                            }
                        }
                    break;
                case ETestAttachmentCid:
                    if (oneAtta != NULL)
                        {
                        oneAtta->iAttaCid->Des().Copy(iValueBuffer);
                        }
                    break;
                case ETestAttContTypeParamName:
                    iByteBuffer->Des().Copy( iValueBuffer );
                    if (oneAtta != NULL)
                        {
                        oneAtta->iContentTypeParams->AppendL(iByteBuffer->Des());
                        }
                    break;
                case ETestAttContTypeParamValue:
                    iByteBuffer->Des().Copy( iValueBuffer );
                    if (oneAtta != NULL)
                        {
                        oneAtta->iContentTypeParams->AppendL(iByteBuffer->Des());
                        }
                    break;
                case ETestAttXTypeParamName:
                    iByteBuffer->Des().Copy( iValueBuffer );
                    if (oneAtta != NULL)
                        {
                        oneAtta->iXTypeParams->AppendL(iByteBuffer->Des());
                        }
                    break;
                case ETestAttXTypeParamValue:
                    iByteBuffer->Des().Copy( iValueBuffer );
                    if (oneAtta != NULL)
                        {
                        oneAtta->iXTypeParams->AppendL(iByteBuffer->Des());
                        }
                    break;
                case ETestAttachmentRoot:
                    iAttaRoot = iAttaCount;
                    break;
                case ETestAlias:
                    // all aliases are global even if they appear
                    // in the middle of a message
                    iAliasArray->AppendL( iValueBuffer );
                    iAliasCount++;
				    break;
                case ETestMessageClass: // should be handled!
                    val = EMmsClassPersonal;
                    if ((iValueBuffer.CompareF(KPersonal)) == 0)
                        {
                        val = EMmsClassPersonal;
                        }
                    else if ((iValueBuffer.CompareF(KAdvertisement)) == 0)
                        {
                        val = EMmsClassAdvertisement;
                        }
                    else if ((iValueBuffer.CompareF(KInformational)) == 0)
                        {
                        val = EMmsClassInformational;
                        }
                    else if ((iValueBuffer.CompareF(KAuto)) == 0)
                        {
                        val = EMmsClassAuto;
                        }
                    else
                        {
                        val = (TMmsMessageClass)KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetMessageClass( val );
                    break;
                case ETestReplyCharging:
                    val = 0;
                    if ((iValueBuffer.CompareF(KRequested)) == 0)
                        {
                        val = KMmsReplyChargingRequested;
                        }
                    else if ((iValueBuffer.CompareF(KReqTextOnly)) == 0)
                        {
                        val = KMmsReplyChargingRequestedTextOnly;
                        }
                    else if ((iValueBuffer.CompareF(KAccepted)) == 0)
                        {
                        val = KMmsReplyChargingAccepted;
                        }
                    else if ((iValueBuffer.CompareF(KAccTextOnly)) == 0)
                        {
                        val = KMmsReplyChargingAcceptedTextOnly;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetReplyCharging( val );
                    break;
                case ETestReplyChargAbs:
                    error = date.Set(iValueBuffer);
                    if ( error == KErrNone )
                        {
                        interval = date.MicroSecondsFrom( y1970 );
                        aMmsHeaders.SetReplyChargingDate( (interval.Int64())/1000000 );
                        }
                    break;
                case ETestReplyChargRel:
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    if (error == KErrNone)
                        {
                        aMmsHeaders.SetReplyChargingInterval( val );
                        }
                    break;
                case ETestReplyChargSize:
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,sizeLimit);
                    if (error == KErrNone)
                        {
                        aMmsHeaders.SetReplyChargingSize( val );
                        }
                    break;
                case ETestReplyChargID:
                    iByteBuffer->Des().Copy( iValueBuffer );
                    aMmsHeaders.SetReplyChargingIdL( iByteBuffer->Des() );
                    break;
                case ETestTID:
                    iByteBuffer->Des().Copy( iValueBuffer );
                    aMmsHeaders.SetTidL( iByteBuffer->Des() );
                    break;
                case ETestContentLocation:
                    iByteBuffer->Des().Copy( iValueBuffer );
                    if ( aMmsHeaders.MessageType() == KMmsMessageTypeMboxViewReq ||
                        aMmsHeaders.MessageType() == KMmsMessageTypeMboxViewConf ||
                        aMmsHeaders.MessageType() == KMmsMessageTypeMBoxDeleteReq )
                        {
                        aMmsHeaders.MMBoxMessageHeadersL().ContentLocationList().AppendL( iByteBuffer->Des() );
                        }
                    else if ( aMmsHeaders.MessageType() == KMmsMessageTypeMBoxDeleteConf )
                        {
                        aMmsHeaders.InsertDeleteContentLocationL( order, iByteBuffer->Des() );
                        }
                    else
                        {
                        aMmsHeaders.SetContentLocationL( iByteBuffer->Des() );
                        }
                    break;
                case ETestPreviouslySentIndex:
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    order = val;
                    break;
                case ETestPreviouslySentBy:
                    aMmsHeaders.InsertPreviouslySentByL( order, iValueBuffer );
                    break;
                case ETestPreviouslySentDate:
                    error = date.Set(iValueBuffer);
                    if ( error == KErrNone )
                        {
                        interval = date.MicroSecondsFrom( y1970 );
                        aMmsHeaders.InsertPreviouslySentDateL( order, (interval.Int64())/1000000 );
                        }
                    break;
                case ETestMessageId:
                    iByteBuffer->Des().Copy( iValueBuffer );
                    aMmsHeaders.SetMessageIdL( iByteBuffer->Des() );
                    break;
                case ETestMessageSize:
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,sizeLimit);
                    if (error == KErrNone)
                        {
                        aMmsHeaders.SetMessageSize( val );
                        }
                    break;
                case ETestVersion:
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,EHex,limit);
                    shortInteger = TInt16( val );
                    if (error == KErrNone)
                        {
                        aMmsHeaders.SetMmsVersion( shortInteger );
                        }
                    break;
                case ETestReadStatus:
                    val = 0;
                    if ((iValueBuffer.CompareF(KRead)) == 0)
                        {
                        val = KMmsReadStatusRead;
                        }
                    else if ((iValueBuffer.CompareF(KDelNotRead)) == 0)
                        {
                        val = KMmsReadStatusDeletedWithoutBeingRead;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetReadStatus( val );
                    break;
                case ETestResponseStatus:
                    val = 0;
                    if ((iValueBuffer.CompareF(KOk)) == 0)
                        {
                        val = KMmsStatusOk;
                        }
                    else if ((iValueBuffer.CompareF(KErrUnspecified)) == 0)
                        {
                        val = KMmsErrorUnspecified;
                        }
                    else if ((iValueBuffer.CompareF(KErrServiceDenied)) == 0)
                        {
                        val = KMmsErrorServiceDenied;
                        }
                    else if ((iValueBuffer.CompareF(KErrMessageFormatCorrupt)) == 0)
                        {
                        val = KMmsErrorMessageFormatCorrupt;
                        }
                    else if ((iValueBuffer.CompareF(KErrAddressUnresolved)) == 0)
                        {
                        val = KMmsErrorSendingAddressUnresolved;
                        }
                    else if ((iValueBuffer.CompareF(KErrMessageNotFound)) == 0)
                        {
                        val = KMmsErrorMessageNotFound;
                        }
                    else if ((iValueBuffer.CompareF(KErrNetworkProblem)) == 0)
                        {
                        val = KMmsErrorNetworkProblem;
                        }
                    else if ((iValueBuffer.CompareF(KErrContentNotAccepted)) == 0)
                        {
                        val = KMmsErrorNoContentAccepted;
                        }
                    else if ((iValueBuffer.CompareF(KErrUnsupportedMessage)) == 0)
                        {
                        val = KMmsErrorUnsupportedMessage;
                        }
                    else if ((iValueBuffer.CompareF(KErrTransient)) == 0)
                        {
                        val = KMmsErrorTransientFailure;
                        }
                    else if ((iValueBuffer.CompareF(KErrTransientAddressUnresolved)) == 0)
                        {
                        val = KMmsErrorTransientSendingAddressUnresolved;
                        }
                    else if ((iValueBuffer.CompareF(KErrTransientNotFound)) == 0)
                        {
                        val = KMmsErrorTransientMessageNotFound;
                        }
                    else if ((iValueBuffer.CompareF(KErrTransientNetworkproblem)) == 0)
                        {
                        val = KMmsErrorTransientNetworkProblem;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanent)) == 0)
                        {
                        val = KMmsErrorPermanentFailure;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanentServiceDenied)) == 0)
                        {
                        val = KMmsErrorPermanentServiceDenied;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanentMessageFormatCorrupt)) == 0)
                        {
                        val = KMmsErrorPermanentMessageFormatCorrupt;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanentAddressUnresolved)) == 0)
                        {
                        val = KMmsErrorPermanentSendingAddressUnresolved;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanentNotFound)) == 0)
                        {
                        val = KMmsErrorPermanentMessageNotFound;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanentContentNotAccepted)) == 0)
                        {
                        val = KMmsErrorPermanentContentNotAccepted;
                        }
                    else if ((iValueBuffer.CompareF(KErrReplyChargingLimitNotMet)) == 0)
                        {
                        val = KMmsErrorPermanentReplyChargingLimitationsNotMet;
                        }
                    else if ((iValueBuffer.CompareF(KErrReplyChargingRequestNotAccepted)) == 0)
                        {
                        val = KMmsErrorPermanentReplyChargingRequestNotAccepted;
                        }
                    else if ((iValueBuffer.CompareF(KErrReplyChargingForwardingDenied)) == 0)
                        {
                        val = KMmsErrorPermanentReplyChargingForwardingDenied;
                        }
                    else if ((iValueBuffer.CompareF(KErrReplyChargingNotSupported)) == 0)
                        {
                        val = KMmsErrorPermanentReplyChargingNotSupported;
                        }
                    else if ((iValueBuffer.CompareF(KErrTransientPartialSuccess)) == 0)
                        {
                        val = KMmsErrorTransientPartialSuccess;
                        }
                    else if ((iValueBuffer.CompareF(KErrAddressHidingNotSupported)) == 0)
                        {
                        val = KMmsErrorPermanentAddressHidingNotSupported;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    if ( aMmsHeaders.MessageType() == KMmsMessageTypeMBoxDeleteConf )
                        {
                        aMmsHeaders.InsertDeleteStatusL( order, val );
                        }
                    else
                        {
                         aMmsHeaders.SetResponseStatus( val );
                        }
                    break;
                case ETestResponseText:
                    if ( aMmsHeaders.MessageType() == KMmsMessageTypeMBoxDeleteConf )
                        {
                        aMmsHeaders.InsertDeleteResponseTextL( order, iValueBuffer );
                        }
                    else
                        {
                        aMmsHeaders.SetResponseTextL( iValueBuffer );
                        }
                    break;
                case ETestRetrieveStatus:
                    val = 0;
                    if ((iValueBuffer.CompareF(KOk)) == 0)
                        {
                        val = KMmsStatusOk;
                        }
                    else if ((iValueBuffer.CompareF(KErrTransient)) == 0)
                        {
                        val = KMmsErrorTransientFailure;
                        }
                    else if ((iValueBuffer.CompareF(KErrTransientNotFound)) == 0)
                        {
                        val = KMmsErrorReceiveTransientMessageNotFound;
                        }
                    else if ((iValueBuffer.CompareF(KErrTransientNetworkproblem)) == 0)
                        {
                        val = KMmsErrorReceiveTransientNetworkProblem;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanent)) == 0)
                        {
                        val = KMmsErrorPermanentFailure;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanentServiceDenied)) == 0)
                        {
                        val = KMmsErrorPermanentServiceDenied;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanentNotFound)) == 0)
                        {
                        val = KMmsErrorReceivePermanentMessageNotFound;
                        }
                    else if ((iValueBuffer.CompareF(KErrRetrieveContentUnsupported)) == 0)
                        {
                        val = KMmsErrorReceivePermanentContentUnsupported;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetResponseStatus( val );
                    break;
                case ETestRetrieveText:
                    aMmsHeaders.SetResponseTextL( iValueBuffer );
                    break;
                case ETestStatus:
                    val = 0;
                    if ((iValueBuffer.CompareF(KDeferred)) == 0)
                        {
                        val = KMmsMessageStatusDeferred;
                        }
                    else if ((iValueBuffer.CompareF(KExpired)) == 0)
                        {
                        val = KMmsMessageStatusExpired;
                        }
                    else if ((iValueBuffer.CompareF(KRetrieved)) == 0)
                        {
                        val = KMmsMessageStatusRetrieved;
                        }
                    else if ((iValueBuffer.CompareF(KRejected)) == 0)
                        {
                        val = KMmsMessageStatusRejected;
                        }
                    else if ((iValueBuffer.CompareF(KUnrecognized)) == 0)
                        {
                        val = KMmsMessageStatusUnrecognized;
                        }
                    else if ((iValueBuffer.CompareF(KIndeterminate)) == 0)
                        {
                        val = KMmsMessageStatusIndeterminate;
                        }
                    else if ((iValueBuffer.CompareF(KForwarded)) == 0)
                        {
                        val = KMmsMessageStatusForwarded;
                        }
                    else if ((iValueBuffer.CompareF(KUnreachable)) == 0)
                        {
                        val = KMmsMessageStatusUnreachable;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetStatus( val );
                    break;
                case ETestMessageType:
                    val = 0;
                    if ((iValueBuffer.CompareF(KSendReq)) == 0)
                        {
                        val = KMmsMessageTypeMSendReq;
                        }
                    else if ((iValueBuffer.CompareF(KSendConf)) == 0)
                        {
                        val = KMmsMessageTypeMSendConf;
                        }
                    else if ((iValueBuffer.CompareF(KNotifInd)) == 0)
                        {
                        val = KMmsMessageTypeMNotificationInd;
                        }
                    else if ((iValueBuffer.CompareF(KNotifResp)) == 0)
                        {
                        val = KMmsMessageTypeMNotifyRespInd;
                        }
                    else if ((iValueBuffer.CompareF(KRetrConf)) == 0)
                        {
                        val = KMmsMessageTypeMRetrieveConf;
                        }
                    else if ((iValueBuffer.CompareF(KAckInd)) == 0)
                        {
                        val = KMmsMessageTypeAcknowledgeInd;
                        }
                    else if ((iValueBuffer.CompareF(KDelInd)) == 0)
                        {
                        val = KMmsMessageTypeDeliveryInd;
                        }
                    else if ((iValueBuffer.CompareF(KReadReqInd)) == 0)
                        {
                        val = KMmsMessageTypeReadRecInd;
                        }
                    else if ((iValueBuffer.CompareF(KReadOrigInd)) == 0)
                        {
                        val = KMmsMessageTypeReadOrigInd;
                        }
                    else if ((iValueBuffer.CompareF(KForwardRec)) == 0)
                        {
                        val = KMmsMessageTypeForwardReq;
                        }
                    else if ((iValueBuffer.CompareF(KForwardConf)) == 0)
                        {
                        val = KMmsMessageTypeForwardConf;
                        }
                    else if ((iValueBuffer.CompareF(KMBoxStoreReq)) == 0)
                        {
                        val = KMmsMessageTypeMboxStoreReq;
                        }
                    else if ((iValueBuffer.CompareF(KMBoxStoreConf)) == 0)
                        {
                        val = KMmsMessageTypeMboxStoreConf;
                        }
                    else if ((iValueBuffer.CompareF(KMBoxViewReq)) == 0)
                        {
                        val = KMmsMessageTypeMboxViewReq;
                        }
                    else if ((iValueBuffer.CompareF(KMBoxViewConf)) == 0)
                        {
                        val = KMmsMessageTypeMboxViewConf;
                        }
                    else if ((iValueBuffer.CompareF(KMBoxUploadReq)) == 0)
                        {
                        val = KMmsMessageTypeMBoxUploadReq;
                        }
                    else if ((iValueBuffer.CompareF(KMBoxUploadConf)) == 0)
                        {
                        val = KMmsMessageTypeMBoxUploadConf;
                        }
                    else if ((iValueBuffer.CompareF(KMBoxDeleteReq)) == 0)
                        {
                        val = KMmsMessageTypeMBoxDeleteReq;
                        }
                    else if ((iValueBuffer.CompareF(KMBoxDeleteConf)) == 0)
                        {
                        val = KMmsMessageTypeMBoxDeleteConf;
                        }
                    else if ((iValueBuffer.CompareF(KMBoxDescr)) == 0)
                        {
                        val = KMmsMessageTypeMBoxDescr;
                        }
                    else if ((iValueBuffer.CompareF(KDeleteReq)) == 0)
                        {
                        val = KMmsMessageTypeDeleteReq;
                        }
                    else if ((iValueBuffer.CompareF(KDeleteConf)) == 0)
                        {
                        val = KMmsMessageTypeDeleteConf;
                        }
                    else if ((iValueBuffer.CompareF(KCancelReq)) == 0)
                        {
                        val = KMmsMessageTypeCancelReq;
                        }
                    else if ((iValueBuffer.CompareF(KCancelResp)) == 0)
                        {
                        val = KMmsMessageTypeCancelConf;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetMessageType( val );
                    break;
                case ETestAttribute:
                    val = 0;
                    iByteBuffer->Des().Copy( iValueBuffer );
                    if ((iByteBuffer->Des().CompareF(KTestBcc)) == 0)
                        {
                        val = KMmsAssignedBcc;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestCc)) == 0)
                        {
                        val = KMmsAssignedCc;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestContent)) == 0)
                        {
                        val = KMmsAssignedContent;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestContentType)) == 0)
                        {
                        val = KMmsAssignedContentType;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestDate)) == 0)
                        {
                        val = KMmsAssignedDate;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestDeliveryReport)) == 0)
                        {
                        val = KMmsAssignedDeliveryReport;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestDeliveryTime)) == 0)
                        {
                        val = KMmsAssignedDeliveryTime;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestExpiry)) == 0)
                        {
                        val = KMmsAssignedExpiry;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestFrom)) == 0)
                        {
                        val = KMmsAssignedFrom;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestMessageClass)) == 0)
                        {
                        val = KMmsAssignedMessageClass;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestMessageId)) == 0)
                        {
                        val = KMmsAssignedMessageId;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestMessageSize)) == 0)
                        {
                        val = KMmsAssignedMessageSize;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestPriority)) == 0)
                        {
                        val = KMmsAssignedPriority;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestReadReply)) == 0)
                        {
                        val = KMmsAssignedReadReply;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestSubject)) == 0)
                        {
                        val = KMmsAssignedSubject;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestTo)) == 0)
                        {
                        val = KMmsAssignedTo;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestReplyCharging)) == 0)
                        {
                        val = KMmsAssignedReplyCharging;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestReplyChargID)) == 0)
                        {
                        val = KMmsAssignedReplyChargingID;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestReplyCharg)) == 0)
                        {
                        val = KMmsAssignedReplyChargingDeadline;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestReplyChargSize)) == 0)
                        {
                        val = KMmsAssignedReplyChargingSize;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestPreviouslySentBy)) == 0)
                        {
                        val = KMmsAssignedPreviouslySentBy;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestPreviouslySentDate)) == 0)
                        {
                        val = KMmsAssignedPreviouslySentDate;
                        }
                    else if ((iByteBuffer->Des().CompareF(KTestAdditionalHeaders)) == 0)
                        {
                        val = KMmsAssignedAdditionalHeaders;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.MMBoxViewHeadersL().AttributeArray().InsertInOrder(val);
                    break;
                case ETestDistributionIndicator:
                    val = 0;
                    if ((iValueBuffer.CompareF(KYes)) == 0)
                        {
                        val = KMmsYes;
                        }
                    else if ((iValueBuffer.CompareF(KNo)) == 0)
                        {
                        val = KMmsNo;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetDistributionIndicator( val );
                    break;
                case ETestLimit:
                    aMmsHeaders.MMBoxViewHeadersL().SetMmsLimit(KMaxTUint32);
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    if ( val > allLimit )
                        {
                        val = KMaxTUint32; // this is too hard to type, more than 999999 means "all"
                        }
                    if( error == KErrNone )
                        {
                        aMmsHeaders.MMBoxViewHeadersL().SetMmsLimit(val);
                        }
                    break;
                case ETestMessageQuota:
                    aMmsHeaders.MMBoxViewHeadersL().SetMMBoxQuotaNumber(KMaxTUint32);
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    if ( val > allLimit )
                        {
                        val = KMaxTUint32; // this is too hard to type, more than 999999 means "all"
                        }
                    if (error == KErrNone)
                        {
                        aMmsHeaders.MMBoxViewHeadersL().SetMMBoxQuotaNumber(val);
                        }
                    break;
                case ETestSizeQuota:
                    aMmsHeaders.MMBoxViewHeadersL().SetMMBoxQuotaSize(KMaxTUint32);
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    if ( val > allLimit )
                        {
                        val = KMaxTUint32; // this is too hard to type, more than 999999 means "all"
                        }
                    if (error == KErrNone)
                        {
                        aMmsHeaders.MMBoxViewHeadersL().SetMMBoxQuotaSize(val);
                        }
                    break;
                case ETestMessageTotal:
                    aMmsHeaders.MMBoxViewHeadersL().SetMMBoxTotalNumber(KMaxTUint32);
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    if ( val > allLimit )
                        {
                        val = KMaxTUint32; // this is too hard to type, more than 999999 means "all"
                        }
                    if (error == KErrNone)
                        {
                        aMmsHeaders.MMBoxViewHeadersL().SetMMBoxTotalNumber(val);
                        }
                    break;
                case ETestSizeTotal:
                    aMmsHeaders.MMBoxViewHeadersL().SetMMBoxTotalSize(KMaxTUint32);
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    if ( val > allLimit )
                        {
                        val = KMaxTUint32; // this is too hard to type, more than 999999 means "all"
                        }
                    if (error == KErrNone)
                        {
                        aMmsHeaders.MMBoxViewHeadersL().SetMMBoxTotalSize(val);
                        }
                    break;
                case ETestMessageCount:
                    aMmsHeaders.MMBoxViewHeadersL().SetMmsMessageCount(KMaxTUint32);
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    if ( val > allLimit )
                        {
                        val = KMaxTUint32; // this is too hard to type, more than 999999 means "all"
                        }
                    if (error == KErrNone)
                        {
                        aMmsHeaders.MMBoxViewHeadersL().SetMmsMessageCount(val);
                        }
                    break;
                case ETestAddKeyword:
                    aMmsHeaders.MMBoxMessageHeadersL().AppendKeywordItemL(KMmsAddToken, iValueBuffer);
                    break;
                case ETestRemoveKeyword:
                    aMmsHeaders.MMBoxMessageHeadersL().AppendKeywordItemL(KMmsRemoveToken, iValueBuffer);
                    break;
                case ETestFilterKeyword:
                    aMmsHeaders.MMBoxMessageHeadersL().AppendKeywordItemL(KMmsFilterToken, iValueBuffer);
                    break;
                case ETestMMState:
                    val = 0;
                    if ((iValueBuffer.CompareF(KDraft)) == 0)
                        {
                        val = KMmsDraft;
                        }
                    else if ((iValueBuffer.CompareF(KSent)) == 0)
                        {
                        val = KMmsSent;
                        }
                    else if ((iValueBuffer.CompareF(KNew)) == 0)
                        {
                        val = KMmsNew;
                        }
                    else if ((iValueBuffer.CompareF(KRetrieved)) == 0)
                        {
                        val = KMmsRetrieved;
                        }
                    else if ((iValueBuffer.CompareF(KForwarded)) == 0)
                        {
                        val = KMmsForwarded;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    if ( aMmsHeaders.MessageType() == KMmsMessageTypeMboxViewReq ||
                        aMmsHeaders.MessageType() == KMmsMessageTypeMboxViewConf )
                        {
                        aMmsHeaders.MMBoxViewHeadersL().MMStateArray().InsertInOrder( val );
                        }
                    else
                        {
                        aMmsHeaders.MMBoxMessageHeadersL().SetMMState( val );
                        }
                    break;
                case ETestQuota:
                    val = 0;
                    if ((iValueBuffer.CompareF(KYes)) == 0)
                        {
                        val = KMmsYes;
                        }
                    else if ((iValueBuffer.CompareF(KNo)) == 0)
                        {
                        val = KMmsNo;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.MMBoxViewHeadersL().SetMmsQuotas( val );
                    break;
                case ETestStart:
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    if (error == KErrNone)
                        {
                        aMmsHeaders.MMBoxViewHeadersL().SetMmsStart(val);
                        }
                    break;
                case ETestStore:
                    val = 0;
                    if ((iValueBuffer.CompareF(KYes)) == 0)
                        {
                        val = KMmsYes;
                        }
                    else if ((iValueBuffer.CompareF(KNo)) == 0)
                        {
                        val = KMmsNo;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.MMBoxMessageHeadersL().SetMmsStore( val );
                    break;
                case ETestStored:
                    val = 0;
                    if ((iValueBuffer.CompareF(KYes)) == 0)
                        {
                        val = KMmsYes;
                        }
                    else if ((iValueBuffer.CompareF(KNo)) == 0)
                        {
                        val = KMmsNo;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.MMBoxMessageHeadersL().SetMmsStored( val );
                    break;
                case ETestStoreStatus:
                    val = 0;
                    if ((iValueBuffer.CompareF(KOk)) == 0)
                        {
                        val = KMmsStatusOk;
                        }
                    else if ((iValueBuffer.CompareF(KErrTransient)) == 0)
                        {
                        val = KMmsErrorTransientFailure;
                        }
                    else if ((iValueBuffer.CompareF(KErrTransientNetworkproblem)) == 0)
                        {
                        val = KMmsErrorStoreStatusTransientNetworkProblem;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanent)) == 0)
                        {
                        val = KMmsErrorPermanentFailure;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanentServiceDenied)) == 0)
                        {
                        val = KMmsErrorPermanentServiceDenied;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanentMessageFormatCorrupt)) == 0)
                        {
                        val = KMmsErrorPermanentMessageFormatCorrupt;
                        }
                    else if ((iValueBuffer.CompareF(KErrPermanentNotFound)) == 0)
                        {
                        val = KMmsErrorStoreStatusPermanentMessageNotFound;
                        }
                    else if ((iValueBuffer.CompareF(KErrMMBoxFull)) == 0)
                        {
                        val = KMmsErrorStoreStatusPermanentMmboxFull;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.MMBoxMessageHeadersL().SetMmsStoreStatus( val );
                    break;
                case ETestStoreStatusText:
                    aMmsHeaders.MMBoxMessageHeadersL().SetMmsStoreStatusTextL( iValueBuffer );
                    break;
                case ETestTotals:
                    val = 0;
                    if ((iValueBuffer.CompareF(KYes)) == 0)
                        {
                        val = KMmsYes;
                        }
                    else if ((iValueBuffer.CompareF(KNo)) == 0)
                        {
                        val = KMmsNo;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.MMBoxViewHeadersL().SetMmsTotals( val );
                    break;

                case ETestDeleteInfoIndex:
                    lex.Assign(iValueBuffer);
                    error = lex.Val(val,radix,limit);
                    order = val;
                    break;

                case ETestApplicId:
                    aMmsHeaders.SetApplicIdL( iValueBuffer );
                    break;
                case ETestReplyApplicId:
                    aMmsHeaders.SetReplyApplicIdL( iValueBuffer );
                    break;
                case ETestApplicInfo:
                    iByteBuffer->Des().Copy( iValueBuffer );
                    aMmsHeaders.SetAuxApplicInfoL( iByteBuffer->Des() );
                    break;
                case ETestContentClass:
                    val = 0;
                    if ((iValueBuffer.CompareF(KText)) == 0)
                        {
                        val = KMmsContentClassText;
                        }
                    else if ((iValueBuffer.CompareF(KImageBasic)) == 0)
                        {
                        val = KMmsContentClassImageBasic;
                        }
                    else if ((iValueBuffer.CompareF(KImageRich)) == 0)
                        {
                        val = KMmsContentClassImageRich;
                        }
                    else if ((iValueBuffer.CompareF(KVideoBasic)) == 0)
                        {
                        val = KMmsContentClassVideoBasic;
                        }
                    else if ((iValueBuffer.CompareF(KVideoRich)) == 0)
                        {
                        val = KMmsContentClassVideoRich;
                        }
                    else if ((iValueBuffer.CompareF(KMegaPixel)) == 0)
                        {
                        val = KMmsContentClassMegaPixel;
                        }
                    else if ((iValueBuffer.CompareF(KContentBasic)) == 0)
                        {
                        val = KMmsContentClassContentBasic;
                        }
                    else if ((iValueBuffer.CompareF(KContentRich)) == 0)
                        {
                        val = KMmsContentClassContentRich;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetContentClass( val );
                    break;
                case ETestDrmContent:
                    val = 0;
                    if ((iValueBuffer.CompareF(KYes)) == 0)
                        {
                        val = KMmsYes;
                        }
                    else if ((iValueBuffer.CompareF(KNo)) == 0)
                        {
                        val = KMmsNo;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetDrmContent( val );
                    break;
                case ETestAdaptationAllowed:
                    val = 0;
                    if ((iValueBuffer.CompareF(KYes)) == 0)
                        {
                        val = KMmsYes;
                        }
                    else if ((iValueBuffer.CompareF(KNo)) == 0)
                        {
                        val = KMmsNo;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetAdaptationAllowed( val );
                    break;
                case ETestRecommendedRetrievalMode:
                    val = 0;
                    if ((iValueBuffer.CompareF(KManual)) == 0)
                        {
                        val = KMmsRecommendedRetrievalModeManual;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetRecommendedRetrievalMode( val );
                    break;
                case ETestRecRetrievalModeText:
                    aMmsHeaders.SetRecommendedRetrievalModeTextL( iValueBuffer );
                    break;
                case ETestReplaceId:
                    iByteBuffer->Des().Copy( iValueBuffer );
                    aMmsHeaders.SetReplaceCancelIdL( iByteBuffer->Des() );
                    break;
                case ETestStatusText:
                    aMmsHeaders.SetResponseTextL( iValueBuffer );
                    break;
                case ETestCancelId:
                    iByteBuffer->Des().Copy( iValueBuffer );
                    aMmsHeaders.SetReplaceCancelIdL( iByteBuffer->Des() );
                    break;
                case ETestCancelStatus:
                    val = 0;
                    if ((iValueBuffer.CompareF(KCancelSuccessful)) == 0)
                        {
                        val = KMmsCancelRequestSuccessfullyReceived;
                        }
                    else if ((iValueBuffer.CompareF(KCancelCorrupted)) == 0)
                        {
                        val = KMmsCancelRequestCorrupted;
                        }
                    else
                        {
                        val = KMmsTestIllegalValue;
                        }
                    aMmsHeaders.SetCancelStatus( val );
                    break;
                default:
                    break;
                }
            }
            else
            {
            switch(readStatus)
                {
                case ETestAlias:
                    iAliasArray->AppendL( iValueBuffer );
                    iAliasCount++;
				    break;

                default:
                    break;
                }

            }
        }
    return(-1);
}

// ---------------------------------------------------------
// CMmsReadFile()::ReadRowL
// ---------------------------------------------------------
//
TTestReadStatus CMmsReadFile::ReadRowL()
    {

        TBuf8<DefaultBufLen> RowBuffer;
        TBuf8<32> KeyBuffer;

        // READ ONE ROW AND DROP CR+LF FROM THE END OF LINE
        TChar delim( 10 );
        iReader->ReadL( RowBuffer, delim);
        TInt length = RowBuffer.Length();
        if ( RowBuffer.Length() < 2 )
            {
            return ETestEof;
            }
        RowBuffer.Delete(length - 2,2);

        // DROP POSSIBLE COMMENT OUT
        TInt pos = RowBuffer.Locate( ';' );
		if ( pos >= 0 ) RowBuffer.Delete( pos, length-pos-2 );

        if ( RowBuffer.Length() == 0 )
            {
            // the line contained only comment
            return ETestUnknown;
            }

        // First split the row (:)
        pos = RowBuffer.Locate( ':' );
        if (pos > 0)
        {
            TPtrC8 p = RowBuffer.Mid( pos+1 );
            length = p.Length();
            iValueBuffer.Zero();
            for (TInt i=0; i < length; ++i)
                {
                iValueBuffer.Append(p[i]);
                }
            iValueBuffer.Trim();
            TPtrC8 pp = RowBuffer.Left(pos);
            KeyBuffer.CopyUC(pp);
            KeyBuffer.Trim();
        }
        // TRY TO FIND CORRECT TAG
        if ((KeyBuffer.CompareF(KTestNewMessage)) == 0) return(ETestNewMessage);
        if ((KeyBuffer.CompareF(KTestMessageType)) == 0) return ETestMessageType;
        if ((KeyBuffer.CompareF(KTestDate)) == 0) return ETestDate;
        if ((KeyBuffer.CompareF(KTestFrom)) == 0) return ETestFrom;
        if ((KeyBuffer.CompareF(KTestTo)) == 0) return ETestTo;
        if ((KeyBuffer.CompareF(KTestCc)) == 0) return ETestCc;
        if ((KeyBuffer.CompareF(KTestBcc)) == 0) return ETestBcc;
        if ((KeyBuffer.CompareF(KTestSubject)) == 0) return ETestSubject;
        if ((KeyBuffer.CompareF(KTestExpiryRel)) == 0) return ETestExpiryRel;
        if ((KeyBuffer.CompareF(KTestExpiryAbs)) == 0) return ETestExpiryAbs;
        if ((KeyBuffer.CompareF(KTestDeliveryTimeRel)) == 0) return ETestDeliveryTimeRel;
        if ((KeyBuffer.CompareF(KTestDeliveryTimeAbs)) == 0) return ETestDeliveryTimeAbs;
        if ((KeyBuffer.CompareF(KTestPriority)) == 0) return ETestPriority;
        if ((KeyBuffer.CompareF(KTestSenderVisibility)) == 0) return ETestSenderVisibility;
        if ((KeyBuffer.CompareF(KTestDeliveryReport)) == 0) return ETestDeliveryReport;
        if ((KeyBuffer.CompareF(KTestReadReply)) == 0) return ETestReadReply;
        if ((KeyBuffer.CompareF(KTestNewAttachment)) == 0) return ETestNewAttachment;
        if ((KeyBuffer.CompareF(KTestAttachmentName)) == 0) return ETestAttaRecommendedName;
        if ((KeyBuffer.CompareF(KTestAttachmentContLoc)) == 0) return ETestAttachmentContLoc;
        if ((KeyBuffer.CompareF(KTestAttachmentType)) == 0) return ETestAttachmentType;
        if ((KeyBuffer.CompareF(KTestAttachmentCharset)) == 0) return ETestAttachmentCharset;
        if ((KeyBuffer.CompareF(KTestAttachmentCid)) == 0) return ETestAttachmentCid;
        if ((KeyBuffer.CompareF(KTestAttachmentRoot)) == 0) return ETestAttachmentRoot;
        if ((KeyBuffer.CompareF(KTestEndOfFile)) == 0) return ETestEof;
        if ((KeyBuffer.CompareF(KTestMessageClass)) == 0) return ETestMessageClass;
        if ((KeyBuffer.CompareF(KTestDelivRepSendAllow)) == 0) return ETestDelivReportSendAllow;
        if ((KeyBuffer.CompareF(KTestAlias)) == 0 ) return ETestAlias;
        if ((KeyBuffer.CompareF(KTestFromAlias)) == 0 ) return ETestFromAlias;
        if ((KeyBuffer.CompareF(KTestToAlias)) == 0 ) return ETestToAlias;
        if ((KeyBuffer.CompareF(KTestCcAlias)) == 0 ) return ETestCcAlias;
        if ((KeyBuffer.CompareF(KTestBccAlias)) == 0 ) return ETestBccAlias;
        if ((KeyBuffer.CompareF(KTestReplyCharging)) == 0 ) return ETestReplyCharging;
        if ((KeyBuffer.CompareF(KTestReplyChargAbs)) == 0 ) return ETestReplyChargAbs;
        if ((KeyBuffer.CompareF(KTestReplyChargRel)) == 0 ) return ETestReplyChargRel;
        if ((KeyBuffer.CompareF(KTestReplyChargSize)) == 0 ) return ETestReplyChargSize;
        if ((KeyBuffer.CompareF(KTestReplyChargID)) == 0 ) return ETestReplyChargID;
        if ((KeyBuffer.CompareF(KTestTID)) == 0 ) return ETestTID;
        if ((KeyBuffer.CompareF(KTestContentLocation)) == 0 ) return ETestContentLocation;
        if ((KeyBuffer.CompareF(KTestPreviouslySentIndex)) == 0 ) return ETestPreviouslySentIndex;
        if ((KeyBuffer.CompareF(KTestPreviouslySentBy)) == 0 ) return ETestPreviouslySentBy;
        if ((KeyBuffer.CompareF(KTestPreviouslySentDate)) == 0 ) return ETestPreviouslySentDate;
        if ((KeyBuffer.CompareF(KTestMessageId)) == 0 ) return ETestMessageId;
        if ((KeyBuffer.CompareF(KTestMessageSize)) == 0 ) return ETestMessageSize;
        if ((KeyBuffer.CompareF(KTestVersion)) == 0 ) return ETestVersion;
        if ((KeyBuffer.CompareF(KTestReadStatus)) == 0 ) return ETestReadStatus;
        if ((KeyBuffer.CompareF(KTestResponseStatus)) == 0 ) return ETestResponseStatus;
        if ((KeyBuffer.CompareF(KTestResponseText)) == 0 ) return ETestResponseText;
        if ((KeyBuffer.CompareF(KTestRetrieveStatus)) == 0 ) return ETestRetrieveStatus;
        if ((KeyBuffer.CompareF(KTestRetrieveText)) == 0 ) return ETestRetrieveText;
        if ((KeyBuffer.CompareF(KTestStatus)) == 0 ) return ETestStatus;
        if ((KeyBuffer.CompareF(KTestAttribute)) == 0 ) return ETestAttribute;
        if ((KeyBuffer.CompareF(KTestDistributionIndicator)) == 0 ) return ETestDistributionIndicator;
        if ((KeyBuffer.CompareF(KTestLimit)) == 0 ) return ETestLimit;
        if ((KeyBuffer.CompareF(KTestMessageQuota)) == 0 ) return ETestMessageQuota;
        if ((KeyBuffer.CompareF(KTestSizeQuota)) == 0 ) return ETestSizeQuota;
        if ((KeyBuffer.CompareF(KTestMessageTotal)) == 0 ) return ETestMessageTotal;
        if ((KeyBuffer.CompareF(KTestSizeTotal)) == 0 ) return ETestSizeTotal;
        if ((KeyBuffer.CompareF(KTestMessageCount)) == 0 ) return ETestMessageCount;
        if ((KeyBuffer.CompareF(KTestAddKeyword)) == 0 ) return ETestAddKeyword;
        if ((KeyBuffer.CompareF(KTestRemoveKeyword)) == 0 ) return ETestRemoveKeyword;
        if ((KeyBuffer.CompareF(KTestFilterKeyword)) == 0 ) return ETestFilterKeyword;
        if ((KeyBuffer.CompareF(KTestMMState)) == 0 ) return ETestMMState;
        if ((KeyBuffer.CompareF(KTestQuota)) == 0 ) return ETestQuota;
        if ((KeyBuffer.CompareF(KTestStart)) == 0 ) return ETestStart;
        if ((KeyBuffer.CompareF(KTestStore)) == 0 ) return ETestStore;
        if ((KeyBuffer.CompareF(KTestStored)) == 0 ) return ETestStored;
        if ((KeyBuffer.CompareF(KTestStoreStatus)) == 0 ) return ETestStoreStatus;
        if ((KeyBuffer.CompareF(KTestStoreStatusText)) == 0 ) return ETestStoreStatusText;
        if ((KeyBuffer.CompareF(KTestTotals)) == 0 ) return ETestTotals;
        if ((KeyBuffer.CompareF(KTestDeleteInfoIndex)) == 0 ) return ETestDeleteInfoIndex;
        if ((KeyBuffer.CompareF(KTestApplicId)) == 0 ) return ETestApplicId;
        if ((KeyBuffer.CompareF(KTestReplyApplicId)) == 0 ) return ETestReplyApplicId;
        if ((KeyBuffer.CompareF(KTestApplicInfo)) == 0 ) return ETestApplicInfo;
        if ((KeyBuffer.CompareF(KTestContentClass)) == 0 ) return ETestContentClass;
        if ((KeyBuffer.CompareF(KTestDrmContent)) == 0 ) return ETestDrmContent;
        if ((KeyBuffer.CompareF(KTestAdaptationAllowed)) == 0 ) return ETestAdaptationAllowed;
        if ((KeyBuffer.CompareF(KTestRecommendedRetrievalMode)) == 0 ) return ETestRecommendedRetrievalMode;
        if ((KeyBuffer.CompareF(KTestRecRetrievalModeText)) == 0 ) return ETestRecRetrievalModeText;
        if ((KeyBuffer.CompareF(KTestReplaceId)) == 0 ) return ETestReplaceId;
        if ((KeyBuffer.CompareF(KTestStatusText)) == 0 ) return ETestStatusText;
        if ((KeyBuffer.CompareF(KTestCancelId)) == 0 ) return ETestCancelId;
        if ((KeyBuffer.CompareF(KTestCancelStatus)) == 0 ) return ETestCancelStatus;
        if ((KeyBuffer.CompareF(KTestAttContTypeParamName)) == 0 ) return ETestAttContTypeParamName;
        if ((KeyBuffer.CompareF(KTestAttContTypeParamValue)) == 0 ) return ETestAttContTypeParamValue;
        if ((KeyBuffer.CompareF(KTestAttXTypeParamName)) == 0 ) return ETestAttXTypeParamName;
        if ((KeyBuffer.CompareF(KTestAttXTypeParamValue)) == 0 ) return ETestAttXTypeParamValue;
        return ETestUnknown;
    }

// ---------------------------------------------------------
// CMmsReadFile()::CreateMessageL
// program build a message from given parts
// ---------------------------------------------------------
//
void CMmsReadFile::CreateMessageL( CMmsClientMtm* aMmsClient, CMmsHeaders* aMmsHeaders )
    {

    // Reset inactivity timer to keem viewServer from crashing
    User::ResetInactivityTime();

    TInt      i;
    TInt error = KErrNone;
    RFile attaFile;
    _LIT8(KLeftAngle, "<");
    _LIT8(KRightAngle, ">");
    // we can't use "seconds from" as it only returns a
    // 32 bit signed integer. If fails in 2038.
    // "microseconds from" returns a 64 bit signed integer

    CMsvStore* store = aMmsClient->Entry().EditStoreL();
    CleanupStack::PushL(store);
    aMmsHeaders->StoreL(*store);
    store->CommitL();
    CleanupStack::PopAndDestroy( store );
    store = NULL;

    aMmsClient->LoadMessageL(); // read store is needed to do this

    store = aMmsClient->Entry().EditStoreL();
    CleanupStack::PushL(store);
    CMsvAttachment* attaInfo = NULL;
    TMsvAttachmentId attaId = 0;

    for ( i=0; i < iAttaStructures->Count(); ++i)
        {
        attaId = KMsvNullIndexEntryId;
        iFilename.Copy(iAttaStructures->At(i)->iAtta->Des());

        error = attaFile.Open( iFs, iFilename, EFileShareReadersOnly | EFileRead );
        User::LeaveIfError( error );

		CleanupClosePushL(attaFile);

		CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
		CleanupStack::PushL( mimeHeaders );
		TPtrC8 contentType = iAttaStructures->At(i)->iAttaType->Des();

        TDataRecognitionResult result;
        result.Reset(); // make sure that it is cleared

        if(iAttaStructures->At(i)->iAttaCid->Length())
            {
            TPtr8 attaCID = iAttaStructures->At(i)->iAttaCid->Des();
            if (attaCID.Find(KLeftAngle) == 0 &&
                attaCID.Find(KRightAngle) == attaCID.Length()-1 )
                {
                // remove angle brackets from cid
                attaCID = attaCID.Mid(1,attaCID.Length()-2);
                }
            mimeHeaders->SetContentIdL(attaCID);
            }

        if (iAttaStructures->At(i)->iAttaCharset)
            {
            mimeHeaders->SetMimeCharset(iAttaStructures->At(i)->iAttaCharset);
            }

        if (iAttaStructures->At(i)->iAttaName->Length())
            {
            iFilename.Copy(iAttaStructures->At(i)->iAttaName->Des());
            }
        iParse.Set( iFilename, NULL, NULL );
        iFilename.Copy( iParse.NameAndExt() );

        mimeHeaders->SetContentLocationL( iFilename );

		// if Mime type has not been set, use RapaRecognizer
		if ( iAttaStructures->At(i)->iAttaType->Length() == 0 && iFilename.Length() > 0)
		    {
		    // TO BE IMPLEMENTED

            RApaLsSession lsSession;

            if ( lsSession.Connect() == KErrNone )
                {
                CleanupClosePushL( lsSession );

                iFilename.Copy(iAttaStructures->At(i)->iAtta->Des());
                if ( lsSession.RecognizeData( iFilename, TPtrC8(), result ) == KErrNone )
                    {
                    // Check confidence level. Recognization must be at least
                    // "EProbable". We don't accept the result if it is "EPossible"
                    // or "EUnlikely" or "ENotRecognized"!

                    if ( result.iConfidence < CApaDataRecognizerType::EProbable )
                        {
                        result.Reset(); // clear buffer and try again with longer buffer
                        }

                    TPtrC8 mimeBuf8 = result.iDataType.Des8();

                    if ( mimeBuf8.Length() == 0 )
                        {
                        // Open file buffer and try again..

                        TInt bufSize = 0;
                        (void)lsSession.GetMaxDataBufSize( bufSize ); // ignore errors
                        if ( bufSize <= 0 )
                            {
                            bufSize = 30;
                            }
                        HBufC8* buf = HBufC8::NewLC( bufSize );
                        TPtr8 des = buf->Des();

                        RFile file;
                        TInt err=file.Open( iFs, iFilename, EFileShareReadersOnly );
                        if ( err == KErrNone )
                            {
                            err = file.Read( des );
                            file.Close();
                            if ( err == KErrNone )
                                {
                                if ( ( lsSession.RecognizeData( iFilename, des, result ) ) == KErrNone )
                                    {
                                    mimeBuf8.Set( result.iDataType.Des8() );
                                    }
                                }

                            }
                        CleanupStack::PopAndDestroy(); // buf
                        }
                    if ( mimeBuf8.Length() > 0 &&
                        result.iConfidence >= CApaDataRecognizerType::EProbable )
                        {
                        contentType.Set( result.iDataType.Des8() );
                        }
                    }
                CleanupStack::PopAndDestroy(1); // lssession
                }
		    }

        if ( contentType.Length() > 0 )
            {
            TInt position = contentType.Find( KMmsSlash8 );
            if ( position >= 0 )
                {
                mimeHeaders->SetContentTypeL( contentType.Left( position ) );
                }
            if ( position < contentType.Length() - 1 )
                {
                mimeHeaders->SetContentSubTypeL( contentType.Mid( position + 1 ) );
                }
//          CreateAttachment2L sets the content type to attaInfo
//            attaInfo->SetMimeTypeL( contentType );
            }

        if (iAttaStructures->At(i)->iAttaRecommendedName->Length())
            {
            iFilename.Copy(iAttaStructures->At(i)->iAttaRecommendedName->Des());
            iParse.Set( iFilename, NULL, NULL );
            iFilename.Copy( iParse.NameAndExt() );
            mimeHeaders->SetSuggestedFilenameL( iFilename );
            }

        TInt j = 0;
        for ( j = 0; j < iAttaStructures->At(i)->iContentTypeParams->MdcaCount(); ++j )
            {
            mimeHeaders->ContentTypeParams().AppendL( iAttaStructures->At(i)->iContentTypeParams->MdcaPoint( j ) );
            }
        for ( j = 0; j < iAttaStructures->At(i)->iXTypeParams->MdcaCount(); ++j )
            {
            mimeHeaders->XTypeParams().AppendL( iAttaStructures->At(i)->iXTypeParams->MdcaPoint( j ) );
            }

        attaInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
        // attaInfo does not go onto cleaunpstack because ownership will
        // be transferred to attachment manager.

        aMmsClient->CreateAttachment2L(
            *store,
            attaFile,
            contentType,
            *mimeHeaders,
            attaInfo,
            attaId);
        attaInfo = NULL; // ownership transferred

        CleanupStack::PopAndDestroy(); // mimeHeaders
        CleanupStack::PopAndDestroy(); // attaFile.Close()

        if ( iAttaRoot > 0 && iAttaRoot == ( i + 1 ) )
            {
            aMmsClient->SetMessageRootL( attaId );
            }
        }

    store->CommitL();
    CleanupStack::PopAndDestroy(); // store

    // This frees all memory and resets all values
    Reset();
    // Reset inactivity timer to keem viewServer from crashing
    User::ResetInactivityTime();
    }


// ---------------------------------------------------------
// CMmsReadFile()::FindAlias
// program build a message from given parts
// ---------------------------------------------------------
//
TInt CMmsReadFile::FindAlias( TPtrC aAlias )
    {
	TBuf<DefaultBufLen> abuf;
	for( TInt i=0; i < iAliasCount; ++i )
	    {
		abuf.Copy( iAliasArray->MdcaPoint(i) );
		abuf.SetLength( abuf.Locate('=') );
		if( ( abuf.CompareF( aAlias ) ) == 0 ) return( i );
	    }
	return( -1 );
    }


// C++ default constructor can NOT contain any code, that
// might leave.
//
CMmsAttaStructure::CMmsAttaStructure()
    {
    iAtta = NULL;
    iAttaType = NULL;
    iAttaName = NULL;
    iAttaCid = NULL;
    iAttaRecommendedName = NULL;
    }


//
void CMmsAttaStructure::ConstructL()
    {
    iAtta = HBufC8::NewL(DefaultBufLen);
    iAttaType = HBufC8::NewL(DefaultBufLen);
    iAttaName = HBufC8::NewL(DefaultBufLen);
    iAttaCid = HBufC8::NewL(DefaultBufLen);
    iAttaRecommendedName = HBufC::NewL(DefaultBufLen);
    iAttaCharset = 0;
   	iXTypeParams  = new(ELeave) CDesC8ArrayFlat(4);
   	iContentTypeParams  = new(ELeave) CDesC8ArrayFlat(4);

    }

// Two-phased constructor.
CMmsAttaStructure* CMmsAttaStructure::NewL()
    {
    CMmsAttaStructure* self = new ( ELeave ) CMmsAttaStructure;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CMmsAttaStructure::~CMmsAttaStructure()
    {
    delete iAtta;
    delete iAttaName;
    delete iAttaType;
    delete iAttaCid;
    delete iAttaRecommendedName;
    if ( iContentTypeParams )
        {
    	iContentTypeParams->Reset();
        }
  	delete iContentTypeParams;
    if ( iXTypeParams )
        {
      	iXTypeParams->Reset();
        }
	delete iXTypeParams;
    }


//  End of File
