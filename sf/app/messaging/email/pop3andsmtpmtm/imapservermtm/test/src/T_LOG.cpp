// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <f32fsys.h>
#include <s32file.h>
#include <e32base.h>
#include <txtetext.h>
#include <txtrich.h>
#include <bautils.h>
#include <miuthdr.h>
#include <miutmsg.h>
#include <miutset.h>  //KUidMsgTypePOP3
#include <imutdll.h>	
#include <imcvtext.h>
#include <imcvrecv.h>
#include <imcvsend.h>

#include <imapset.h>

//Oyster includes
#include <msvstd.h>
#include <msvids.h>
#include <msvuids.h>
#include <msvruids.h>
#include <msvreg.h>

#include <msventry.h> 
#include <msvapi.h>
#include <mtclbase.h>
#include <cemailaccounts.h>
#include <pop3set.h>

#include <offop.h>

#include "t_log.h"

//----------------------------------------------------------------------------------------

_LIT( KFileNameCombined, "Entry_All.txt");

_LIT( KFileNameBody, "Entry_Bodys.txt");
_LIT( KFileNameHeader, "Entry_Header.txt");
_LIT( KFileNameMimeHeader, "Entry_MimeHeader.txt");
_LIT( KFileNameSettings, "Entry_Settings.txt");

_LIT( KFileNameEntryStruct, "Entry_Zac.txt");

//----------------------------------------------------------------------------------------

_LIT(KPerCentS, "%S");
_LIT(KCRLF, "\r\n");


//----------------------------------------------------------------------------------------

// nicked from IMCM::OFFOP.CPP

LOCAL_D const TUid KUidImQueuedOperationList = {0x10001794};	// 2648441492 dec.

//----------------------------------------------------------------------------------------
// Handy conversion routines between 8 and 16bit worlds

#define Ptr8From16( aText ) TPtrC8( (const TUint8 *)(aText).Ptr(), (aText).Size() )
#define Ptr16From8( aText ) TPtrC16( (const TUint16 *)(aText).Ptr(), (aText).Size()/2 )

LOCAL_C HBufC16* HBuf16From8LC( const TDesC8& aText )
	{
	HBufC16* pOutputLine = HBufC16::NewLC( aText.Length() );
	TPtr16 pOutput = pOutputLine->Des();
	pOutput.Zero();

	TUint8 theChar;
	TInt i=0;
	
	while(i<aText.Length())
		{
		theChar=aText[i++];
		pOutput.Append( (TUint16)theChar );
		}

	return pOutputLine;
	}

// Note this function will lose data if fed text other than ISO-8859-1
LOCAL_C HBufC8* HBuf8From16LC( const TDesC16& aText )
	{
	HBufC8* pOutputLine = HBufC8::NewLC( aText.Length() );
	TPtr8 pOutput = pOutputLine->Des();
	pOutput.Zero();

	TUint16 theChar;
	TInt i=0;
	
	while(i<aText.Length())
		{
		theChar=aText[i++];
		pOutput.Append( (TUint8)theChar );
		}

	return pOutputLine;
	}

//----------------------------------------------------------------------------------------

class COutputterOverflow: public TDes16Overflow
	{
public:
	COutputterOverflow( COutputter* aPutter );
	void Overflow(TDes16& aDes);
private:
	COutputter* iPutter;
	};

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

COutputterOverflow::COutputterOverflow( COutputter* aPutter )
	{
	iPutter = aPutter;
	}

void COutputterOverflow::Overflow(TDes16 &aDes)
	{
	TRAPD(ret,iPutter->PrintL( _L("\r\n\r\n\r\n") ));
	TRAP(ret,iPutter->PrintL( _L("Overflow when outputting data")));
	TRAP(ret,iPutter->PrintL( aDes ));
	TRAP(ret,iPutter->PrintL( _L("\r\n\r\n\r\n")));
	}

//----------------------------------------------------------------------------------------

COutputter* COutputter::NewL(RTest* aTest)
	{
	COutputter* putter = new (ELeave) COutputter;
	CleanupStack::PushL(putter);

	putter->iTest = aTest;
	putter->iUseFiles = ETrue;
	putter->ConstructL();

	CleanupStack::Pop();
	return putter;
	}

void COutputter::ConstructL()
	{
	iOverflow = new(ELeave) COutputterOverflow( this );
	iPrefix.FillZ();
	}

COutputter::~COutputter()
	{
	while ( iFilesOpen > 0 )
		UnSetFile();

	delete iOverflow;
	}

void COutputter::SetRFs(RFs* aFs)
	{
	iFs = aFs;
	}

void COutputter::UnSetFile()
	{
	if ( iFilesOpen > 0 )
		iFile[--iFilesOpen].Close();
	}

void COutputter::SetFilePrefix( TUint aNum )
	{
	iPrefix.Format( _L("%08x_"), aNum);
	}

void COutputter::SetFileL( const TDesC& aLeafName, TBool aUnicode )
	{
	if ( iUseFiles )
		{
		if (iFilesOpen >= 4)
			iTest->Panic( _L("Too many open files") );

		RFile& file = iFile[iFilesOpen++];

		TFileName name = iPrefix;
		name += aLeafName;
		
		TInt err = file.Open( *iFs, name, EFileShareAny | EFileStreamText | EFileWrite);
		if (err == KErrNotFound) // file does not exist - create it
			err = file.Create( *iFs, name, EFileShareAny | EFileStreamText | EFileWrite);
		User::LeaveIfError(err);

		// not sure what this is for - but it was in originally
		TInt offset=0;
		(*iTest)(file.Seek(ESeekEnd, offset)==KErrNone);

		iUnicode=aUnicode;
		}
	iPrefix.FillZ();
	}

void COutputter::PrintL( const TDesC8& aText ) // should be L
 	{
	if ( iFilesOpen > 0 )
		{
		if (iUnicode)
			{
			HBufC16 *buf = HBuf16From8LC( aText );
			iFile[iFilesOpen-1].Write( Ptr8From16(*buf) );
			CleanupStack::PopAndDestroy(); // buf
			}
		else
			iFile[iFilesOpen-1].Write( aText );
		}
	else
		{
#if _UNICODE
		HBufC16 *buf = HBuf16From8LC( aText );
		PrintL( *buf );
		CleanupStack::PopAndDestroy(); // buf
#else
		TInt len = aText.Length();
		for ( TInt pos = 0; pos < len; pos += 256 )
			{
			TPtrC ptr = aText.Mid( pos, len-pos<256?len-pos:256 );
			iTest->Printf( KPerCentS, &ptr );
			}
#endif
		}
	}

void COutputter::PrintL( const TDesC16& aText ) // should be L
	{
	if ( iFilesOpen > 0 )
		{
		if (iUnicode)
			iFile[iFilesOpen-1].Write( Ptr8From16( aText ) );
		else
			{
			HBufC8 *buf = HBuf8From16LC( aText );
			iFile[iFilesOpen-1].Write( *buf );
			CleanupStack::PopAndDestroy(); // buf
			}
		}
	else
		{
#if _UNICODE
		// write data in chunks to ensure no overrun of underlying
		// Printf function
		TInt len = aText.Length();
		for ( TInt pos = 0; pos < len; pos += 256 )
			{
			TPtrC ptr = aText.Mid( pos, len-pos<256?len-pos:256 );
			iTest->Printf( KPerCentS, &ptr );
			}
#else
		HBufC8 *buf = HBuf8From16LC( aText );
		PrintL( *buf );
		CleanupStack::PopAndDestroy(); // buf
#endif
		}
	}

void COutputter::Printf( TRefByValue<const TDesC> aFmt, ... )
	{
	TBuf<1024> buf;

	VA_LIST list;
	VA_START( list, aFmt );
	buf.AppendFormatList( aFmt, list, iOverflow );
	VA_END( list );
	
	TRAP_IGNORE(PrintL( buf ));
	}

void COutputter::PrintIfNotNullL( const TDesC& aLabel, const TDesC8& aValue )
	{
	if ( aValue.Length() )
		{
		PrintL( aLabel );
		PrintL( aValue );
		PrintL( KCRLF );
		}	
	}

void COutputter::PrintIfNotNullL( const TDesC& aLabel, const TDesC16& aValue )
	{
	if ( aValue.Length() )
		{
		PrintL( aLabel );
		PrintL( aValue );
		PrintL( KCRLF );
		}	
	}

void COutputter::PrintListL( const TDesC& aLabel, CDesC8Array& aList )
	{
	for(TInt i=0; i < aList.Count(); i++)
		{
		PrintL(aLabel);
		PrintL(aList[i]);
		PrintL(KCRLF);
		}
	}

void COutputter::PrintListL( const TDesC& aLabel, CDesC16Array& aList )
	{
	for(TInt i=0; i < aList.Count(); i++)
		{
		PrintL(aLabel);
		PrintL(aList[i]);
		PrintL(KCRLF);
		}
	}

void COutputter::PrintDataL( const TDesC8& aData )
	{
	TInt i;
	for(i=0; i < aData.Length(); i++)
		{
		Printf(_L("%02x "), aData[i]);
		if ((i&7)==7)
			PrintL(KCRLF);
		}
	if ((i&7)!=0)
		PrintL(KCRLF);
	}

void CMsgOutputter::RemoveRichtextFormatting(TDes& aSourceLine)
	{
	TUint16* ptr = (TUint16*)aSourceLine.Ptr();
	TUint16* start = ptr;

	TInt totalLength = aSourceLine.Length();
	do {
		if(*ptr==CEditableText::EParagraphDelimiter || *ptr==CEditableText::ELineBreak || *ptr==CEditableText::EPageBreak)
			*ptr=0x0A;
		} while((++ptr-start)<totalLength);
	}

//----------------------------------------------------------------------------------------
void CMsgOutputter::BodyDataL( CMsvStore& aFileStore )
//----------------------------------------------------------------------------------------
	{
	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraLayer);

	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charLayer);

	CRichText* body = CRichText::NewL(paraLayer,charLayer);
	CleanupStack::PushL(body);

	aFileStore.RestoreBodyTextL(*body);

	HBufC* pBodyText = HBufC::NewLC(body->DocumentLength()+(body->DocumentLength()/70)+1);

	if ( iParts==EToMultipleFiles )
		iPutter->SetFileL( KFileNameBody );
	iPutter->Printf( _L("*** %08x *************** RichText Data ***************\r\n"), iMsgId);
	iPutter->Printf( _L("Size >>> %d\r\n"), body->DocumentLength());
	
	TPtr pBody = pBodyText->Des();
	body->Extract(pBody, 0);
	RemoveRichtextFormatting(pBody);
	iPutter->PrintL( pBody );
	
	iPutter->PrintL(_L("\r\n********************* end of Body ***********************\r\n"));
	
	CleanupStack::PopAndDestroy(4); // pBodyText, paraLayer, charLayer, body

	if ( iParts==EToMultipleFiles )
		iPutter->UnSetFile();
	}


//----------------------------------------------------------------------------------------
void CMsgOutputter::MimeHeaderL( CMsvStore& aFileStore )
//----------------------------------------------------------------------------------------
	{
	if ( iParts==EToMultipleFiles )
		iPutter->SetFileL( KFileNameMimeHeader );

	iPutter->Printf(_L("*** %08x ************* Mime Header ******************\r\n"), iMsgId);

	CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
	mimeHeader->RestoreL(aFileStore);
	
	iPutter->Printf(_L("Version: %d\r\n"),mimeHeader->Version());

	iPutter->PrintIfNotNullL( _L("Content-Type: "), mimeHeader->ContentType() );
	iPutter->PrintIfNotNullL( _L("Content-Type,SubType: "), mimeHeader->ContentSubType() );
	iPutter->PrintIfNotNullL( _L("Content-Disposition: "), mimeHeader->ContentDisposition() );
	iPutter->PrintIfNotNullL( _L("Content-Description: "), mimeHeader->ContentDescription() );
	iPutter->PrintIfNotNullL( _L("Content-Base: "), mimeHeader->ContentBase() );
	iPutter->PrintIfNotNullL( _L("Content-ID: "), mimeHeader->ContentID() );
	iPutter->PrintIfNotNullL( _L("Content-Location: "), mimeHeader->ContentLocation() );

	if ( mimeHeader->ContentTransferEncoding() )
		iPutter->Printf( _L("Content-Transfer-Encoding: %d\r\n"), mimeHeader->ContentTransferEncoding() );

	iPutter->PrintListL( _L("ContentTypeParams: "), mimeHeader->ContentTypeParams() );
	iPutter->PrintListL( _L("ContentDispositionParams: "), mimeHeader->ContentDispositionParams() );

	iPutter->PrintL(_L("******************** end of MIME Header ****************\r\n"));
	if ( iParts==EToMultipleFiles )
		iPutter->UnSetFile();

	CleanupStack::PopAndDestroy(); // mimeHeader
	}

//----------------------------------------------------------------------------------------
void CMsgOutputter::HeaderL( CMsvStore& aFileStore )
//----------------------------------------------------------------------------------------
	{
	if ( iParts==EToMultipleFiles )
		iPutter->SetFileL( KFileNameHeader );
	iPutter->Printf( _L("*** %08x ************* Rfc822 Header ****************\r\n"), iMsgId);

	CImHeader* header = CImHeader::NewLC();
	
	TUint emptySize = header->DataSize();
	header->RestoreL(aFileStore);

	TUint size = header->DataSize()-emptySize;

	iPutter->Printf(_L("Size >>> %d\r\n"), size);
	iPutter->PrintIfNotNullL(_L("Subject: "), header->Subject());
	iPutter->PrintIfNotNullL(_L("ID: "), header->ImMsgId());
	iPutter->PrintIfNotNullL(_L("From: "), header->From());
	iPutter->PrintIfNotNullL(_L("Reply-To: "), header->ReplyTo());
	iPutter->PrintIfNotNullL(_L("Return-Receipt-To: "), header->ReceiptAddress());

	iPutter->PrintListL(_L("To: "), header->ToRecipients());
	iPutter->PrintListL(_L("Cc: "), header->CcRecipients());
	iPutter->PrintListL(_L("Bcc: "), header->BccRecipients());
	
	iPutter->PrintL(_L("******************** end of RFC822 header **************\r\n"));

	if ( iParts==EToMultipleFiles )
		iPutter->UnSetFile();
	CleanupStack::PopAndDestroy(); // header
	}

TPtrC CMsgOutputter::SyncTypeString( TFolderSyncType aType )
	{
	switch (aType)
		{
	case EUseCombination:
		return _L("UseCombination");
		
	case EUseLocal:
		return _L("UseLocal");

	case EUseRemote:
		return _L("UseRemote");
		}
	return _L("");
	}

TPtrC CMsgOutputter::SubscribeTypeString( TFolderSubscribeType aType )
	{
	switch (aType)
		{
	case EUpdateNeither:
		return _L("UpdateNeither");
		
	case EUpdateLocal:
		return _L("UpdateLocal");

	case EUpdateRemote:
		return _L("UpdateRemote");

	case EUpdateBoth:
		return _L("UpdateBoth");
		}
	return _L("");
	}

TPtrC CMsgOutputter::GetMailOptionsString( TImap4GetMailOptions aOpts )
	{
	switch (aOpts)
		{
	case EGetImap4EmailHeaders:
		return _L("GetImap4EmailHeaders");
	case EGetImap4EmailBodyText:
		return _L("GetImap4EmailBodyText");
	case EGetImap4EmailBodyTextAndAttachments:
		return _L("GetImap4EmailBodyTextAndAttachments");
	case EGetImap4EmailAttachments:
		return _L("GetImap4EmailAttachments");
		}
	return _L("");
	}

//----------------------------------------------------------------------------------------
void CMsgOutputter::Imap4SettingsL(const TMsvEntry& aEntry)
//----------------------------------------------------------------------------------------
	{
	if ( iParts==EToMultipleFiles )
		iPutter->SetFileL( KFileNameSettings );
	iPutter->Printf(_L("*** %08x ************* Settings ****************\r\n"), iMsgId);

	CImImap4Settings* settings = new (ELeave)CImImap4Settings();
	CleanupStack::PushL(settings);
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	
	TImapAccount imapAccountId;
	accounts->GetImapAccountL(aEntry.Id(), imapAccountId);
	accounts->LoadImapSettingsL(imapAccountId, *settings);
	CleanupStack::PopAndDestroy(accounts);

	iPutter->Printf(         _L("IAP index:                     %u\r\n"), iEntry->Entry().MtmData1());
	iPutter->PrintIfNotNullL( _L("LoginName:                     "), settings->LoginName());
	iPutter->PrintIfNotNullL( _L("Password:                      "), settings->Password());
	iPutter->PrintIfNotNullL( _L("FolderPath:                    "), settings->FolderPath());
	iPutter->Printf(         _L("PathSeparator:                 %c\r\n"), settings->PathSeparator());
	iPutter->Printf(         _L("DisconnectedUserMode:          %u\r\n"), settings->DisconnectedUserMode()?1:0);
	iPutter->PrintIfNotNullL( _L("Synchronise:                   "), SyncTypeString(settings->Synchronise()));
	iPutter->PrintIfNotNullL( _L("Subscribe:                     "), SubscribeTypeString(settings->Subscribe()));
	iPutter->Printf(         _L("AutoSendOnConnect:             %u\r\n"), settings->AutoSendOnConnect()?1:0);
	iPutter->Printf(         _L("MaxEmailSize:                  %u\r\n"), settings->MaxEmailSize());
	iPutter->Printf(         _L("DeleteEmailsWhenDisconnecting: %u\r\n"), settings->DeleteEmailsWhenDisconnecting()?1:0);
	iPutter->Printf(         _L("AcknowledgeReceipts:           %u\r\n"), settings->AcknowledgeReceipts()?1:0);
	iPutter->PrintIfNotNullL( _L("GetMailOptions:                "), GetMailOptionsString(settings->GetMailOptions()));

	iPutter->PrintIfNotNullL( _L("Server:                        "), settings->ServerAddress());
	iPutter->Printf(         _L("Port:                          %u\r\n"), settings->Port());
	iPutter->Printf(         _L("Secure:                        %u\r\n"), settings->SecureSockets()?1:0);
	
	iPutter->PrintL(_L("******************** end of Settings **************\r\n"));
	if ( iParts==EToMultipleFiles )
		iPutter->UnSetFile();

	CleanupStack::PopAndDestroy(); // settings
	}


//----------------------------------------------------------------------------------------
void CMsgOutputter::SettingsL(const TMsvEntry& aEntry)
//----------------------------------------------------------------------------------------
	{
	if ( iParts==EToMultipleFiles )
		iPutter->SetFileL( KFileNameSettings );
	iPutter->Printf(_L("*** %08x ************* Settings ****************\r\n"), iMsgId);

	CImBaseEmailSettings* settings(NULL);
	
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	
	
	if (aEntry.iMtm == KUidMsgTypeIMAP4)
		{
		CImImap4Settings* imap4Settings = new(ELeave) CImImap4Settings;
		TImapAccount imapAccountId;
		accounts->GetImapAccountL(aEntry.Id(), imapAccountId);
		accounts->LoadImapSettingsL(imapAccountId, *imap4Settings);
		settings = imap4Settings;
		}
	else if (aEntry.iMtm == KUidMsgTypePOP3)
		{
		CImPop3Settings* pop3Settings = new(ELeave) CImPop3Settings;
		TPopAccount pop3AccountId;
		accounts->GetPopAccountL(aEntry.Id(), pop3AccountId);
		accounts->LoadPopSettingsL(pop3AccountId, *pop3Settings);
		settings = pop3Settings;
		}
	else if (aEntry.iMtm == KUidMsgTypeSMTP)
		{
		CImSmtpSettings* smtpSettings = new(ELeave) CImSmtpSettings;
		TSmtpAccount smtpAccountId;
		accounts->GetSmtpAccountL(aEntry.Id(), smtpAccountId);
		accounts->LoadSmtpSettingsL(smtpAccountId, *smtpSettings);
		settings = smtpSettings;
		}
	else
		{
		User::Leave(KErrNotFound);
		}

	iPutter->Printf(         _L("IAP index: %u\r\n"), iEntry->Entry().MtmData1());
	iPutter->PrintIfNotNullL( _L("Server:    "), settings->ServerAddress());
	iPutter->Printf(         _L("Port:      %u\r\n"), settings->Port());
	iPutter->Printf(         _L("Secure:    %u\r\n"), settings->SecureSockets());
	
	iPutter->PrintL(_L("******************** end of Settings **************\r\n"));
	if ( iParts==EToMultipleFiles )
		iPutter->UnSetFile();
	
	CleanupStack::PopAndDestroy(); // settings

	CleanupStack::PopAndDestroy(accounts);
	}


TPtrC CMsgOutputter::OffLineOpTypeString(CImOffLineOperation::TOffLineOpType op)
	{
	switch (op)
		{
	case CImOffLineOperation::EOffLineOpNone:
		return _L("None");

	case CImOffLineOperation::EOffLineOpCopyToLocal:
		return _L("CopyToLocal");
	case CImOffLineOperation::EOffLineOpCopyFromLocal:
		return _L("CopyFromLocal");
	case CImOffLineOperation::EOffLineOpCopyWithinService:
		return _L("CopyWithinService");

	case CImOffLineOperation::EOffLineOpMoveToLocal:
		return _L("MoveToLocal");
	case CImOffLineOperation::EOffLineOpMoveFromLocal:
		return _L("MoveFromLocal");
	case CImOffLineOperation::EOffLineOpMoveWithinService:
		return _L("MoveWithinService");

	case CImOffLineOperation::EOffLineOpDelete:
		return _L("Delete");

	case CImOffLineOperation::EOffLineOpChange:
		return _L("Change");
	case CImOffLineOperation::EOffLineOpCreate:
		return _L("Create");

	case CImOffLineOperation::EOffLineOpMtmSpecific:
		return _L("MtmSpecific");

	default:
		break;
		}
	return _L("Unknown");
	}

//----------------------------------------------------------------------------------------
void CMsgOutputter::QueuedOpsL( CMsvStore& aFileStore )
//----------------------------------------------------------------------------------------
	{
	if ( iParts==EToMultipleFiles )
		iPutter->SetFileL( KFileNameSettings );
	iPutter->Printf(_L("*** %08x ************* Queued operations ****************\r\n"), iMsgId);

	CImOffLineOperationArray* ops=CImOffLineOperationArray::NewL();
	CleanupStack::PushL(ops);

	CImOffLineArrayStore arraystore(*ops);
	arraystore.RestoreL(aFileStore);
	
	for (TInt opNum=0; opNum < ops->CountOperations(); opNum++)
		{
		const CImOffLineOperation& op = ops->Operation(opNum);
		iPutter->PrintL(         _L("------------------\r\n"));
		iPutter->Printf(        _L("Operation %d\r\n"), opNum);
		iPutter->PrintIfNotNullL(_L("OpType:   "), OffLineOpTypeString(op.OpType()));
		iPutter->Printf(        _L("Message:  %08x\r\n"), op.MessageId());
		iPutter->Printf(        _L("Target:   %08x\r\n"), op.TargetMessageId());
		if (op.OpType() == CImOffLineOperation::EOffLineOpMtmSpecific)
			{
			iPutter->Printf(        _L("Function: %d\r\n"), op.MtmFunctionId());
			iPutter->Printf(        _L("Data:     %d bytes\r\n"), op.MtmParameters().Length());
			iPutter->PrintDataL(op.MtmParameters());
			}
		}
	
	iPutter->PrintL(_L("******************** end of Queued operations **************\r\n"));
	if ( iParts==EToMultipleFiles )
		iPutter->UnSetFile();
	
	CleanupStack::PopAndDestroy(); // ops
	}


//----------------------------------------------------------------------------------------

TPtrC CMsgOutputter::TypeString( const TMsvEmailEntry& entry )
	{
	TPtrC type;
	if(entry.iType == KUidMsvRootEntry)
		type.Set(_L("ROOT"));
	else if(entry.iType == KUidMsvServiceEntry)
		type.Set(_L("SERVICE"));
	else if(entry.iType == KUidMsvFolderEntry)
		type.Set(_L("FOLDER"));
	else if(entry.iType == KUidMsvMessageEntry)
		type.Set(_L("MESSAGE"));
	else if(entry.iType == KUidMsvEmailTextEntry)
		type.Set(_L("TEXT"));
	else if(entry.iType == KUidMsvAttachmentEntry)
		type.Set(_L("ATTACHMENT"));
	else if(entry.iType == KUidMsvEmailHtmlEntry)
		type.Set(_L("HTML"));
	else
		type.Set(_L("UNKNOWN"));
	return type;
	}

TPtrC CMsgOutputter::PriorityString( const TMsvEmailEntry& entry )
	{
	TPtrC priority;
	TMsvPriority temp=entry.Priority();
	if(temp==EMsvHighPriority)
		priority.Set(_L("Hi"));
	else if(temp==EMsvMediumPriority)
		priority.Set(_L("Me"));
	else if(temp==EMsvLowPriority)
		priority.Set(_L("Lo"));
	else
		priority.Set(_L("No"));
	return priority;
	}

TPtrC CMsgOutputter::DisconnectedOpString( const TMsvEmailEntry& entry )
	{
	TImDisconnectedOperationType opType = entry.DisconnectedOperation();
	switch (opType)
		{
	case ENoDisconnectedOperations:
		return _L("Non");
	case EDisconnectedCreateOperation:
		return _L("Cre");
	case EDisconnectedDeleteOperation:
		return _L("Del");
	case EDisconnectedChangeOperation:
		return _L("Cha");
	case EDisconnectedCopyToOperation:
		return _L("CTo");
	case EDisconnectedCopyFromOperation:
		return _L("CFr");
	case EDisconnectedCopyWithinServiceOperation:
		return _L("CWi");
	case EDisconnectedMoveToOperation:
		return _L("MTo");
	case EDisconnectedMoveFromOperation:
		return _L("MFr");
	case EDisconnectedMoveWithinServiceOperation:
		return _L("MWi");
	case EDisconnectedSpecialOperation:
		return _L("Spe");
	case EDisconnectedUnknownOperation:
		return _L("Unk");
	case EDisconnectedMultipleOperation:
		return _L("Mul");
	default:
		break;
		}
	return _L("");
	}

void CMsgOutputter::FlagsString( TBuf<8>& other, const TMsvEmailEntry& entry )
	{
	other = _L("--------");

	if(entry.Visible())
		other.Replace(0, 1, _L("V"));
	if(entry.Complete())
		other.Replace(7, 1, _L("C"));

	if (entry.iType == KUidMsvFolderEntry)
		{
		if(entry.Subscribed())
			other.Replace(1, 1, _L("S")); // subscribed on server
		if(entry.LocalSubscription())
			other.Replace(2, 1, _L("L")); // subscribed to locally
		if(entry.ValidUID())
			other.Replace(3, 1, _L("I"));
		}
	else
		{
		if(entry.New())
			other.Replace(1, 1, _L("N"));
		if(entry.Unread())
			other.Replace(2, 1, _L("U"));
		if(entry.Receipt())
			other.Replace(3, 1, _L("R"));
		if(entry.Orphan())
			other.Replace(4, 1, _L("O"));
		if(entry.MHTMLEmail())
			other.Replace(5, 1, _L("M"));
		if(entry.Attachment())
			other.Replace(6, 1, _L("A"));
		}
	}

void CMsgOutputter::IMAP4FlagsString( TBuf<7>& other, const TMsvEmailEntry& entry )
	{
	other = _L("-------");

	if(entry.UnreadIMAP4Flag())
		other.Replace(0, 1, _L("U"));
	if(entry.SeenIMAP4Flag())
		other.Replace(1, 1, _L("S"));
	if(entry.AnsweredIMAP4Flag())
		other.Replace(2, 1, _L("A"));
	if(entry.FlaggedIMAP4Flag())
		other.Replace(3, 1, _L("F"));
	if(entry.DeletedIMAP4Flag())
		other.Replace(4, 1, _L("X"));
	if(entry.DraftIMAP4Flag())
		other.Replace(5, 1, _L("D"));
	if(entry.RecentIMAP4Flag())
		other.Replace(6, 1, _L("R"));
	}

void CMsgOutputter::OutputPartsL(TBuf<5>& aStreams)
	{
	aStreams=_L("-----");

	if(iEntry->HasStoreL())
		{
		CMsvStore* fileStore = iEntry->ReadStoreL();
		CleanupStack::PushL(fileStore);
		
		TMsvEmailEntry entry = iEntry->Entry(); // Get Details

		iPutter->SetFilePrefix(entry.Id());
		
		if (iParts == EToOneFile)
			iPutter->SetFileL(KFileNameCombined);
		
		// messages
		if(fileStore->IsPresentL(KUidMsgFileIMailHeader))
			{
			if (iParts!=ENone)
				HeaderL(*fileStore);
			aStreams.Replace(0, 1, _L("H"));
			}
		if(fileStore->IsPresentL(KUidMsgFileMimeHeader))
			{
			if (iParts!=ENone)
				MimeHeaderL(*fileStore);
			aStreams.Replace(1, 1, _L("M"));
			}
		if(fileStore->IsPresentL(KMsvEntryRichTextBody))
			{
			if (iParts!=ENone)
				BodyDataL(*fileStore);
			aStreams.Replace(2, 1, _L("B"));
			}
		if(entry.Attachment())
			aStreams.Replace(3, 1, _L("A"));

		// services
		if(fileStore->IsPresentL(KUidMsvServiceEntry))
			{
			if (iParts!=ENone)
				{
				if (entry.iMtm==KUidMsgTypeIMAP4)
					Imap4SettingsL(entry);
				else
					SettingsL(entry);
				}
			aStreams.Replace(0, 1, _L("S"));
			}

		// folders and services
		if(fileStore->IsPresentL(KUidImQueuedOperationList))
			{
			if (iParts!=ENone)
				QueuedOpsL(*fileStore);
			aStreams.Replace(1, 1, _L("Q"));
			}

		if (iParts == EToOneFile)
			iPutter->UnSetFile();

		CleanupStack::PopAndDestroy(); // fileStore
		}
	}

CMsgOutputter* CMsgOutputter::NewL( COutputter* aPutter, CMsvServerEntry* aEntry )
	{
	CMsgOutputter* self = new(ELeave) CMsgOutputter(aPutter,aEntry);
	return self;
	}

CMsgOutputter::CMsgOutputter( COutputter* aPutter, CMsvServerEntry* aEntry )
	: iPutter(aPutter), iEntry(aEntry)
	{
	}

CMsgOutputter::~CMsgOutputter()
	{
	}

void CMsgOutputter::DumpL( TMsvId aId, TInt aSummary,
						   TInt aParts,
						   TBool aRecurse, TBool aShowIds,
						   MsgMatchFn aMatch )
	{
	iEntry->SetEntry(aId);

	iSummary = aSummary;
	iParts = aParts;
	iRecurse = aRecurse;
	iShowIds = aShowIds;
	iMatch = aMatch;

	if (iSummary == EToOneFile)
		{
		iPutter->SetFileL( KFileNameEntryStruct );
		iPutter->PrintL( _L8("Store: (H)eader (M)ime header (B)ody (A)ttachment (S)ervice data (Q)ueuedops\r\n") );
		iPutter->PrintL( _L8("Flags: (V)isible (C)omplete (S)ubscribed (L)ocalsubscribe validu(I)d\r\n") );
		iPutter->PrintL( _L8("Flags: (N)ew (U)nread (R)eceipt (O)rphan (M)html (A)ttachment\r\n") );
		iPutter->PrintL( _L8("IMAP4: (U)nread (S)een (A)nswered (F)lagged (X)Deleted (D)raft (R)ecent\r\n") );
		iPutter->PrintL( _L8("\r\n") );
		}

	__UHEAP_MARK;
	DumpRecurseL();
	__UHEAP_MARKEND;

	if (iSummary == EToOneFile)
		iPutter->UnSetFile();

	// park
	iEntry->SetEntry(KMsvNullIndexEntryId);
	}

void CMsgOutputter::DumpRecurseL()
	{
	const TMsvEmailEntry entry = iEntry->Entry(); // Get Details

	// we may not want to show ids because they are not persistent
	// over message store wipes
	iMsgId = iShowIds ? entry.Id() : 0;

//	TBool matchThis = iMatch ? (*iMatch)(entry) : ETrue;
	TBool matchThis = iMatch ? (*iMatch)(iEntry) : ETrue;

	TBuf<5> streams;
	if (matchThis)
		OutputPartsL(streams);
	
	// find the children
	CMsvEntrySelection* msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);
	User::LeaveIfError( iEntry->GetChildren(*msvSelection) );
	
	TInt attachCount = msvSelection->Count(); 	// For each child ..

	if (matchThis && iSummary != ENone)
		{
		// get information about the entry
		TPtrC type = TypeString( entry );
		TPtrC priority = PriorityString( entry );

		TBuf<7> imap4flags;
		IMAP4FlagsString(imap4flags, entry);

		TBuf<8> other;
		FlagsString(other, entry);

		TPtrC discOp = DisconnectedOpString(entry);
		
		// indent according to depth
		TInt i;
		for(i=0; i<iDepth; i++)
			iPutter->PrintL(_L(" "));

		iPutter->Printf( _L("%S"), &type );

		for(i=iDepth + type.Length(); i<8+10; i++)
			iPutter->PrintL(_L(" "));

		iPutter->Printf( _L(" %08x [%3d], Size=%8d, Store=%S, D=%S, P=%S, F=%S, I4F=%S, R=%08x, uid=%08x, mtm=%08x"),
						 entry.Id(),
						 attachCount, 
						 entry.iSize,
						 &streams,
						 &discOp,
						 &priority,
						 &other,
						 &imap4flags,
						 entry.iRelatedId,
						 entry.UID(),
						 entry.iMtm);

		iPutter->Printf( _L(", '%S'"), &entry.iDescription );
		iPutter->Printf( _L(", '%S'"), &entry.iDetails );
		iPutter->PrintL( KCRLF );
		}
	
	if ( iRecurse )
		{
		iDepth++;

		for(TInt j=0; j<attachCount; j++)
			{
			iEntryCount++;

			iEntry->SetEntry((*msvSelection)[j]);
		
			__UHEAP_MARK;
			DumpRecurseL();
			__UHEAP_MARKEND;
			}	

		iDepth--;
		}

	CleanupStack::PopAndDestroy(); // msvSelection
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

class CTextFile : public RFile
	{
public:
	CTextFile();
	~CTextFile();

	TInt ReadChar(TDes& aDes);
	TPtrC ReadLineL();
	void SetUnicodeL();
	
private:
	HBufC *iBuf;
	TBool iFileIsUnicode;
	};

CTextFile::CTextFile()
	{
#if _UNICODE
	iFileIsUnicode = ETrue;
#else
	iFileIsUnicode = EFalse;
#endif
	}

CTextFile::~CTextFile()
	{
	if (iBuf != NULL)
		delete iBuf;
	}

void CTextFile::SetUnicodeL()
	{
	TBuf8<2> c;
	User::LeaveIfError(Read(c,2));

	TInt pos = 0;
	User::LeaveIfError(Seek(ESeekAddress,pos));

	// OK this is not guaranteed to work but it will in all cases
	// where we have created the files
	iFileIsUnicode = c[1] == 0;
	}

TInt CTextFile::ReadChar(TDes& aDes)
	{
	TInt err;
	if (iFileIsUnicode)
		{
		TBuf8<2> c;
		err = Read(c, 2);
	
		aDes[0] = c[0];
#if _UNICODE
		aDes[0] |= c[1] << 8;
		// truncate the high byte if in non-Unicode build
#endif
		}
	else
		{
#if _UNICODE
		TBuf8<1> c;
		err = Read(c, 1);
		aDes[0] = c[0];
#else
		err = Read(aDes, 1);
#endif
		}

	return err;
	}

TPtrC CTextFile::ReadLineL()
	{
	if (iBuf == NULL)
		iBuf = HBufC::NewL(512);

	TBuf<1> c;
	TBool finished = EFalse;
	do
		{
		User::LeaveIfError(ReadChar(c));
		
		if (c.Length() && c[0] != 0xA0)
			{
			TPtr16 des = iBuf->Des();
			if (iBuf->Length() == des.MaxLength())
				iBuf = iBuf->ReAllocL(iBuf->Length()*2);
			
			des.Append(c);
			}
		else
			finished = ETrue;
		}
	while (!finished);

	return iBuf->Des();
	}

//----------------------------------------------------------------------------------------

// eof T_LOG.CPP
