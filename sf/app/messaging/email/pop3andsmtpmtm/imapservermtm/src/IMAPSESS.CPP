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
// Deal with a connection to an IMAP4rev1 server
// This involves logging in, issuing commands and parsing responses into
// suitable return formats as necessary, and updating the message server's
// database.
// 
//

#include <msvstd.h>
#include <miuthdr.h>
#include <miut_err.h>
#include <msventry.h>
#include <imcvutil.h>

#include <txtetext.h>

#include <cmsvbodytext.h>
#include <txtrich.h>
#include <imcvtext.h>
#include <imcvsend.h>
#include <imcm.rsg>		// resource definition for IMCV
#include <barsread.h>	// TResourceReader
#include <msvapi.h>

#include "impsmtm.h"

#include "imapsess.h"
#include "impspan.h"
#include "fldindex.h"
#include <imapset.h>
#include <miutlog.h>
#include <msvstore.h>
#include <commdb.h>
#include <commdbconnpref.h>
#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <cemailaccounts.h>
#ifdef __WINS__
#include <e32wins.h>    // for maxfilename lengths
#include <msvapi.h>
#endif

#ifdef _DEBUG
#define LOG_COMMANDS(a) a
#define DBG(a) a
#define PRINTING
#else
#define LOG_COMMANDS(a)
#define DBG(a)
#undef PRINTING
#endif

#include "cimapcanceltimer.h"
#include "mimapsessionobserver.h"

// For the "UID SEARCH" command, the reply is of the form:
// "* SEARCH 1234547890 1234567891 ......."
//      :         :
//  |---9---|	  :
//           |---11----|
//
// Here the untagged headers is 9 characters and each UID part may be up to 11 characters.
// So if we wanted to limit the number of replies recieved in a single search so that it is
// no larger is size than a single body fetch part....

// Maximum number of UIDs acceptable in a single reply.
const TInt KImapUidSearchSize=(5120-9)/11;

// Initial size of buffer in which the "Real name <user@host>" strings
// are built before they're put into the CImHeader and the size by
// which to increment the buffer when necessary
const TInt KImapAddressSizeInc=256;

// Initial size of the buffer containing the UID Search list
const TInt KUidListStringSize=256;

// Illegal UID we use for marker
const TUint KIllegalUID	= 0xffffffff;

// Idle time when waiting for a cancelled fetch to complete (microseconds)
const TInt KImapFetchCancelIdleTime	= 10000000;	// 10 seconds

// Idle time (seconds) when waiting for a DONE command to complete
// when coming out of IMAP IDLE state.
const TInt KImapDoneInactivityTimeSeconds = 5;

// IMAP text
_LIT8(KIMAP_UNTAGGED, "*");
_LIT8(KIMAP_CONTINUATION, "+");
_LIT8(KIMAP_BODY, "BODY");
_LIT8(KIMAP_BODYPEEK, "BODY.PEEK");
_LIT8(KIMAP_BODYSTRUCTURE, "BODYSTRUCTURE");
_LIT8(KIMAP_BYE, "BYE");
_LIT8(KIMAP_CAPABILITY, "CAPABILITY");
_LIT8(KIMAP_EXISTS, "EXISTS");
_LIT8(KIMAP_EXPUNGE, "EXPUNGE");
_LIT8(KIMAP_FETCH, "FETCH");
_LIT8(KIMAP_FLAGS, "FLAGS"); 
_LIT8(KIMAP_HEADERFIELDS, "HEADER.FIELDS");

_LIT8(KIMAP_ALERT, "ALERT");
_LIT8(KIMAP_LIST, "LIST");
_LIT8(KIMAP_LSUB, "LSUB");
_LIT8(KIMAP_NIL, "NIL");
_LIT8(KIMAP_NO, "NO");
_LIT8(KIMAP_PREAUTH, "PREAUTH");
_LIT8(KIMAP_READWRITE, "READ-WRITE");
_LIT8(KIMAP_READONLY, "READ-ONLY");
_LIT8(KIMAP_RECENT, "RECENT");
_LIT8(KIMAP_RFC822SIZE, "RFC822.SIZE");
_LIT8(KIMAP_UID, "UID");
_LIT8(KIMAP_UIDVALIDITY, "UIDVALIDITY");
_LIT8(KIMAP_UIDNEXT, "UIDNEXT");
_LIT8(KIMAP_MIME, "MIME");
_LIT8(KIMAP_SEARCH, "SEARCH");

// IMAP capabilities
_LIT8(KIMAP_VERSION, "IMAP4rev1");
_LIT8(KIMAP_STARTTLS, "STARTTLS");
_LIT8(KIMAP_LOGINDISABLED, "LOGINDISABLED");
_LIT8(KIMAP_IDLE, "IDLE");

// IMAP commands (trailing spaces, except for those which take no params)
_LIT8(KIMAPC_CLOSE, "CLOSE");
_LIT8(KIMAPC_LOGOUT, "LOGOUT");
_LIT8(KIMAPC_SUBSCRIBE, "SUBSCRIBE ");
_LIT8(KIMAPC_UNSUBSCRIBE, "UNSUBSCRIBE ");
_LIT8(KIMAPC_IDLE, "IDLE");
_LIT8(KIMAPC_DONE, "DONE");

// IMAP flags
_LIT8(KIMAPFLAG_NOSELECT, "\\Noselect");
_LIT8(KIMAPFLAG_NOINFERIORS, "\\Noinferiors");
_LIT8(KIMAPFLAG_ANSWERED, "\\Answered");
_LIT8(KIMAPFLAG_DELETED, "\\Deleted");
_LIT8(KIMAPFLAG_DRAFT, "\\Draft");
_LIT8(KIMAPFLAG_FLAGGED, "\\Flagged");
_LIT8(KIMAPFLAG_RECENT, "\\Recent");
_LIT8(KIMAPFLAG_SEEN, "\\Seen");
_LIT8(KIMAPFLAG_UNREAD, "\\Unread");

// MIME message types
_LIT8(KMIME_MESSAGE, "MESSAGE");
_LIT8(KMIME_RFC822, "RFC822");
_LIT8(KMIME_TEXT, "TEXT");
_LIT8(KMIME_HTML, "HTML");
_LIT8(KMIME_XVCARD, "X-VCARD");
_LIT8(KMIME_VCALENDAR, "X-VCALENDAR");
_LIT8(KMIME_ICALENDAR, "CALENDAR");
_LIT8(KMIME_NAME, "NAME");
_LIT8(KMIME_NAME_RFC2231, "NAME*");
_LIT8(KMIME_FILENAME, "FILENAME");
_LIT8(KMIME_FILENAME_RFC2231, "FILENAME*");
_LIT8(KMIME_ATTACHMENT, "ATTACHMENT");
_LIT8(KMIME_DELIVERY_STATUS, "DELIVERY-STATUS");
_LIT8(KMIME_ALTERNATIVE, "ALTERNATIVE");
_LIT8(KMIME_RELATED, "RELATED");
_LIT8(KMIME_IMAGE, "IMAGE");
_LIT8(KMIME_AUDIO, "AUDIO");
_LIT8(KMIME_VIDEO, "VIDEO");
_LIT8(KMIME_APPLICATION, "APPLICATION");

// Encoding types
_LIT8(KMIME_BASE64, "BASE64");

// Multipart types
_LIT8(KMIME_MIXED, "MIXED");

// for first stage download (to view in folder list)
_LIT8(KImapFetchSmallHeaderToEnd, "%d UID FETCH %d:* (UID FLAGS BODYSTRUCTURE BODY.PEEK[HEADER.FIELDS (Received Date Subject From %S)])\r\n");
_LIT8(KImapFetchSmallHeaderRange,"%d UID FETCH %d:%d (UID FLAGS BODYSTRUCTURE BODY.PEEK[HEADER.FIELDS (Received Date Subject From %S)])\r\n");
_LIT8(KImapFetchSmallHeaderRangeRefined,"%d UID FETCH %S (UID FLAGS BODYSTRUCTURE BODY.PEEK[HEADER.FIELDS (Received Date Subject From %S)])\r\n");

// for second stage download (to view when downloading whole email)
_LIT8(KImapFetchLargeHeader, "%d UID FETCH %d (UID FLAGS BODYSTRUCTURE BODY.PEEK[HEADER.FIELDS (Received Date Subject From Reply-to To Cc Bcc Message-ID %S)])\r\n");
_LIT8(KImapFetchLargeHeaderRange, "%d UID FETCH %d:* (UID FLAGS BODYSTRUCTURE BODY.PEEK[HEADER.FIELDS (Received Date Subject From Reply-to To Cc Bcc Message-ID %S)])\r\n");

// Use %S so we can specify a refined FETCH based on a previous refined SEARCH
_LIT8(KImapFetchLargeHeaderRangeRefined, "%d UID FETCH %S (UID FLAGS BODYSTRUCTURE BODY.PEEK[HEADER.FIELDS (Received Date Subject From Reply-to To Cc Bcc Message-ID %S)])\r\n");
// Constants for fetching body
_LIT8(KImapFetchBodyPeek, "%d UID FETCH %d (BODY.PEEK[%S]<%d.%d>)\r\n");
_LIT8(KImapFetchBody,     "%d UID FETCH %d (BODY[%S]<%d.%d>)\r\n");
_LIT8(KImapFetchMimeBodyPeek, "%d UID FETCH %u (BODY.PEEK[%S]<0.%d> BODY.PEEK[%S.MIME])\r\n");
_LIT8(KImapFetchMimeBody,     "%d UID FETCH %u (BODY[%S]<0.%d> BODY[%S.MIME])\r\n");

// Constants for sending commands 
_LIT8(KImapCommand, "%S\r\n");

// The maximum number of octets to be uuencoded on each line is 45
const TInt KUuDecodedLineLength = 45;	

const TInt KBodyTextChunkSizeBytes = 512;
const TInt KKiloByteSize = 1024;
// UIDs are  32bit integers, which takes maximum 10 decimal digits.
// KMaxUint32Chars is used to check whether the string containg the UID 
// list has allocated enough memory when creating it.
const TInt KMaxUint32Chars = 10;

// Processing to remove illegal characters from a filename
LOCAL_C void StripIllegalCharactersFromFileName(TDes16& aName)
    {
    TInt length=aName.Length();
    for(TInt index=0; index < length; index++)
        {
		//parse extracted filename and replace any illegal chars  with a default
        TUint charr=(TUint)aName[index];
        if(	charr == '*' || charr == '\\' || charr == '<' || charr == '>' ||
            charr == ':'  || charr == '"' || charr == '/' || charr == '|' ||
			charr == '?' || charr < ' ')
            {
            aName[index] = KImcvDefaultChar;
            }
        }
   }


// We don't do the async notifications yet: they make everything a bit more wobbly
// SJM actually I guess we do do them now!
#define ASYNC_NOTIFICATIONS

// do we automatically set the iRelatedId of each object to itself? No
// for now as it confuses the offline handling code which assumes it
// is a shadow entry if iRelatedId is set.
#define SET_RELATED_ID	0

// This is very nasty but necessary as the flag returning functions
// return 0 or not-zero not 0 or 1

#define FIXBOOL(a)		(a?ETrue:EFalse)

// Directory structure
CImImap4DirStruct::CImImap4DirStruct()
	{
	}

CImImap4DirStruct::~CImImap4DirStruct()
	{
	// Get rid of leaf
	delete iLeafname;
	}

void CImImap4DirStruct::SetLeafnameL(const TDesC& aName)
	{
	// Make buffer, set it
	iLeafname=HBufC::NewL(aName.Length());
	*iLeafname=aName;
	}

TPtrC CImImap4DirStruct::Leafname()
	{
	// Return it
	return(*iLeafname);
	}

CImImap4Session::CImImap4Session(MImapSessionObserver& aObserver) // construct high-priority active object
	: CMsgActive(1), iState(EImapStateDisconnected), iCancelledTag(-1),
	  iSecurityState(EUnknown), iCharset(KCharacterSetIdentifierUtf8),
	  iObserver(aObserver),iPrimarySession(NULL)
	{
	__DECLARE_NAME(_S("CImImap4Session"));
	}

CImImap4Session* CImImap4Session::NewLC(TInt aId, MImapSessionObserver& aObserver)

	{
	CImImap4Session* self=new (ELeave) CImImap4Session(aObserver);

	CleanupStack::PushL(self);
	self->ConstructL(aId);
	return self;
	}

CImImap4Session* CImImap4Session::NewL(TInt aId, MImapSessionObserver& aObserver)
	{
	CImImap4Session* self=NewLC(aId, aObserver);
	CleanupStack::Pop();
	return self;
	}

void CImImap4Session::ConstructL(TInt aId)
	{
	// Add to active scheduler
	CActiveScheduler::Add(this);

	// Get FS
	User::LeaveIfError(iFs.Connect());

	// Get somewhere to store service settings
	iServiceSettings=new (ELeave) CImImap4Settings;

	// Get an IO processor
	iImapIO=CImapIO::NewL(aId);

	// Message selection
	iSelection=new (ELeave) CMsvEntrySelection;

	// List of messages to delete on folder close
	iDeletedUids=new (ELeave) CArrayFixFlat<TMsvId>(8);

	// List of messages to fetch in single fetch operation
	iFetchList=new (ELeave) CArrayFixFlat<TMsvId>(8);

	// List of messages who's Seen status has changed
	iSetSeenList=new (ELeave) CArrayFixFlat<TMsvId>(8);
	iClearSeenList=new (ELeave) CArrayFixFlat<TMsvId>(8);

	RResourceFile resFile;
	OpenResourceFileL(resFile,iFs);	// NB leaves if file not found

	// make sure the resource file will be closed if anything goes wrong
	// CloseResourceFile is declared in IMCVDLL.H and defined in IMCVDLL.CPP
	TCleanupItem close(CloseResourceFile,&resFile);
	CleanupStack::PushL(close);
		
	// Read iStore8BitData flag.
	HBufC8* buf = resFile.AllocReadLC( STORE_8BIT_BODY_TEXT );
	TResourceReader reader;
	reader.SetBuffer(buf);
	iStore8BitData = reader.ReadInt8();
	CleanupStack::PopAndDestroy(buf);

	buf=resFile.AllocReadLC(DEFAULT_ATTACHMENT_NAME);
	reader.SetBuffer(buf);
	iDefaultAttachmentName=reader.ReadTPtrC().AllocL();
	CleanupStack::PopAndDestroy(2, &resFile); // buf, resFile (Close resfile)

	if (!iStore8BitData)
		{
		// CRichText bits
		iParaLayer=CParaFormatLayer::NewL();
		iCharLayer=CCharFormatLayer::NewL();
		}

	// Create converter objects
	iCharacterConverter=CCnvCharacterSetConverter::NewL();
	iCharConv=CImConvertCharconv::NewL(*iCharacterConverter, iFs);
	iHeaderConverter=CImConvertHeader::NewL(*iCharConv); 

	// we assume that this message is MIME as we have no way of
	// detecting otherwise (without sending a new FETCH to the
	// server to get the MIME-Version).
	iHeaderConverter->SetMessageType(ETrue);

	// Get timer to defeat IMSK timeout
	iDummyRead = CImImap4SessionDummyRead::NewL(*this, *iImapIO, Priority());
	
	iIdleRead =  CImImap4SessionIdleRead::NewL(*this, Priority());
	
	// List of message UIDs in a folder
	iSearchList=new (ELeave) CArrayFixFlat<TUint32>(8);		// For granularity of 8, 32 bytes are allocated each time.
	iSyncLimit = KImImapSynchroniseAll;
	// set up progress types
	iProgress.iType=EImap4GenericProgressType;
	iCurrentDrive = MessageServer::CurrentDriveL(iFs);

	iIdleTimerExpired = EFalse;

	iIdleTimer = CIdleTimeoutTimer::NewL(*this);
	iReissueIdle = EFalse;
	iDisconnectAfterIdleStopped = EFalse;
	iFetchPartialMail=EFalse;
	iCaf = new (ELeave) CImCaf(iFs);

	iIsICalendar = EFalse;
	iIsVCalendar = EFalse;
	
	iUidString = HBufC8::NewL(KUidListStringSize);
	
	iCancelTimer = CImapCancelTimer::NewL(*this);
	}

CImImap4Session::~CImImap4Session()
	{
	Cancel(); // make sure we're cancelled

	delete iDummyRead;
	delete iIdleRead;

	// No settings
	delete iServiceSettings;
	delete iPrefs;

	// Get rid of connection
	delete iImapIO;

	// Get rid of message body bits
	delete iBodyBuf;
	delete iMessageBody;
	delete iBodyText;
	delete iParaLayer;
	delete iCharLayer;
	
	// Message selection
	delete iSelection;

	// List of messages to delete
	delete iDeletedUids;

	// List of messages to fetch
	delete iFetchList;

	// List of seen flags to set/clear
	delete iSetSeenList;
	delete iClearSeenList;

	// Partial line: used when doing Q-P decoding, as it works on a line at a time.
	delete iPartialLine;

	// Any attachment info (ie: there was a fetch in progress)
	delete iAttachmentFile;
	delete iAttachmentFullPath;
	delete iAttachmentMimeInfo;
	delete iDefaultAttachmentName;
	delete iFooterString;
	
	// Any message sizer left over (ie: we've been deleted in the middle of
	// an append operation)
	delete iMessageSizer;
	delete iMessageSender;
	delete iLineBuffer;

	// Characterset conversion
	delete iHeaderConverter;
	delete iCharConv;
	delete iCharacterConverter;

	// CMsvServerEntry used for moves
	delete iMoveEntry;

	//cached TMsvEntry data
	delete iCachedEntryData;

	// List of message UIDs in a folder
	delete iSearchList;

	// Selection passed during synchronisation
	delete iSynchronisationSelection;

	delete iIdleTimer;
	delete iCaf;
	delete iUidString;
	iFs.Close();
	
	delete iCancelTimer;
	
	//Delete Username and Password
	delete iUsername;
	delete iPassword;
	
	// Note: iList is owned by a caller, not us - we don't delete it
	}

// Logging calls: passed through to ImapIO
void CImImap4Session::LogText(const TDesC8& aString)
	{
	// Log the text
	iImapIO->LogText(aString);
	}

void CImImap4Session::LogText(TRefByValue<const TDesC8> aFmt,...)
	{
	VA_LIST list;
	VA_START(list,aFmt);
	TBuf8<1024> aBuf;
	//handles the data over flow panics. returns immediately without performing any action.
	TDes8OverflowHandler overFlowHandler;
	
	aBuf.AppendFormatList(aFmt,list, &overFlowHandler);
	LogText(aBuf);
	}

// Do setentry, leave if there is an error
void CImImap4Session::SetEntryL(const TMsvId aId)
	{
#ifdef PRINTING
	TInt error=iEntry->SetEntry(aId);
	if (error)
		LogText(_L8("SetEntryL(%x) returned %d"),aId,error);
	User::LeaveIfError(error);
#else
	User::LeaveIfError(iEntry->SetEntry(aId));
#endif
	}

// Change entry, leave if error
void CImImap4Session::ChangeEntryL(const TMsvEntry& aEntry)
	{
#ifdef PRINTING
	TInt error=iEntry->ChangeEntry(aEntry);
	if (error)
		LogText(_L8("ChangeEntryL(%x) returned %d"),aEntry.Id(),error);
	User::LeaveIfError(error);
#else
	User::LeaveIfError(iEntry->ChangeEntry(aEntry));
#endif
	}

// Change entry in bulk mode (i.e. no index file commit), leave if error
void CImImap4Session::ChangeEntryBulkL(const TMsvEntry& aEntry)
	{
#ifdef PRINTING
	TInt error=iEntry->ChangeEntryBulk(aEntry);
	if (error)
		LogText(_L8("ChangeEntryL(%x) returned %d"),aEntry.Id(),error);
	User::LeaveIfError(error);
#else
	User::LeaveIfError(iEntry->ChangeEntryBulk(aEntry));
#endif
	}
// Get children, leave if error
void CImImap4Session::GetChildrenL(CMsvEntrySelection& aSelection)
	{
#ifdef PRINTING
	TInt error=iEntry->GetChildren(aSelection);
	if (error)
		LogText(_L8("GetChildrenL() returned %d"),error);
	User::LeaveIfError(error);
#else
	User::LeaveIfError(iEntry->GetChildren(aSelection));
#endif
	}

// This can be called after Select() has completed to find out if the
// folder has changed in any way since the last sync
TBool CImImap4Session::FolderChanged() const
	{
	return iMailboxReceivedExists ||
		iMailboxReceivedExpunge ||
		iMailboxReceivedFlags;
	}

TBool CImImap4Session::ImapIdleSupported() const
	{
	return iUseIdleCommand && iCapabilityIdleSupport;
	}

TBool CImImap4Session::IsIdling() const
	{
	return(iState==EImapStateIdling);
	}

// Transfers the current selection into the iFolderIndex, and sorts it by
// UID.
void CImImap4Session::MakeSortedFolderIndexL(TBool aUseCachedEntryData)
	{
		
	TInt noofchildren=iSelection->Count();
	
	// Reset folder index
	iFolderIndex.SetSizeL(noofchildren);
	TInt a=0;

	if(!aUseCachedEntryData)
		{ //can't rely on iCachedEntryData
		TMsvEntry* entryPtr;
		TMsvId id;
		for(a=0;a<noofchildren;a++)
			{
			// Save UID/TMsvId of this entry
			id=(*iSelection)[a];
			User::LeaveIfError(iEntry->GetEntryFromId(id,entryPtr));
			iFolderIndex[a].iUid=((TMsvEmailEntry)(*entryPtr)).UID();
			iFolderIndex[a].iMsvId=id;
			}
		}
	else
		{
		for(a=0;a<noofchildren;a++)
			{
			// Save UID/TMsvId of this entry
			iFolderIndex[a].iUid=(*iCachedEntryData)[a].iUid;
			iFolderIndex[a].iMsvId=(*iSelection)[a];
			}
		}

	// Sort it by UID
	iFolderIndex.Sort();

	// Check for any duplicate UIDs (ie, a dud netscape server)
	TMsvEntry* entryPtr;
	TMsvEntry* nextEntryPtr;
	for(a=1;a<noofchildren;a++)
		{
		if(iFolderIndex[a].iUid!=0 && iFolderIndex[a].iUid==iFolderIndex[a-1].iUid)
			{
			if(!aUseCachedEntryData)
				{
				// get the TMsvEntry for the message/folder
				User::LeaveIfError(iEntry->GetEntryFromId(iFolderIndex[a].iMsvId,entryPtr));
				User::LeaveIfError(iEntry->GetEntryFromId(iFolderIndex[a-1].iMsvId,nextEntryPtr));
				// check if type of TMsvEntry and type of next TMsvEntry are both Messages
				if( entryPtr->iType.iUid == nextEntryPtr->iType.iUid && entryPtr->iType.iUid == KUidMsvMessageEntryValue)
					{
					User::Leave(KErrCorrupt);
					}
				}
			else
				{
				User::Leave(KErrCorrupt);
				}
			}
			
		}

#ifdef PRINTING
	LogText(_L8("MakeSortedFolderIndex done: index list follows. children=%d"),noofchildren);
	for(a=0;a<noofchildren;a++)
		LogText(_L8("  MsvId=%8x   UID=%d"),iFolderIndex[a].iMsvId,iFolderIndex[a].iUid);
#endif
	}

HBufC* CImImap4Session::DoUnModUTF7LC(TDesC8& aBuffer)
	{
	iCharConv->PrepareToConvertToFromOurCharsetL(KCharacterSetIdentifierImapUtf7);

	// unicode version won't be longer than the original (in chars)
	HBufC* text=HBufC::NewL(aBuffer.Length());
	CleanupStack::PushL(text);

	//LogText(_L8("DoUnModUTF7LC: from %S"),&aBuffer);

	TInt numUC, indexUC;
	TPtr des = text->Des();
	iCharConv->ConvertToOurCharsetL(aBuffer, des, numUC, indexUC);

	//LogText(_L8("DoUnModUTF7LC: to   %S len %d numUC %d"),&des, des.Length(), numUC);
	
	return text;
	}

// Enquote a string (being sent as a string literal) if required
void CImImap4Session::DoQuoteL(HBufC8*& aBuffer)
	{
	// Null string? Nothing to do
	if (!aBuffer->Length() || !aBuffer->Des().Length()) return;

	// Anything needing quoting in there?
	if (aBuffer->Des().Locate('\\')==KErrNotFound &&
		aBuffer->Des().Locate('\"')==KErrNotFound) return;

	// Run through string, inserting quote characters as needed
	for(TInt a=0;a<aBuffer->Des().Length();a++)
		{
		if (aBuffer->Des()[a]=='\\' || aBuffer->Des()[a]=='\"')
			{
			HBufC8 *newbuf=aBuffer->ReAllocL(aBuffer->Des().Length()+1);

			// Been moved due to realloc?
			if (newbuf!=aBuffer)
				{
				// In all cases when DoQuoteL() is called, the buffer is on the top of
				// the cleanup stack: change this to indicate the correct entry
				CleanupStack::Pop();
				CleanupStack::PushL(aBuffer=newbuf);
				}

			aBuffer->Des().Insert(a,_L8("\\"));
			a++;
			}
		}
	}

TInt CImImap4Session::FindFilename(const CImMimeHeader& aMimeInfo, TPtrC8& aFilename)
	{
	// Look in content-type list
	const CDesC8Array& ctype=aMimeInfo.ContentTypeParams();

	DBG((LogText(_L8("FindFilename: Checking %d entries in content-type list"),
				ctype.Count())));

	TInt tuple=0;
	while(tuple<ctype.Count())
		{
#ifdef PRINTING
		TPtrC8 t1=ctype[tuple],t2=ctype[tuple+1];
		LogText(_L8("  %S %S"),&t1,&t2);
#endif
		// Look for "name xxx"
		if (ctype[tuple].CompareF(KMIME_NAME)==0)
			{
			// Got it: report that we found it
			aFilename.Set(ctype[tuple+1]);
			TBuf8<KMaxFileName>buf(aFilename);
			buf.Trim();
			if(buf.Length()==0)
				{
				return(KErrNotFound);
				}

			return(KErrNone);
			}
		else if (ctype[tuple].CompareF(KMIME_NAME_RFC2231)==0)
			{
			// Got it: report that we found it
			aFilename.Set(ctype[tuple+1]);
			return(KErrRFC2231Encoded);
			}
		tuple+=2;
		}

	// Not found in the content type, try content disposition
	tuple=0;
	const CDesC8Array& cdisp=aMimeInfo.ContentDispositionParams();
	while(tuple<cdisp.Count())
		{
#ifdef PRINTING
		TPtrC8 t1=cdisp[tuple],t2=cdisp[tuple+1];
		LogText(_L8("  %S %S"),&t1,&t2);
#endif
		// Look for "filename xxx"
		if (cdisp[tuple].CompareF(KMIME_FILENAME)==0)
			{
			// Got it: report that we found it
			aFilename.Set(cdisp[tuple+1]);
			return(KErrNone);
			}
		else if (cdisp[tuple].CompareF(KMIME_FILENAME_RFC2231)==0)
			{
			// Got it: report that we found it
			aFilename.Set(cdisp[tuple+1]);
			return(KErrRFC2231Encoded);
			}

		tuple+=2;
		}

	// Didn't find it
	return(KErrNotFound);
	}
	
void CImImap4Session::FindFilenameDecodeL(const CImMimeHeader& aMimeInfo, TFileName& aFileName)
	{
	// Make an attachment name
	aFileName.Zero();

	TPtrC8 origFileName;

	// Look for filename in Content-Type list
	TInt err = FindFilename(aMimeInfo, origFileName);
	if (KErrNotFound == err)
		{
		// Fall back to simple "attachment" (language specific)
		aFileName=iDefaultAttachmentName->Des();
		}
	else if (KErrRFC2231Encoded == err)
		{
		// A file name has been found but it is encoded (RFC2231)
		// Use the default file name but append the file extension so that its type can be recognised
		aFileName=iDefaultAttachmentName->Des();
		TInt dotPos = origFileName.Length() - 1;
		TBool dotFound = EFalse;
		
		// Find the extension
		while ((dotPos != 0) && (!dotFound))
			{
			if (origFileName[dotPos] == '.')
				{
				dotFound = ETrue;
				// Extension found: append it to the filename
				TInt extensionLength = origFileName.Length() - dotPos;
				if ((aFileName.Length() + extensionLength) <= aFileName.MaxLength())
					{
					HBufC* extension = HBufC::NewLC(extensionLength);
					extension->Des().Copy(origFileName.Right(extensionLength));
					aFileName.Append(*extension);
					CleanupStack::PopAndDestroy(extension);
					}
				}

			--dotPos;
			}
		}
	else
		{
		// Run it through the QP decoder
		HBufC *decoded=HBufC::NewLC(origFileName.Length());
		TPtr decoded_ptr(decoded->Des());

		// Decode filename from the header
		iHeaderConverter->DecodeHeaderFieldL(origFileName, decoded_ptr);
		
		DBG((LogText(_L8("FindFilenameDecode: '%S' to '%S' "),&origFileName,&decoded_ptr)));

		// Need to do a check on the filename length here.
		// If it is too long, set to the max possible, keeping extension.
		TFileName path;
			
		TInt fileNameLength = path.Length() + decoded_ptr.Length();

		if( fileNameLength > KMaxFileName)
			{
#ifdef __WINS__
			TFileName winsFileName;
			TFileName mailStoreDrive;
			TDriveUnit drive(MessageServer::CurrentDriveL(iFs));
			mailStoreDrive.Append(drive.Name());
			mailStoreDrive.Append(KPathDelimiter);
			MapEmulatedFileName(winsFileName, mailStoreDrive);
			TInt prefixLen = winsFileName.Length();
#else
			TInt prefixLen = 0;
#endif
			// Crop the Old File Name
			TInt lengthToCrop = (fileNameLength - KMaxFileName) + prefixLen;
			// Use LocateReverse rather than TParsePtr as decoded_ptr may be > 256 chars
			TInt dot = decoded_ptr.LocateReverse( '.' );
			TPtrC extension = decoded_ptr.Mid(dot != KErrNotFound ? dot : decoded_ptr.Length());
			TInt newFileNameLength = decoded_ptr.Length() - extension.Length() - lengthToCrop;
			TPtrC newFileName=decoded_ptr.Left(newFileNameLength);

			// Create the New File Name (ie File Name & Extension)
			aFileName.Zero();
			aFileName.Append(newFileName);
			aFileName.Append(extension);
			}
		else
			{
			aFileName.Copy(decoded_ptr);
			}
		CleanupStack::PopAndDestroy(); // decoded
		}
	}

// Update iDate in iMailboxId to show the time now (last sync time)
void CImImap4Session::SyncCompleteL()
	{
        DBG((LogText(_L8("CImImap4Session::SyncCompleteL()"))));
	// Find entry
	SetEntryL(iMailboxId);
	TMsvEmailEntry message=iEntry->Entry();

	// Find 'now'
	TTime now;
	now.UniversalTime();
	message.iDate=now;

	// Check to see if there has been a change in the number of messages in the remote folder.
	TBool folderSizeChanged=(message.RemoteFolderEntries()!=iMailboxSize);

	// Set 'unread' flag on folder if there are any unread messages within it
	if (FIXBOOL(message.Unread())!=iSomeUnread || !message.Visible() || folderSizeChanged)
		{
		// Update flags
		message.SetUnread(iSomeUnread);
		message.SetVisible(ETrue);
		message.SetRemoteFolderEntries(iMailboxSize);
 		ChangeEntryBulkL(message);
		}

	// we need to ensure the hierarchy of folders containing this one
	// is now visible. Note previously this incorrectly only did this
	// when we were not in DisconncetedUserMode
	do
		{
		// Move up one
		SetEntryL(message.Parent());
		message=iEntry->Entry();

		// Ensure visibility
		if (!message.Visible())
			{
			message.SetVisible(ETrue);
			ChangeEntryL(message);
			}
		}
	while(message.iType!=KUidMsvServiceEntry);
	
	// Before we got back to the idle state, we need to commit any
	// outstanding entries to the index file to complete the bulk
	// synchronization operation
	iEntry->CompleteBulk();

	if (iReissueIdle)
		{
		iState=EImapStateSelected;
		DoStartIdleL();
		}
	}

// Reset subscription flags for all children, and recurse into folders
void CImImap4Session::ResetSubscriptionFlagsL(const TMsvId aFolder)
	{
	// Do this one
	SetEntryL(aFolder);
	TMsvEmailEntry entry=iEntry->Entry();

	// A folder or service? If not, return
	if (entry.iType!=KUidMsvServiceEntry &&
		entry.iType!=KUidMsvFolderEntry)
		return;

	// Reset flag if needed
	if (entry.Subscribed())
		{
		// Reset flag and save
		entry.SetSubscribed(EFalse);
		ChangeEntryL(entry);
		}

	// Any children?
	CMsvEntrySelection *children=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(children);
	GetChildrenL(*children);
	if (children->Count())
		{
		// Do each in turn
		for(TInt child=0;child<children->Count();child++)
			ResetSubscriptionFlagsL((*children)[child]);
		}
	CleanupStack::PopAndDestroy();
	}
	
TBool CImImap4Session::IsCancelling() const
	{
	return iCancelAndIdle;
	}
	
void CImImap4Session::CancelAndIdleL(TBool aReissueIdle)
	{
	// Flag that a cancel and idle command has been requested.
	iCancelAndIdle = ETrue;

	switch( iState )
		{
	case EImapStateFetchWait:
		{
		// Stop requesting fetches and wait for the current fetches to be completed.
		iState = EImapStateFetchCancelWait;
		iReissueIdle = aReissueIdle;
		
		// Start an idle timer - this is ensure that if the GPRS session is currently
		// suspended we don't hang forever waiting for the remaining fetch data to be
		// received.
		iCancelTimer->After(KImapFetchCancelIdleTime);
		
		// Delete any partially downloaded attachments
		if ( iAttachmentFileState == EFileIsOpen )
			{
			iAttachmentFileState=EFileIsIncomplete;
			if(iCaf->Processing())
				{
				iCaf->EndProcessingL();
				}
			else
				{
				iAttachmentFile->CloseFile();
				}
			CMsvStore* store = iEntry->EditStoreL(); 
			CleanupStack::PushL(store);
			// Could be multiple attachments in the folder.
			TInt i;
			TInt attachmentCount = store->AttachmentManagerL().AttachmentCount();
			for(i=0;i<attachmentCount;i++)
				{
				// Remove [0] as array is shuffled. Once index [n] is removed n+1 becomes n
				store->AttachmentManagerExtensionsL().RemoveAttachmentL(0);
				}
			if(attachmentCount)			
				store->CommitL();
			CleanupStack::PopAndDestroy(store);
			TMsvEmailEntry message=iEntry->Entry();

			CreateAttachmentInfoL((TMsvEmailEntry&)iEntry->Entry());
			ChangeEntryBulkL(message);
			}
	
		DBG((LogText(_L8("CImImap4Session::CancelAndIdleL() - fetch cancel; continue to process requested fetches - no more parts to be requested"))));
		} break;
		
	case EImapStateIdleWait:
		{
		// Not much to do - already issued IDLE command, just wait for the IDLE 
		// command to 'start'.
		// NOTE - set the iReissueIdle flag to ensure that IssueIdleRead is 
		// called when we get the server response.
		iReissueIdle = aReissueIdle;
		
		// Start an idle timer - this is ensure that if the GPRS session is currently
		// suspended we don't hang forever waiting for the remaining data to be
		// received.
		iCancelTimer->After(KImapFetchCancelIdleTime);		
		
		DBG((LogText(_L8("CImImap4Session::CancelAndIdleL() - waiting for IDLE - continue"))));
		} break;
		
	case EImapStateIdling:
		{
		// Nothing much to do here - already IDLE-ing! Unset the cancel-and-idle flag
		iCancelAndIdle = EFalse;
		DBG((LogText(_L8("CImImap4Session::CancelAndIdleL() - already IDLE-ing - do nothing"))));
		} break;
	
	case EImapStateStopIdleWait:
		{
		// Need to wait for the current IDLE command to complete, then re-issue
		// another one!!
		iReissueIdle = aReissueIdle;

		// Start an idle timer - this is ensure that if the GPRS session is currently
		// suspended we don't hang forever waiting for the remaining data to be
		// received.
		iCancelTimer->After(KImapFetchCancelIdleTime);
		
		DBG((LogText(_L8("CImImap4Session::CancelAndIdleL() - IDLE stop requested - wait for response and then re-issue IDLE command"))));
		} break;
		
	case EImapStateSelectWait:
		{
		// Waiting for select to complete - issue IDLE once select completes.
		// A check will be made to ensure that the selected mailbox is the inbox.

		iReissueIdle = aReissueIdle;

		// Start an idle timer - this is ensure that if the GPRS session is currently
		// suspended we don't hang forever waiting for the remaining data to be
		// received.
		iCancelTimer->After(KImapFetchCancelIdleTime);
		
		DBG((LogText(_L8("CImImap4Session::CancelAndIdleL() - cancel select - wait for response and then issue IDLE command"))));		
		} break;
		
	case EImapStateSelected:
		{
		// A mailbox has been selected - does IDLE need to be re-issued?
		iReissueIdle = aReissueIdle;
		
		if( iReissueIdle )
			{
			// Yep, need to re-issue the IDLE (as was IDLE-ing before).
			// For this need to be in the INBOX as a writable-select.
			
			// First reset counts to safe values here to avoid reporting left
			// over values from previous fetch. Correct values will be set up
			// once headers have been fetched and parts counted (taken from SelectL).
			iProgress.iPartsToDo=iProgress.iBytesToDo=1;
			iProgress.iPartsDone=iProgress.iBytesDone=0;

			// Do the select (if we really need to) or skip if possible.
			if( iMailboxId==GetInbox() && iMailboxWritable )
				{
				DBG((LogText(_L8("Need to re-issue IDLE command"))));

				// No need to do the select - so re-issue the IDLE
				DoStartIdleL();				
				}
			else
				{
				DBG((LogText(_L8("Need to issue IDLE - first select Inbox (writable)"))));
			
				// Looks like we need to do the select...
				DoSelectL(GetInbox(), ETrue);
				}
			}		
		else
			{
			DBG((LogText(_L8("Do not re-issue IDLE - cancel completed"))));
			
			// Cancelling completed - stay in this mailbox and do not issue idle.
			iCancelAndIdle = EFalse;
			}			
		} break;
		
	case EImapStateMoveEntryWait:
		{
		// We're cancelling a move entry: we need to stop it specifically
		iMoveEntry->Cancel();
		iState=iSavedState;	
		
		// Recurse (yuk!) into method with the new 'saved' state.	
		CancelAndIdleL(aReissueIdle);
		return;
		}
		// No break statement required due to return statement

	default:
		// For all other states - just do a 'normal' cancel - this will probably
		// disconnect the session.
		iCancelAndIdle = EFalse;
		Cancel();
		return;
		}

	// Need to complete the parent/observer.
	CMsgActive::DoCancel();
	}
	
// Called when parent wants to cancel current operation
void CImImap4Session::DoCancel()
	{
	DBG((LogText(_L8("CImImap4Session::DoCancel() called whilst in state %d"),iState)));

	if(IsIdling())
		{
		iIdleRead->Cancel();
		}
	else 
		{
		if(iAttachmentFile && iAttachmentFileState==EFileIsOpen)
			{
			DBG((LogText(_L8("CImImap4Session::DoCancel() closing attachment file"))));
			if(iCaf->Processing())
				{
				TRAP_IGNORE(iCaf->EndProcessingL());
				}
			else
				{
				iAttachmentFile->CloseFile();
				}
			iAttachmentFileState=EFileNotOpen;
			}
	
		// What were we about to do?
		switch(iState)
			{
		case EImapStateConnectWait:
		case EImapStateGreetingWait:
		case EImapStateLoginSendUser:
		case EImapStateLoginSendPassword:
		case EImapStateLoginWait:
			DoDisconnect();
			break;
			
		case EImapStateSelectWait:
			// Selecting: fail back to noselect
			iState=EImapStateNoSelect;
			break;

		case EImapStateMoveEntryWait:
			// We're cancelling a move entry: we need to stop it specifically
			iMoveEntry->Cancel();
			iState=iSavedState;
			break;


		default:
			// Something else: disconnect for safety
			DoDisconnect();
			break;
			}

		// Note tag which we've cancelled: anything outstanding, basically,
		// which means anything up to (and including) the last command issued,
		// which is iTag
		iCancelledTag=iTag;

		iImapIO->Cancel();
		}
 
	iIdleTimer->Cancel();

	DBG((LogText(_L8("CImImap4Session::DoCancel() finished 1"))));

	// ...ask parent to finish up
	CMsgActive::DoCancel();

	DBG((LogText(_L8("CImImap4Session::DoCancel() finished 2"))));
	}

// Disconnect and complete with an error code
void CImImap4Session::Fail(const TInt aError)
	{
	DBG((LogText(_L8("CImImap4Session::Fail(%d)"),aError)));
	DoDisconnect();
        DBG((LogText(_L8("-----------------------------------------------------------"))));
	DBG((LogText(_L8("CImap4Session::Fail(): calling Complete()"))));
	DBG((LogText(_L8("-----------------------------------------------------------"))));
	Complete(aError);
	}
	
void CImImap4Session::DoDisconnect()
	{
	DBG(LogText (_L8("CImImap4Session::DoDisconnect()")));
	iImapIO->Disconnect();
	iState = EImapStateDisconnected;
	iSecurityState = EUnknown;
	iCommandsOutstanding = 0;
	iSendQueued = EFalse;
	iReceiveQueued = EFalse;
	iReissueIdle = EFalse;
	iCancelAndIdle = EFalse;
	iIdleTimer->Cancel(); 
	iIdleTimerExpired = EFalse;
	}

void CImImap4Session::DummyComplete(TInt aError)
	{
	DBG(LogText(_L8("+ CImImap4Session::DummyComplete(err=%d)"), aError));
	if(aError >= KErrNone)
		{ // Got a response, let's have a look at it
		CImapAtom* p=iImapIO->RootAtom()->Child();
		if (!p)
			{
			aError = KErrNotFound;
			return;
			}

		if (p->Compare(KIMAP_UNTAGGED))
			{
			// Process it, it could be something useful
			TRAP( aError, ProcessUntaggedL(p->ToNextL(),EFalse) );
			DBG( LogText(_L8("Dummy read untagged msg processed with %d"), aError) );
			}
		else
			{
			// Got a valid response that wasn't untagged... uhoh!
			DBG( LogText(_L8("Dummy read received a non-untagged response!")) );
			aError = KErrCorrupt;
			}
		}

	if(aError < KErrNone)
		{
		// If the dummy read returned an error then the line was probably dropped,
		// or, if the read returned something that was rubbish then the server is 
		// not playing by the rules.
		// Either way, let's disconnect.
		LostConnection(aError);

		// Can't report the error, no one to report to
 		}
	DBG( LogText(_L8("- CImImap4Session::DummyComplete()")) );
	}

void CImImap4Session::LostConnection(TInt /*aError*/)
	{
	// the line must have been dropped so call DoDisconnect to ensure state is up
	// to date
	DoDisconnect();

	// mark service as offline immediately
	// the returned error code ignored
	if (iEntry->SetEntry(iServiceId))
		{
		TMsvEntry entry=iEntry->Entry();
		entry.SetConnected(EFalse);
		iEntry->ChangeEntry(entry);
		}
	}


void CImImap4Session::IdleReadError(TInt aError)
	{	
	 // Read completed with an error, probably lost the connection
	DBG(LogText(_L8("IMAP Idle outstanding read completed with %d"), aError));

	LostConnection(aError);

	// Stop the idle timer so that it does not try to restart the idle when it expires
	iIdleTimer->Cancel();
	}
 

void CImImap4Session::IssueIdleRead()
	{
	__ASSERT_DEBUG(IsIdling() && !IsActive(), gPanic(EBadUseOfImap4Op));
	DBG((LogText(_L8("Idle read issued"))));
	iIdleRead->Start(iStatus);
	DBG((LogText(_L8("******************************************************************"))));
	DBG((LogText(_L8("CImImap4Session::IssueIdleRead(): waiting for iIdleRead to wake me"))));
	DBG((LogText(_L8("******************************************************************"))));
 	SetActive();
 	}
	

void CImImap4Session::IssueDummy()
	{
#ifdef ASYNC_NOTIFICATIONS
	// Issue a dummy read from the CImapIO class so we can check the connection
	// status. ONLY IF WE'RE CONNECTED!
	if (iState>=EImapStateNoSelect)
		{
		iDummyRead->Start();
		}
#endif
	}

void CImImap4Session::CancelDummy()
	{
#ifdef ASYNC_NOTIFICATIONS
	// Cancel it!
	iDummyRead->Cancel();
#endif
	}

void CImImap4Session::ReissueIdleL()
	{
	DBG((LogText(_L8("CImImap4Session::Re-issueIdle(): State: %d"), iState)));
 	Cancel();
 
        DBG((LogText(_L8("CImImap4Session::ReIssueIdle(): setting iReissueIdle to true"))));
 	iReissueIdle=ETrue;
	DoStopIdleL();
	}

void CImImap4Session::ReissueDummy()
	{
	IssueDummy();
	}


void CImImap4Session::DoComplete(TInt& aStatus)
	{
	DBG((LogText(_L8("CImImap4Session::DoComplete(iState=%d, aStatus=%d)"),iState,aStatus)));

	if (iState == EImapStateSelected)
		{
		iCompoundStopIdle = EFalse;
		iStoppingIdleForSync = EFalse;
		}

	if( iState != EImapStateFetchCancelWait &&
		iAttachmentFile && iAttachmentFileState==EFileIsOpen )
		{
		// Do not close the attachment file if we're cancelling the fetch - we 
		// will still be receiving data and if this completes the attachment
		// then we want the attachment file available for that.
		
		DBG((LogText(_L8("CImImap4Session::DoComplete closing attachment file"))));
		if(iCaf->Processing())
			{
			TRAP_IGNORE(iCaf->EndProcessingL());						
			}
		else
			{
			iAttachmentFile->CloseFile();
			}
		iAttachmentFileState=EFileNotOpen;
		}

	// All ok?
	if (aStatus==KErrNone)
		{
		// Everything is fine. However, we need to queue a dummy read from the
		// CImapIO layer to ensure that we get notified if the connection dies
		// unexpectedly		

		// Update the progress error code first.
		iProgress.iErrorCode=aStatus;

		if (ImapIdleSupported()==EFalse)
			{
			IssueDummy();
			}

		if (IsIdling())
			{
			IssueIdleRead();
			}

		return;
		}
		
	if( iCancelAndIdle )
		{
		// Record the error code and exit the method - ensure that we don't
		// disconnect.
		iProgress.iErrorCode=aStatus;
		return;
		}

	// Some error has ocurred. Deal with it.
	switch(iState)
		{
	case EImapStateCreateWait:
	case EImapStateRenameWait:
	case EImapStateDeleteWait:
	case EImapStateSubscribeWait:
		// A 'KErrIMAPNO' error isn't fatal to the connection
		if (aStatus==KErrCancel)
			{
			// Back to previous state: these commands won't have
			// disturbed it.
			iState=iSavedState;
			return;
			}
		else if (aStatus==KErrIMAPNO)
			{
			// Report error
			if (iState == EImapStateDeleteWait)
				iProgress.iErrorCode=KErrImapCantDeleteFolder;
			else
				iProgress.iErrorCode=KErrNotSupported;
			aStatus=iProgress.iErrorCode;

			// Back to previous state
			iState=iSavedState;
			return;
			}

		// Otherwise, process as per normal
		break;

	case EImapStateSelectWait:
	case EImapStateSynchroniseWait:
		// KErrIMAPNO isn't fatal, we just go back to the selected state
		if (aStatus==KErrIMAPNO)
			{
			iState=EImapStateSelected;
			return;
			}
		break;

	case EImapStateMoveEntryWait:
		// We're done with the moveentry

		// Park the move entry again
		iMoveEntry->SetEntry(NULL);
		break;
		
	case EImapStateIdleWait:
	case EImapStateStopIdleWait:
		if (iIdleTimerExpired)
			{
			// error has occurred following re-issue of an IDLE command
			// Notify the server MTM that the error has occurred as there
			// is no outstanding asynchonous request on this session.
			// (IDLE is issued autonomously by the IMAP Session).
			iObserver.NonCompletedFailure();
			}
		break;	

	case EImapStateFetchCancelWait:
		// record the error (i.e. cancel) for progress and do not disconnect.
		iProgress.iErrorCode=aStatus;
		// drop through to next case... (as that is returning and so not disconnecting).
	case EImapStateNoSelect:
	case EImapStateSelected:
	case EImapStateIdling:
		return;

	default:
		break;
		}

	// If we get here with an error, then we need to disconnect.
	// Earlier on, if there was a time when disconnection wasn't
	// required, we would have returned.
	DoDisconnect();
	
	// Save error code in progress
	iProgress.iErrorCode=aStatus;
	}

// Copy a message: in fact, we move the entire message to the destination, but
// then recreate the empty shell (no parts fetched) of the source
void CImImap4Session::CopyMessage(TRequestStatus& aRequestStatus, const TMsvId aSourceFolder, const TMsvId aSource, const TMsvId aDestinationFolder, TMsvId* aNewSource, const TBool aRemoveOriginal)
	{
	TInt err=KErrNone;
	if (!Connected())
		{
		Queue(aRequestStatus);
		err=KErrDisconnected;
		}
	else
		TRAP(err,CopyMessageL(aRequestStatus, aSourceFolder, aSource, aDestinationFolder, aNewSource, aRemoveOriginal));
	if (err!=KErrNone)
		{
		// park moveentry if it fails to get going
		if (iMoveEntry)
			iMoveEntry->SetEntry(NULL);
                DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::CopyMessage(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));

		Complete(err);
		}
	}

void CImImap4Session::CopyMessageL(TRequestStatus& aRequestStatus, const TMsvId aSourceFolder, const TMsvId aSource, const TMsvId aDestinationFolder, TMsvId* aNewSource, const TBool aRemoveOriginal)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND CopyMessage(%x (in %x) to %x)"),aSource,aSourceFolder,aDestinationFolder)));

	Queue(aRequestStatus);

	// Get a moveentry if we don't already have one
	if (!iMoveEntry)
		{
		// Get a MoveEntry: we need to ask for one as a child of this entry, so
		// move it to the root and ask for a child in the local service, which should
		// always be there.
		SetEntryL(KMsvRootIndexEntryId);

		// Get child
		iMoveEntry=iEntry->NewEntryL(KMsvLocalServiceIndexEntryId);
		}

	// Do the move, using the iMoveEntry CMsvServerEntry object, after parking our original
	// one
	iEntry->SetEntry(NULL);
	User::LeaveIfError(iMoveEntry->SetEntry(aSourceFolder));	

	DBG((LogText(_L8("About to call iEntry->MoveEntry(%x,%x) when in folder %x"),aSource,aDestinationFolder,aSourceFolder)));

	// We're moving - note bits for bottom half handling
	iMoveSource=aSource;
	iMoveSourceFolder=aSourceFolder;
	iNewSource=aNewSource;

	// Cancel any dummy operation that might be outstanding
	if (ImapIdleSupported()==EFalse)
		{
		CancelDummy();
		}

	// Trap around this so we can park moveentry if it fails
	//DS - now selectively either Copy or Move.
	aRemoveOriginal?
		iMoveEntry->MoveEntryL(aSource,aDestinationFolder,iStatus):
		iMoveEntry->CopyEntryL(aSource,aDestinationFolder,iStatus);

	// Move into the new state and go active
	iSavedState=iState;
	iState=EImapStateMoveEntryWait;
	if (!IsActive()) SetActive();
	}

#if 0
// Debug only: print out a representation of the parse tree
void CImImap4Session::showtree(CImapAtom *root,int indent)
	{
	TInt b=0;

	// Run through children
	do
		{
		TPtrC8 atom=root->Atom();
		LogText(_L8("%03d Sibling %d: '%S'"),indent,b++,&atom);

		if (root->Child())
			showtree(root->Child(),indent+2);

		root=root->Next();
		}
	while(root);
	}
#endif

// Parse greeting message
TInt CImImap4Session::ProcessGreetingL()
	{
	CImapAtom* p=iRootAtom->ToChildL();

	// Should be a '*'
	if (!p->Compare(KIMAP_UNTAGGED))
		User::Leave(KErrGeneral);
	
	// Greeting line can be:
	// * BYE ... (server busy)
	// * PREAUTH ... (no login needed)
	// * OK ... (normal)
	p=p->ToNextL();
	if (p->Compare(KIMAP_BYE))
		{
		// Server is busy
		return(KErrImapServerBusy);
		}
	else if (p->Compare(KIMAP_PREAUTH))
		{
		// Already authorised, straight into Noselect
		iSavedState=EImapStateNoSelect;
		}
	else if (p->Compare(KIMAP_OK))
		{
		// Need to login
		iSavedState=EImapStateLoginWait;

		// Is this a CC:Mail server? (paranoid mode)
		if (p->Next())
			{
			if (p->Next()->Compare(_L8("CC:Mail")))
				{
				// We are, note it.
				iTalkingToCCMail=ETrue;

				DBG((LogText(_L8("We're talking to a CC:Mail server, modified fetch strategy enabled."))));
				}
			else if (p->Next()->Compare(_L8("OpenMail")) )
				{
				iTalkingToOpenMail=ETrue;
				DBG((LogText(_L8("We're talking to an OpenMail server, modified fetch strategy enabled."))));
				}
			}
		}

	// Looks ok
	return(KErrNone);
	}

// Parse select reply messages
TInt CImImap4Session::ProcessCommandReplyL()
	{
	CImapAtom *p=iRootAtom->ToChildL();

	// Which command does this reply belong to?
	TInt thisis=iTag-(iCommandsOutstanding-1);

	// A cancelled command?
	TBool cancelled(EFalse);
	if (thisis<=iCancelledTag)
		{
		cancelled=ETrue;
		}

	// '+' indicates ideling
	if (p->Compare(KIMAP_CONTINUATION))
		{
		return(ProcessContinuationResponse(p->ToNextL()));
		}

	// '*' indicates an untagged message
	if (p->Compare(KIMAP_UNTAGGED))
		{
		// Process it
		return(ProcessUntaggedL(p->ToNextL(),cancelled));
		}
	
	// If we got here, it's a tagged reply.
	// Check it's the one we're expecting
	TInt tag(0);
	TInt error(p->Value(tag));
	if (error!=KErrNone)
		{
		// Problem parsing
		return error;
		}
	
	// Some command sequencing debugging
	DBG((LogText(_L8("Expecting tag %d, got tag %d"),thisis,tag)));

	// One less outstanding command
	iCommandsOutstanding--;

	// If the tagged reply is for a command that had been cancelled,
	// and there are still commands outstanding, then don't complete:
	// instead, just keep reading replies.
	if (tag < iCancelledTag + 1)
		{
		return KErrNotReady;
		}

	// Move on to result
	p=p->ToNextL();

#ifdef PRINTING
	// Print success of failure
	TPtrC8 n=p->Atom();
	LogText(_L8("Result for command tag %d is '%S'"),iTag,&n);
#endif
	// If it's OK, pass it to untagged processor
	if (p->Compare(KIMAP_OK))
		{		
		// It might have stuff like 'READ-WRITE' in it... but only if
		// the next atom has a child (ie open bracket)
		if (p->Next() && p->Next()->Child())
			{
			DBG((LogText(_L8("CImap4Session::ProcessCommandReply(): OK recieved with children"))));
 			// Ignore the return code: we've got our tagged reply!
			ProcessUntaggedL(p,EFalse);
			}
		else
			{
			// received ok response
			DBG((LogText(_L8("CImap4Session::ProcessCommandReply(): OK received - no children"))));
			}
		return(KErrNone);
		}
	else if (p->Compare(KIMAP_NO))
		{
		// The server didn't like this
		return(KErrIMAPNO);
		}

	// It's not OK: there's been an error
	return(KErrGeneral);
	}

// Parse Continuation Response
TInt CImImap4Session::ProcessContinuationResponse(CImapAtom* /*aAtom*/)
	{
	if (iState==EImapStateIdleWait)
		{
		iState = EImapStateIdling;
		if(!iIdleTimer->IsActive())
			{
			iIdleTimer->After(iIdleTimeout);
			}
		return KErrNone;
		}
	else
		{
		return KErrArgument;
		}
	}

// Parse untagged messages
TInt CImImap4Session::ProcessUntaggedL(CImapAtom *aAtom, const TBool aCancelled)
	{
	DBG((LogText(_L8("CImap4Session::ProcessUntaggedL(): running..."))));

	CImapAtom *p=aAtom;

	// Look at first atom
	if (static_cast<TChar>(p->Atom()[0]).IsDigit())
		{
		// First atom is a number
		TUint  msgnr(0);

		// Got it ok?
		if (p->Value(msgnr)!=KErrNone)
			User::Leave(KErrArgument);

		// Next atom will be one of:
		// EXISTS, RECENT, FETCH, etc
		p=p->ToNextL();
		if (p->Compare(KIMAP_EXISTS))
			{
			// Mailbox size changed?
			DBG((LogText(_L8("Mailbox size now %d was %d"),msgnr,iMailboxSize)));

			// Note it
			if (iMailboxSize != static_cast<TInt>(msgnr) )
				{
				// Set it to EXISTS
				iMailboxSize=msgnr;

				// if the EXISTS didn't report a change in size then
				// pretend it wasn't received
				iMailboxReceivedExists=ETrue;
				}

			// Resize index
			iFolderIndex.SetSizeL(iMailboxSize);
			}
		else if (p->Compare(KIMAP_EXPUNGE))
			{
			// Note it
			iMailboxReceivedExpunge=ETrue;
			}
		else if (p->Compare(KIMAP_RECENT))
			{
			// Note it
			iMailboxRecent=msgnr;
			}
		else if (!aCancelled && p->Compare(KIMAP_FETCH))
			{
			// Process fetch: any fetch data following?
			if (p->Next() && p->Next()->Child())
				{
				// Got an open bracket situation, looks good
				return(ProcessFetchL(msgnr,p->Next()->Child()));
				}
			else
				User::Leave(KErrGeneral);
			}
		else
			{
#ifdef PRINTING
			// Unknown
			TPtrC8 a=p->Atom();
			LogText(_L8("Unknown reply '* %d %S'"),msgnr,&a);
#endif
			}
		}
	else
		{
		// First atom not a number. Is it OK?
		if (p->Compare(KIMAP_OK))
			{
			// OK *can* be followed by bracketed attrib or attrib/value pair
			// however, this is not always the case: for example, EXAMINE'ing
			// a new folder with Netscape IMAP4rev1 Service 3.56 gives this
			// response:
			// 24/01/99 13:44:58 >> 53 EXAMINE "Thingy/trevor"
			// 24/01/99 13:44:59 << * OK Reset UID sequence counter.
			// This is totally legal in the spec, but not awfully useful to us as
			// plain text messages are server-specific and are really for carbon-
			// based lifeforms to read.
			if ((p=p->Next())==NULL)
				{
				// No message. Just '* OK'. What a pointless waste of bandwidth.
				return(KErrNotReady);
				}

			// Is this the start of a bracketed construct?
			if (p->Compare(_L8("(")) ||
				p->Compare(_L8("[")))
				{
				CImapAtom* child=p->ToChildL();

				if (child->Compare(KIMAP_UIDVALIDITY))
					{
					// Save it
					child=child->ToNextL();
					if (child->Value(iUidValidity)!=KErrNone)
						User::Leave(KErrArgument);
					}
				else if (child->Compare(KIMAP_UIDNEXT))
					{
					// Save it
					child=child->ToNextL();
					if (child->Value(iUidNext)!=KErrNone)
						User::Leave(KErrArgument);
					}
				else if (child->Compare(KIMAP_READWRITE))
					{
					// Note read-write open
					iMailboxWritable=ETrue;
					}
				else if (child->Compare(KIMAP_READONLY))
					{
					// Note read-only open
					iMailboxWritable=EFalse;
					}
				else if (child->Compare(KIMAP_ALERT))
					{
 					// alerts to be handled here, but return with no error until then
					LogText(_L8("Alert received- SessionState:iState %d"),iState);	
					if(iState==EImapStateIdling)
						{
						return KErrNone;
						}
					}
				else
					{
#ifdef PRINTING
					TPtrC8 unk=p->Atom();
					LogText(_L8("* OK [%S ???]"),&unk);
#endif
					}
				}
			else
				{
#ifdef PRINTING
				TPtrC8 unk=p->Atom();				
				LogText(_L8("* OK %S ???"),&unk);
#endif

				if(iState==EImapStateIdling)
					{
					return(KErrNotReady);
					}	
				}
			}
		else if (!aCancelled && p->Compare(KIMAP_LIST))
			{
			// Ignore it unless we've got somewhere to save it
			ProcessListL(p->ToNextL());
			}
		else if (!aCancelled && p->Compare(KIMAP_LSUB))
			{
			// Process subscription list reply
			ProcessLsubL(p->ToNextL());
			}
		else if (!aCancelled && p->Compare(KIMAP_SEARCH))
			{
			// Process UID search reply
			// Need to check that we actually received a list of UIDs in the response
			// because under certain circumstances we can get a reply that lists no UIDs.
			// This can happen if the search command specifies a set of UIDs but none
			// of them can be found on the server because they have all been expunged.
			// It can also happen if the search command includes a search string but
			// no messages on the server match it.
			if (p->Next())
				{
				ProcessSearchL(p->ToNextL());
				}
			}
		else if (p->Compare(KIMAP_BYE))
			{
			// Are we already logging out?
			if (iState!=EImapStateLogoutWait)
				{
				// Unexpected disconnection
				// WRITE!
				}
			}
		else if (p->Compare(KIMAP_NO))
			{
#ifdef PRINTING
			// NO message from server. Display it.
			LogText(_L8("Got NO:"));
			while((p=p->Next())!=NULL)
				{
				TPtrC8 word(p->Atom());
				LogText(_L8(" %S"),&word);
				}
#endif
			if(iState==EImapStateIdling)
					{
					// ignore this and remain in IDLE.
					return KErrNotReady;
					}
			}
		else if (!aCancelled && p->Compare(KIMAP_FLAGS))
			{
			// FLAGS response during folder open
			iMailboxReceivedFlags=ETrue;
#ifdef PRINTING
			LogText(_L8("Got FLAGS:"));
			p=p->ToNextL();
			p=p->ToChildL();
			do
				{
				TPtrC8 word(p->Atom());
				LogText(_L8(" %S"),&word);
				p=p->Next();
				}
			while(p!=NULL);
#endif
			}
		else if (p->Compare(KIMAP_CAPABILITY))
			{
			// clear here just for good measure
			iSeenVersion=EFalse;
			iCapabilityIdleSupport = EFalse;
			iCapabilityStartTLS=EFalse;
			iCapabilityLoginDisabled=EFalse;
			
			// CAPABILITY reply
			while((p=p->Next())!=NULL)
				{
				if (p->Compare(KIMAP_VERSION))
					iSeenVersion=ETrue;
				else if (p->Compare(KIMAP_IDLE))
					iCapabilityIdleSupport = ETrue;
				else if (p->Compare(KIMAP_STARTTLS))
					iCapabilityStartTLS=ETrue;
				else if (p->Compare(KIMAP_LOGINDISABLED))
					iCapabilityLoginDisabled=ETrue;
				}
			}
		else
			{
#ifdef PRINTING
			// Unknown
			TPtrC8 a=p->Atom();
			LogText(_L8("Unknown reply '* %S'"),&a);
#endif
			}
		}

	return(KErrNotReady);
	}

// Fill in a CImHeader from an envelope atom
void CImImap4Session::ProcessEnvelopeL(CImHeader* aHeader, TMsvEntry& aEntry, CImapAtom *aAtom)
	{
	CImapAtom *q=aAtom->ToChildL();
	TPtrC8 tptr;
	// ensure that nothing is placed on the cleanup stack between here
	// and calls to ProcessAddress/ProcessAddressList
	HBufC8 *address=HBufC8::NewLC(KImapAddressSizeInc);

	DBG((LogText(_L8("Processing envelope data"))));

	// Parse date information
	tptr.Set(q->Atom());
	TImRfc822DateField date;
	date.ParseDateField(tptr,aEntry.iDate);
	q=q->ToNextL();

	// Subject in CImHeader (TMsvEntry is later on after post-processing)
	if (!q->Compare(KIMAP_NIL))
		aHeader->SetSubjectL(q->Atom());
	q=q->ToNextL();

	// From information: both in CImHeader and TMsvEntry
	if (q->Child())
		{
		DBG((LogText(_L8("Processing 'From' information"))));

		ProcessAddressL(&address,q->ToChildL());
		aHeader->SetFromL(address->Des());
		}
	else
		{
		// No From information. Set blank
		aHeader->SetFromL(_L(""));
		}
	q=q->ToNextL();

	// Discard sender information
	q=q->ToNextL();

	// ReplyTo information
	if (q->Child())
		{
		DBG((LogText(_L8("Processing 'ReplyTo' information"))));

		// Replyto exists
		ProcessAddressL(&address,q->ToChildL());
		aHeader->SetReplyToL(address->Des());
		}
	else
		{
		// No replyto. Use From info
		aHeader->SetReplyToL(aHeader->From());
		}
	q=q->ToNextL();

	// To information
	DBG((LogText(_L8("Processing 'To' information"))));

	ProcessAddressListL(&address,aHeader->ToRecipients(),q->Child());
	q=q->ToNextL();
			
	// CC list
	DBG((LogText(_L8("Processing 'CC' information"))));

	ProcessAddressListL(&address,aHeader->CcRecipients(),q->Child());
	q=q->ToNextL();

	// BCC list
	DBG((LogText(_L8("Processing 'BCC' information"))));

	ProcessAddressListL(&address,aHeader->BccRecipients(),q->Child());
	q=q->ToNextL();

	// In-Reply-To
	q=q->ToNextL();

	// Message-Id
	aHeader->SetImMsgIdL(q->AtomNoAngleBrackets());

	// Decode any QP encoding in header fields
	iHeaderConverter->DecodeAllHeaderFieldsL(*aHeader);

	// Set from line in TMsvEntry
	aEntry.iDetails.Set(aHeader->From());

	// Set subject in TMsvEntry
	aEntry.iDescription.Set(aHeader->Subject());

	// Get rid of buffer
	CleanupStack::PopAndDestroy();

	DBG((LogText(_L8("Finished processing envelope information"))));
	}

void CImImap4Session::StripSpace(HBufC8* aBuf)
	{
	TInt len = aBuf->Length();
	TInt in = 0;
	TInt out = 0;
	TPtr8 p = aBuf->Des();
	while (in < len)
		{
		TUint8 c = p[in++];
		if (c > ' ')
			p[out++] = c;
		}
	// we could shrink the buffer here but we won't bother because it
	// is going to get copied and freed anyway
	}

// Fill in a CImHeader from the extra header fields atoms, currently
// Priority and Receipt info. aText is an extract direct from the
// message header, ie lines of name: value\r\n terminated with an
// empty line. Not known whether the lines can be folded so assume
// they may.
void CImImap4Session::ProcessHeaderExtraL(CImHeader* aHeader, CImMimeHeader* aMimeHeader, TMsvEmailEntry* aEntry, TPtrC8 aText)
	{
#ifdef _DEBUG
	TPtrC8 dump = aText.Left(256);
	DBG((LogText(_L8("Processing HeaderExtra data '%S'"), &dump)));
#endif

	// utils class
	CImcvUtils* utils=CImcvUtils::NewLC();

	TPtrC8 line = aText;
	HBufC8* valueBuf = NULL;

	TPtrC8 name;

	TBool foundReplyToPrompt = EFalse;
	// Check for content-type Application/xxx
	// There may be a CAF agent ready to consume the content if it's DRM
	// If aMimeHeader is set then this is the mime header prior to the actual mime section download
	if(aMimeHeader && aMimeHeader->ContentType().MatchF(KImcvApplication) == 0)
		{		
		// CAF registration requires concatenated content-type and subtype
		// The type and subtype have been received and stored.
		// Create buffer for concatenating. + 1 creates space for '/' 
		HBufC8* buf = HBufC8::NewLC(aMimeHeader->ContentSubType().Length() + aMimeHeader->ContentType().Length() + 1);
		TPtr8 ptr(buf->Des());
		ptr.Copy(aMimeHeader->ContentType());
		ptr.Append(KImcvForwardSlash);
		ptr.Append(aMimeHeader->ContentSubType());
		// Registration does not necessarily succeed but we don't care at this point.
		iCaf->RegisterL(ptr);
		CleanupStack::PopAndDestroy(buf);
		}

	while (line.Length())
		{
		TBool processPrevious = valueBuf != NULL;
		TPtrC8 current;

		TInt len = line.Find(KImcvCRLF);
		if (len > 0)
			{
			// split line into this one and the rest
			current.Set(line.Left(len));
			line.Set(line.Mid(len+2));

			// handle folded headers
			if (current[0] <= ' ' && valueBuf)
				{
				HBufC8* buf=valueBuf->ReAllocL( valueBuf->Length() + current.Length() );
				if (buf!=valueBuf)
					{
					CleanupStack::Pop();
					CleanupStack::PushL(valueBuf=buf);
					}
				valueBuf->Des().Append( current );

				processPrevious = EFalse;
				}
			}
		else
			{
			// set line to null
			line.Set(line.Left(0));
			}
		
		// find matching headers, can only be set if valueBuf was
		// non-null
		if (processPrevious)
			{
			// Dont put the following line back in as will cause a panic if the subject
			// field is too long. Defect EXT-53KD67.
			//DBG((LogText(_L8("header: name %S value %S"), &name, valueBuf)));

			if (aEntry)
				{
				CDesC8ArrayFlat* array = new(ELeave) CDesC8ArrayFlat(4);
			  	CleanupStack::PushL(array);
			  	CImcvUtils::PriorityFieldsL(*array);
			  	for (TInt i(0); i<array->Count(); i++)
				  	{
					if(name.CompareF((*array)[i])==0)
				  		{
						aEntry->SetPriority(utils->EvaluatePriorityText(*valueBuf));
				  		}
				  	}
			  	CleanupStack::PopAndDestroy(array); 
				}

			if (aHeader)
				{
				CImcvUtils* imcvUtils = CImcvUtils::NewLC();
				if (imcvUtils->EvaluateReturnReceiptFields(name))
					{
					aHeader->SetReceiptAddressL(*valueBuf);

					// Only set Receipt flag if this email has not
					// been 'seen' by somebody - to prevent multiple
					// notifications
					if (!aEntry->SeenIMAP4Flag())
						aEntry->SetReceipt(ETrue);
					}
					
				if((name.CompareF(KImcvFromPrompt))==0)
					{
					// Set from line in TMsvEntry
					aHeader->SetFromL(*valueBuf);
					}
				else if((name.CompareF(KImcvSubjectPrompt))==0)
					{
					// Set subject in TMsvEntry
					aHeader->SetSubjectL(*valueBuf);
					}
				else if((name.CompareF(KImcvDatePrompt))==0)
					{
					if(!iParsedTime)
						{
						// Set date in TMsvEntry
						TImRfc822DateField date;
						date.ParseDateField(*valueBuf,aEntry->iDate);	
						}
					}
				else if((name.CompareF(KImcvReceivedPrompt))==0)
					{
					if(!iParsedTime)
						{
						// Set date in TMsvEntry
						TImRfc822DateField date;
						
						//remove the data before the comma, to just leave the date				
						TPtr8 ptr(valueBuf->Des());
						TInt lPos=ptr.Locate(';');	
						ptr = ptr.Right(ptr.Length()-lPos-2);				
						date.ParseDateField(ptr,aEntry->iDate);
						iParsedTime=ETrue;	
						}
					}
				else if((name.CompareF(KImcvReplyToPrompt))==0)
					{
					aHeader->SetReplyToL(*valueBuf);
					foundReplyToPrompt = ETrue;
					}
				else if((name.CompareF(KImcvMessageIdPrompt))==0)
					aHeader->SetImMsgIdL(*valueBuf);
				else if((name.CompareF(KImcvToPrompt))==0)
					ProcessAddressListL(aHeader->ToRecipients(), &valueBuf);
				else if((name.CompareF(KImcvCcPrompt))==0)
					ProcessAddressListL(aHeader->CcRecipients(), &valueBuf);
				else if((name.CompareF(KImcvBccPrompt))==0)
					ProcessAddressListL(aHeader->BccRecipients(), &valueBuf);

				CleanupStack::PopAndDestroy(); // imcvUtils

				// we are currently ignoring DispositionOptions as
				// there is nowhere to store it
				}

			if (aMimeHeader)
				{
				// Check to see if this extra header data should be passed to the CAF agent				
				if(iCaf->Registered())
					{
					iCaf->AddToMetaDataL(name,valueBuf->Des());
					}
				if (name.CompareF(KImcvContentBase) == 0)
					{
					StripSpace(valueBuf);
					aMimeHeader->SetContentBaseL(*valueBuf);
					}
				else if (name.CompareF(KImcvContentLocation) == 0)
					{
					StripSpace(valueBuf);

					HBufC *decoded=HBufC::NewLC(valueBuf->Length());
					TPtr decoded_ptr(decoded->Des());

					iHeaderConverter->DecodeHeaderFieldL(*valueBuf,decoded_ptr);
					aMimeHeader->SetContentLocationL(*decoded);
					CleanupStack::PopAndDestroy(); // decoded
					}
				}

			CleanupStack::PopAndDestroy(); // valueBuf
			valueBuf = NULL;
			}

		if (current.Length() && current[0] > ' ')
			{
			// split this line into name and value
			TInt colon = current.Locate(':');

			name.Set(current.Left(colon+1)); // include the colon
			TPtrC8 value = current.Mid(colon+1);
			
			// skip any initial WS in the value
			while (value.Length() != 0 && value[0] <= ' ')
				value.Set(value.Mid(1));

			valueBuf = value.AllocLC();
			}
		}	

	if (aHeader)
		{
		// If no reply to information, use the From value
		if (!foundReplyToPrompt)
			{
			aHeader->SetReplyToL(aHeader->From());
			}

		// Decode any QP encoding in header fields
		iHeaderConverter->DecodeAllHeaderFieldsL(*aHeader);

		// Set from line in TMsvEntry
		aEntry->iDetails.Set(aHeader->From());

		// Set subject in TMsvEntry
		aEntry->iDescription.Set(aHeader->Subject());
		}	

	// just in case 
	if (valueBuf)
		CleanupStack::PopAndDestroy(); // valueBuf

	// pop off the items allocated
	CleanupStack::PopAndDestroy(); // utils
	
	DBG((LogText(_L8("Finished processing HeaderExtra"))));
	}

// adapted this function from the one in CImRecvConvert
void CImImap4Session::GetDefaultFilename(TDes& aName, const TMsvEmailEntry& aMessage, const CImMimeHeader* mime)
	{
	aName = *iDefaultAttachmentName;

	// Add on appropriate extension
	if (aMessage.iType == KUidMsvEmailTextEntry)
		{
		aName.Append(KTextExtension);
		}
	else if (aMessage.MHTMLEmail())
		{
		aName.Append(KHtmlExtension);
		}
	else if (aMessage.VCard() || aMessage.VCalendar())
		{
		aName.Append(KVCardExtension);
		}
	else if (aMessage.ICalendar())
		{
		aName.Append(KICalExtension);
		}
	else if ( aMessage.iType == KUidMsvAttachmentEntry )
		{
 		if ( (mime->ContentSubType()==KImcvBmp) ||
			 (mime->ContentSubType()==KImcvGif) ||
			 (mime->ContentSubType()==KImcvJpeg) ||
			 (mime->ContentSubType()==KImcvTiff) ||
			 (mime->ContentSubType()==KImcvWav) )
			{
			TBuf<KMaxExtensionLength> buf;
			buf.Copy(mime->ContentSubType());
			aName.Append(KImcvFullStop);
			aName.Append(buf);
			}
		}
	}

TBool CImImap4Session::DoesAtomContainAttachment(CImapAtom *aAtom)
// Check through all of this Atom's Siblings to see if they contain an attachment
	{
	TBool hasAttachment = EFalse;
	CImapAtom* currentAtom = aAtom;

	// Search through all of the Sibling Atoms
	while (currentAtom != NULL)
		{
		// Check if there is a Child Atom with an Attachment
		if (currentAtom->Child() != NULL)
			{
			if (currentAtom->Child()->Compare(KMIME_ATTACHMENT))
				{
				// This Sibling contains an Attachment.
				hasAttachment = ETrue;
				break;
				}
			}

		// Move onto the next sibling
		currentAtom = currentAtom->Next();
		}

	return hasAttachment;
	}

// Build a single entry
void CImImap4Session::BuildTreeOneL(const TMsvId aParent, CImapAtom *aAtom, const TDesC8& aPath,
									const TMsvId aThisMessage, TInt& aAttachments, TBool& aIsMHTML, TInt& aRelatedAttachments)
	{
	DBG((LogText(_L8("BuildTreeOneL(message=%x, parent=%x)"),aThisMessage,aParent)));

	// First, is this actually an entry, or another level of nesting?
	if (aAtom->Child())
		{
		// Another level of nesting? Call BuildTreeL()
		BuildTreeL(aParent,aAtom,aPath,aThisMessage,aAttachments,aIsMHTML, aRelatedAttachments);
		return;
		}

	// Skeleton for new entry
	SetEntryL(aParent);

	TFileName attachmentFilename;	//	DS somewhere to store an attachment filename
	TMsvEmailEntry message;
	message.iSize=0;
	message.iMtm=KUidMsgTypeIMAP4;
	message.iServiceId=iServiceId;
	message.SetUID(iMessageUid);
	message.SetValidUID(ETrue);
	message.SetComplete(EFalse);

	// Reply from server is in this form:
	// TYPE SUBTYPE (PARAM1 VALUE1 ...) ID DESCRIPTION ENCODING OCTETS
	//
	// Text parts:
	// TYPE SUBTYPE (PARAM1 VALUE1 ...) ID DESCRIPTION ENCODING OCTETS NLINES

	// Save mime TYPE/SUBTYPE
	CImMimeHeader *mime=CImMimeHeader::NewLC();
	CImapAtom *type=aAtom;
	CImapAtom *subtype=aAtom->Next();
	mime->SetContentTypeL(type->Atom());
	mime->SetContentSubTypeL(subtype->Atom());

#ifdef PRINTING
	TPtrC8 mt=type->Atom(),ms=subtype->Atom();
	LogText(_L8("  MIME type %S/%S"),&mt,&ms);
#endif

	// We start by assuming the data will be stored as a binary
	// attachment
	message.iType=KUidMsvAttachmentEntry;
	if (type->Compare(KMIME_TEXT))
		{
		// text/html?
		if (subtype->Compare(KMIME_HTML))
			{
			//  If this Atom doesn't contain an Attachment, then this is a MHTML Message.
			if (!DoesAtomContainAttachment(subtype))
				{
				message.iType=KUidMsvEmailHtmlEntry;
				aIsMHTML=ETrue;	
				}	
			}
		// text/x-vcard?
		else if (subtype->Compare(KMIME_XVCARD))
			{
			// Set vCard flag in message
			message.SetVCard(ETrue);

			// Defaults to binary
			}
		// text/x-vcalendar
		else if (subtype->Compare(KMIME_VCALENDAR))
			{
			// Set vCalendar flag in message
			message.SetVCalendar(ETrue);
			iIsVCalendar = ETrue;
			
			// Defaults to binary
			}
		// text/calendar
		else if (subtype->Compare(KMIME_ICALENDAR))
			{
			// Set iCalendar flag in message
			message.SetICalendar(ETrue);
			iIsICalendar = ETrue;
			
			// Defaults to binary
			}
		else
			message.iType=KUidMsvEmailTextEntry;
		}

	// ...and mime path
	mime->SetRelativePathL(aPath);

	DBG((LogText(_L8("  MIME path %S"),&aPath)));

	// Parameter list
	CImapAtom *parameter=subtype->ToNextL();

	TUint charset = KUidMsvCharsetNone;

	// Store parameter stuff
	if (!parameter->Compare(KIMAP_NIL))
		{
		DBG((LogText(_L8("  Parameter list:"))));

		// Process list
		CImapAtom *type_param=parameter->ToChildL();
		while(type_param && type_param->Next())
			{
			CImapAtom *type_value;
			type_value=type_param->ToNextL();

			// All items are 2-tuples (parameter value (...)): get both, and store
			TPtrC8 param=type_param->Atom();
			TPtrC8 value=type_value->Atom();

			DBG((LogText(_L8("    %S %S"),&param,&value)));
		
			mime->ContentTypeParams().AppendL(param);
			mime->ContentTypeParams().AppendL(value);
		
			// Have we come across a 'NAME' tuple? If so, force the MIME type of this
			// entry to be an attachment.
			if ((param.CompareF(KMIME_NAME)==0)
				|| (param.CompareF(KMIME_NAME_RFC2231) == 0))
				{
				DBG((LogText(_L8("It has an attachment filename, therefore this is an attachment"))));

				FindFilenameDecodeL(*mime,attachmentFilename);
				StripIllegalCharactersFromFileName(attachmentFilename);
				message.iDetails.Set(attachmentFilename);

				// If embedded message do not save as an attachment
				if (message.iType!=KUidMsvMessageEntry)
					message.iType=KUidMsvAttachmentEntry;
				}
			else if (param.CompareF(KImcvCharset)==0)
				{
				// Set the Mime charset from the parameter value
				if (value.Length() != 0)
					{
					charset = iCharConv->GetMimeCharsetUidL(value);
					}
				}


			// Next item
			type_param=type_value->Next();
			}
		}

	mime->SetMimeCharset(charset);

	// ID: save it
	CImapAtom *id=parameter->ToNextL();
	if (!id->Compare(_L8("NIL")))
		mime->SetContentIDL(id->AtomNoAngleBrackets());

	// Description: save it
	CImapAtom *description=id->ToNextL();
	if (!description->Compare(_L8("NIL")))
		mime->SetContentDescriptionL(description->Atom());

	// Encoding
	CImapAtom *encoding=description->ToNextL();
	mime->SetContentTransferEncodingL(encoding->Atom());

#ifdef PRINTING
	TPtrC8 enc=encoding->Atom();
	LogText(_L8("  Encoding %S"),&enc);
#endif

	// Octets (encoded form)
	CImapAtom *octets=encoding->ToNextL();
	TInt actualsize;
	if (octets->Value(actualsize)!=KErrNone)
		User::Leave(KErrGeneral);

	// Twiddle this to show *decoded* size: this is basically the size of
	// this part, multiplied by 6/8 if it's BASE64 encoded. For all other
	// encodings, we leave the size as-is as there's no hard & fast rule
	// which can be applied.
	if (encoding->Compare(KMIME_BASE64))
		message.iSize=(actualsize*6)/8;
	else
		message.iSize=actualsize;

	// Add into total message size
	iDecodedSizeOfAllParts+=message.iSize;

	// Store *remote* size in a dodgy place
	message.iBioType=actualsize;

	//If any part of email (text/plain mime, text/html mime, attachment....) 
	// is empty then should not fetch it.
	if(actualsize == 0)
		{
		message.SetComplete(ETrue);
		}

#ifdef PRINTING
	LogText(_L8("  Octets %d"),message.iBioType);

	TPtrC8 type_p=type->Atom(),subtype_p=subtype->Atom();
	LogText(_L8("Building mime stuff: %S/%S"),&type_p,&subtype_p);
#endif

	// MD5 block will start after any optional parts
	CImapAtom *md5;

	if (type->Compare(KMIME_MESSAGE) && subtype->Compare(KMIME_RFC822))
		{
		// Skip RFC822 header, which should *all* be present
		// Like this for clarity
		CImapAtom *envelope=octets->ToNextL();
		CImapAtom *structure=envelope->ToNextL();
		CImapAtom *nooflines=structure->ToNextL();

		// embedded message - marked as a message
		message.iType=KUidMsvMessageEntry;

		iDecodedSizeOfAllParts-=message.iSize;

		// Next atom is MD5 - IF PRESENT
		md5=nooflines->Next();
		}
	else
		{
		// Find MD5 block: if this part is TEXT/* we have number of lines next
		if (type->Compare(KMIME_TEXT))
			{
			// Number of lines is next atom, followed by MD5 - IF PRESENT
			CImapAtom *nooflines=octets->ToNextL();
			md5=nooflines->Next();
			}
		else		
			md5=octets->Next();
		}

	// Do we have any extended fields? If so, deal with them
	if (md5)
		{
		// Next (if present) is Content-Disposition, closely followed by language
		CImapAtom *disposition=md5->Next();
		CImapAtom *language=(disposition==NULL)?NULL:disposition->Next();
		language=language; // Stop .aer warnings: we know it's not used (yet)

		DBG((LogText(_L8("Processing content-disposition"))));

		// Store disposition stuff
		if (disposition && !disposition->Compare(KIMAP_NIL))
			{
			// Process list
			CImapAtom *pos=disposition->Child();
			while(pos)
				{
				// Single item (eg "INLINE") or 2-tuple (eg ("FILENAME" "blah.gif"))?
				if (pos->Child())
					{
					// Tuple
					CImapAtom* tuple = pos->ToChildL();
					while(tuple)
						{
						mime->ContentDispositionParams().AppendL(tuple->Atom());
						mime->ContentDispositionParams().AppendL(tuple->ToNextL()->Atom());

						// Filename? If so, force this as an attachment
						if ((tuple->Atom().CompareF(KMIME_FILENAME)==0)
							|| (tuple->Atom().CompareF(KMIME_FILENAME_RFC2231)==0))
							{
							DBG((LogText(_L8("It has an attachment filename, therefore this is an attachment"))));
							FindFilenameDecodeL(*mime,attachmentFilename);
							StripIllegalCharactersFromFileName(attachmentFilename);
							message.iDetails.Set(attachmentFilename);

							// If embedded message do not save as an attachment
							if (message.iType!=KUidMsvMessageEntry)
								message.iType=KUidMsvAttachmentEntry;
							}

						// Skip to next tuple
						tuple = tuple->ToNextL()->Next();
						}
					}
				else
					{
					// Single item
					mime->ContentDispositionParams().AppendL(pos->Atom());
					mime->ContentDispositionParams().AppendL(_L8(""));
					}
			
				// Skip to next entry
				pos=pos->Next();
				}
			}
		}

	// Now we're working on the type
	if (message.iType==KUidMsvMessageEntry)
		{
		// MESSAGE/RFC822
		// This means that the next atom will be the envelope info, and the
		// one following that will be the body structure of the embedded
		// message.
		//
		// This is an entire message-within-a-message and so gets treated like
		// an actual mail (has it's own multipartdata thing)
	
		// Make CImHeader bits
		CImHeader *messageheader=CImHeader::NewLC();
		CImapAtom *envelope=octets->ToNextL();
		ProcessEnvelopeL(messageheader,message,envelope);

		// Create message
		User::LeaveIfError(iEntry->CreateEntryBulk(message));
		SetEntryL(message.Id());

		// Store CImHeader bits
		CMsvStore* entryStore=iEntry->EditStoreL();
		CleanupStack::PushL(entryStore);
		messageheader->StoreL(*entryStore);
		mime->StoreL(*entryStore);
		entryStore->CommitL();
		CleanupStack::PopAndDestroy(3);

#if SET_RELATED_ID
		//	DS - Set message's iRelatedId to messageId to allow later UI kludges
		TMsvEntry changeEntry(iEntry->Entry());
		changeEntry.iRelatedId=changeEntry.Id();
		ChangeEntryBulkL(changeEntry);
#endif
		// Descend into attachments of this embedded message
		CImapAtom *structure=envelope->ToNextL();
		TInt attachments=0;
		TBool isMHTML=EFalse;

		BuildTreeL(message.Id(),structure->ToChildL(),aPath,message.Id(),attachments,isMHTML,aRelatedAttachments);
		DBG((LogText(_L8("Build embedded message id %x attachments %d MHTML %d"),message.Id(),attachments,isMHTML)));

		// Save attachment and MHTML flags
		if (attachments>0 || isMHTML)
			{
			SetEntryL(message.Id());
			TMsvEmailEntry thisMessage=iEntry->Entry();

			if (attachments>0)
				{
				thisMessage.SetAttachment(ETrue);
				}

			if (isMHTML)
				{
				thisMessage.SetMHTMLEmail(ETrue);
				}

			ChangeEntryBulkL(thisMessage);
			}

		// we are now counting embedded messages as attachments
		aAttachments++;
		}
	else
		{
		// Something else - create an attachment entry
		SetEntryL(aParent);

		// save parent folder type
		TImEmailFolderType parentFolderType = ((TMsvEmailEntry)iEntry->Entry()).MessageFolderType();
	
		// set attachment and HTML flags on item
		if ( message.iType==KUidMsvAttachmentEntry)
			message.SetAttachment(ETrue);

		if ( message.iType==KUidMsvEmailHtmlEntry)
			message.SetMHTMLEmail(ETrue);

		// ensure there is a filename if it is a non-text item (which
		// also controls iFetchIsText, the flag used in DecodeAndStore
		// to say whether to stream to a file or RichText store.
		if (message.iType!=KUidMsvEmailTextEntry && message.iDetails.Length() == 0)
			{
			// use iAttachmentName for temporary buffer
			GetDefaultFilename(iAttachmentName, message, mime);
			message.iDetails.Set(iAttachmentName);
			}
		
		User::LeaveIfError(iEntry->CreateEntryBulk(message));
		SetEntryL(message.Id());
		
		DBG((LogText(_L8("Created attachment id %x as child of %x - type %d"),message.Id(),aParent, parentFolderType)));

#if SET_RELATED_ID
		//	DS - Set message's iRelatedId to messageId to allow later UI kludges
		TMsvEntry changeEntry(iEntry->Entry());
		changeEntry.iRelatedId=changeEntry.Id();
		ChangeEntryBulkL(changeEntry);
#endif

		DBG((LogText(_L8("Streaming MIME info into id %x"),iEntry->Entry().Id())));

		// Stream the MIME info out into the message
		// This will either stream it to the actual message (if the above if
		// evaluated to True, the entry is still set to the message), or to
		// the newly created child
		CMsvStore* entryStore=iEntry->EditStoreL();
		CleanupStack::PushL(entryStore);
		mime->StoreL(*entryStore);
		entryStore->CommitL();	
		CleanupStack::PopAndDestroy(2, mime);

    	// This entry is NOT an attachment in the following cases - 
		// 1)	This is an attachment whose parent is a MULTIPART/RELATED folder.
		//		In this case, this entry could be a image entity for an MHTML
		//		entry with the same parent.
		// 2)	This is an MHTML entry whose parent is a MULTIPART/ALTERNATIVE 
		//		folder. In this case, this entry is the MHTML alternative to a
		//		text entry with the same parent.
		// 3)	This is an MHTML entry whose parent is MESSAGE folder. In this
		//		case, the message is a simple MHTML message with no text 
		//		alternative or embedded image.
		// 4)	This is an MHTML entry whose parent is a MULTIPART/RELATED folder.
		//		In this case, this entry is the MHTML for the message.
		// 5)	This is an MHTML entry whose parent is a MULTIPART/MIXED folder.
		//		In this case, this entry is the MHTML for the message. It cannot
		//		be the attachment it self as then it would be of type attachment.
		// Therefore, an entry is only an attachment if is of type attachment and
		// its parent is not a MULTIPART/RELATED folder.
    	if( message.iType==KUidMsvAttachmentEntry && parentFolderType != EFolderTypeRelated )
			{
    		++aAttachments;
			}
		// if it is related we might want to include it if the message
		// turns out not to be MHTML
		else if ( message.iType==KUidMsvAttachmentEntry &&
			parentFolderType == EFolderTypeRelated )
			{
			++aRelatedAttachments;
			}
		}

	DBG((LogText(_L8("BuildTreeOneL done: created id %x, attachments so far %d"), message.Id(), aAttachments)));
	}

// Build attachment tree below a message
void CImImap4Session::BuildTreeL(TMsvId aParent, CImapAtom *aAtom, const TDesC8& aPath,
								 const TMsvId aThisMessage, TInt& aAttachments, TBool& aIsMHTML, TInt& aRelatedAttachments)
	{
	DBG((LogText(_L8("BuildTreeL(message=%x, parent=%x"),aThisMessage,aParent)));

	// One attachment only?
	if (aAtom->Child()==NULL)
		{
		// Deal with the single entry (doesn't use AllocL)
		HBufC8* newpath=HBufC8::NewLC(aPath.Length()+4);
		*newpath=aPath;
		if (aPath.Length())
			newpath->Des().Append(_L8("."));
		newpath->Des().AppendNum(1);
		BuildTreeOneL(aParent,aAtom,newpath->Des(),aThisMessage,aAttachments,aIsMHTML, aRelatedAttachments);
		CleanupStack::PopAndDestroy();
		}
	else
		{
		// Nest down a level: create a folder
		SetEntryL(aParent);
		TMsvEmailEntry message;
		message.iMtm=KUidMsgTypeIMAP4;
		message.iServiceId=iServiceId;
		message.iType=KUidMsvFolderEntry;
		message.iSize=0;
		message.SetComplete(EFalse);
		User::LeaveIfError(iEntry->CreateEntryBulk(message));

		DBG((LogText(_L8("Created attachment folder id %x as child of %x"),message.Id(),aParent)));

		aParent=message.Id();

		// CC:Mail server doesn't respond to BODYSTRUCTURE correctly:
		// it gives the same response as FETCH BODY, ie it doesn't have
		// all the extended MIME stuff.
		// Skip to the last 4 atoms: this is the multipart type & stuff
		CImapAtom *multipart=aAtom;
		while(multipart && multipart->Child()!=NULL)
			multipart=multipart->Next();

		// Got anything?
		if (multipart)
			{
			// Parse multipart type string, do this first so
			// information is available when parsing children
			TImEmailFolderType ft=EFolderTypeUnknown;
			if (multipart->Compare(KImcvRelated))
				ft=EFolderTypeRelated;
			if (multipart->Compare(KImcvMixed))
				ft=EFolderTypeMixed;
			if (multipart->Compare(KImcvParallel))
				ft=EFolderTypeParallel;
			if (multipart->Compare(KImcvAlternative))
				ft=EFolderTypeAlternative;
			if (multipart->Compare(KImcvDigest))
				ft=EFolderTypeDigest;
			
			SetEntryL(aParent);
			
			// ...and save it
			TMsvEmailEntry folder=iEntry->Entry();
			folder.SetMessageFolderType(ft);
#if SET_RELATED_ID
			//	DS - Set message's iRelatedId to messageId to allow later UI kludges
			folder.iRelatedId=folder.Id();
#endif
			ChangeEntryBulkL(folder);

			// Process the multipart object
			TInt subnr=1;
			while(aAtom && aAtom!=multipart)
				{
				// Tag or child?
				if (aAtom->Child())
					{
					// Process item (doesn't use AllocL)
					HBufC8* newpath=HBufC8::NewLC(aPath.Length()+4);
					*newpath=aPath;
					if (aPath.Length())
						newpath->Des().Append(_L8("."));
					newpath->Des().AppendNum(subnr++);
					BuildTreeOneL(aParent,aAtom->ToChildL(),newpath->Des(),
								  aThisMessage,aAttachments,aIsMHTML, aRelatedAttachments);
					CleanupStack::PopAndDestroy();
					}
		
				// Next item
				aAtom=aAtom->Next();
				}
			}
		}
	}


// convert text from its charset and write to richtext store. aText
// can span multiple and partial lines
void CImImap4Session::WriteToBodyL(const TDesC8& aText)
	{
	TInt pos = iMessageBody->DocumentLength();

	// Add bits of body text, converting along the way, till no characters left
	// .. to convert.

	// Convert text before writing to body.
	TInt rem = 0;

	// there will be a max of one output char per input byte
	HBufC16* text16=HBufC16::NewLC(aText.Length());
	TPtr16 ptr16=text16->Des();

	if (!iPreparedToConvert)
		{
		ptr16.Copy(aText);
		iMessageBody->InsertL(pos, ptr16);
		}
	else
		{
		TInt unconvertedChars, firstPos; // not used 
		rem = iCharConv->ConvertToOurCharsetL(aText, ptr16, 
											  unconvertedChars, firstPos);
		if (rem < 0) // error
			{
			// Copy unconverted characters.
			ptr16.Copy(aText);
			iMessageBody->InsertL(pos, ptr16);
			}
		else if (rem && rem < iLeftOver.MaxLength())
			iLeftOver.Copy(aText.Right(rem));	

		// convert CRLF to ELineBreak
		TInt start = 0;
		TInt length = ptr16.Length();
		TInt i;
		for (i=1; i<length; i++)
			{
			if (ptr16[i-1] == KImcvCR && ptr16[i] == KImcvLF)
				{
				ptr16[i-1] = CEditableText::ELineBreak;

				// write this line to body
				TPtrC ptr = ptr16.Mid(start, i-start);
				iMessageBody->InsertL(pos, ptr);
				pos += ptr.Length();
				start = i+1;
				}
			}

		if (start != i)
			{
			TPtrC ptr = ptr16.Mid(start, i-start);
			iMessageBody->InsertL(pos, ptr);
			}
		}

	CleanupStack::PopAndDestroy(); // text16
	}
	
// convert text from its charset and write to file, return error code
// from write
TInt CImImap4Session::WriteToAttachmentL(const TDesC8& aText)
	{
	TInt error;
	
	// Convert text before writing to attachment.
	TInt rem = 0;

	// there will be a max of one output char per input byte
	HBufC16* text16=HBufC16::NewLC(aText.Length());
	TPtr16 ptr16=text16->Des();

	if (!iPreparedToConvert)
		{
		if(iCaf->Processing())
			{
			error = iCaf->WriteData(aText);
			}
		else
			{
			error = iAttachmentFile->WriteFile(aText);
			}
		}
	else
		{
		TInt unconvertedChars, firstPos; // not used 
		rem = iCharConv->ConvertToOurCharsetL(aText, ptr16, 
											  unconvertedChars, firstPos);
		if (rem < 0) // error
			{
			ptr16.Copy(aText);  // Copy unconverted characters.
			}
		else if (rem && rem < iLeftOver.MaxLength())
			{
			// any remainder is due to partial code sequence not lack of space
			iLeftOver.Copy(aText.Right(rem));
			}

		TPtrC8 text8((TUint8*) text16->Des().Ptr(), text16->Des().Size());
		if(iCaf->Processing())
			{
			error = iCaf->WriteData(text8);
			}
		else
			{
			error = iAttachmentFile->WriteFile(text8);
			}
		}

	CleanupStack::PopAndDestroy(); // text16

	return error;
	}

// Copied and adapted this function from the one in CImRecvConvert
TBool CImImap4Session::CheckUUEStartL(const TDesC8& aSourceLine)
	{
	// Checks if the descriptor contains the UUE begin header
	// Extracts the file name if it is
	
	TInt sourceLength = aSourceLine.Length();
	if(sourceLength < KImcvUueStart().Length()+3) // can't be "begin ###", it's not long enough; 3=length of ###
		return EFalse;

	if(!aSourceLine.Left(KImcvUueStart().Length()).CompareF(KImcvUueStart)) // start of line might be UUE boundary
		{
		// we also need to check that the next three chars are numbers - Unix file access code
		const TUint8* sourceLinePtr = aSourceLine.Ptr();
		TInt length=KImcvUueStart().Length();// this defines length as 6 ie. "b e g i n  " 
		if( TChar(sourceLinePtr[length]).IsDigit() && 
			TChar(sourceLinePtr[length+1]).IsDigit() && 
			TChar(sourceLinePtr[length+2]).IsDigit() )
			{
			// Found 'begin ###' at the start of a line - assume this is a UUencode header
			// The attachment name in this header is ignored. We use the value from the MIME header
			return ETrue;
			}
		}
		
	return EFalse;
	}


// Decode and store received data
void CImImap4Session::DecodeAndStoreL(const TPtrC8& aBodyData, const TBool aEndOfStream)
	{
	DBG((LogText(_L8("DecodeAndStore(%d bytes, endofstream=%d, encoding=%d, iLeftOver=%d)"),aBodyData.Length(),aEndOfStream,iEncodingType,iLeftOver.Length())));

	// Somewhere to store decoded data, at least as long as source (plus anything we have left
	// in the partial line buffer which may now get consumed)
	TInt outputbuffersize=aBodyData.Length()+4;
	if (iPartialLine)
		outputbuffersize+=iPartialLine->Des().Length();

	HBufC8* decoded=HBufC8::NewLC(outputbuffersize);
	TPtr8 decoded_ptr=decoded->Des();

	// Bump progress: bytesdone is *encoded* length, so we just use the encoded length
	iProgress.iBytesDone+=aBodyData.Length();
	// Which decoder are we using?
	switch(iEncodingType)
		{
		case EEncodingTypeNone:
		case EEncodingType7Bit:
		case EEncodingType8Bit:
		case EEncodingTypeBinary:
		case EEncodingTypeUnknown:
			// Nothing to do, just copy data
			decoded->Des().Append(aBodyData);
			break;

		case EEncodingTypeBASE64:
			// Decode Base64 data: just filter it through decoder, it
			// ignores line breaks anyway.
			iB64Decoder.Decode(aBodyData,decoded_ptr);
			break;

		case EEncodingTypeUU:
			{
			TPtrC8 bodydata=aBodyData;

			// Got a partial buffer?
			if (!iPartialLine)
				{
				// Allocate buffer
				iPartialLine=HBufC8::NewL(KUuDecodedLineLength);
				iUUDecoding = EFalse;
				}
			
			// Decode UUEncoded data: line by line
			TBool decodeEnded = EFalse;
			TInt position=0;
			while ( bodydata.Length() && !decodeEnded )
				{
				// Find() returns the start of "\r\n". The decoding algorithm
				// requires that the encoded line contains the "\r\n".
				TInt lineEnd = bodydata.Find( _L8("\r\n") );
				if (lineEnd != KErrNotFound)
					{
					lineEnd = lineEnd + 2;
					AppendExtendL( &iPartialLine, bodydata.Left( lineEnd ), EFalse);
				
					bodydata.Set( bodydata.Mid( lineEnd ) );
				
					// Check for a well-formated  begin-tag
					if ( CheckUUEStartL( iPartialLine->Des() ) )
						{
						iUUDecoding = ETrue;
						}
					else if ( iPartialLine->Compare( KImcvUueEnd ) != 0 && iUUDecoding )
						{
						// Every malformatted string is decoded as an empty string 
						// with length 0. Appending such a string is harmless.
						TPtr8 destination((unsigned char*)decoded_ptr.Ptr()+position,0,outputbuffersize-position);
						iUUDecoder.Decode(*iPartialLine,destination);
						position+=destination.Length();			
						}
					else if ( iUUDecoding )
						{
						decodeEnded = ETrue;
						iUUDecoding = EFalse;	
						}
					
					iPartialLine->Des().Zero();
					}
				else
					{
					AppendExtendL( &iPartialLine, bodydata, EFalse);
					
					// advance to end of bodydata
  					bodydata.Set(bodydata.Ptr()+bodydata.Length(), 0);
					}
				}
			decoded->Des().SetLength(position);	
			break;
			}

		case EEncodingTypeQP:
			{
			TPtrC8 bodydata=aBodyData;

			// Got a partial buffer?
			if (!iPartialLine)
				{
				// Allocate buffer
				iPartialLine=HBufC8::NewL(256);
				}

			// Build buffer to decode: basically, QP decoder wants CRLF terminated
			// lines, so we build them in the iPartialLine buffer. There may be
			// stuff already there from previous data packet - so we just append.
			TInt position=0;
			while(bodydata.Length())
				{
				// Find a line break
				TInt lineend=bodydata.Find(_L8("\r\n"));

				// No break?
				if (lineend==KErrNotFound && !aEndOfStream)
					{
					// Stick it all in the partialline buffer, we should get a CRLF
					// soon...
					AppendExtendL( &iPartialLine,bodydata, EFalse);
					break;
					}
				else
					{
					if (lineend==KErrNotFound)
						{
						// Append whole thing left to buffer
						AppendExtendL( &iPartialLine,bodydata, EFalse);

						// advance to end of bodydata
						bodydata.Set(bodydata.Ptr()+bodydata.Length(), 0);
						}
					else
						{
						// Append to buffer up to that point (including the \r\n)
						AppendExtendL( &iPartialLine,bodydata.Left(lineend+2), EFalse);

						// Remove from the buffer we're working on (including the \r\n)
						bodydata.Set(bodydata.Ptr()+lineend+2,bodydata.Length()-lineend-2);
						}

					// Decode & skip on in buffer
					TPtr8 destination((unsigned char*)decoded_ptr.Ptr()+position,0,outputbuffersize-position);
					iQPDecoder.Decode(*iPartialLine,destination);
					position+=destination.Length();
					iPartialLine->Des().Zero();
					}
				}

			// Update decoded
			decoded->Des().SetLength(position);
			break;
			}
		}

	// put back any partially converted data
	if (iLeftOver.Length())
		{
		decoded->Des().Insert(0, iLeftOver);
		iLeftOver.SetLength(0);
		}

	// What format is it? TEXT/* we put into a richtext thingy, otherwise just
	// stream it to store
	if (iFetchIsText)
		{
		if(aEndOfStream && (iMessageBody || iBodyBuf) && iBodyPartRemainingSize)
			{
			CleanupStack::Pop();// decoded 
			TInt newSize = decoded->Size() + iFooterString->Size();
			decoded = decoded->ReAlloc(newSize);
			CleanupStack::PushL(decoded);
			decoded->Des().Append(*iFooterString);
			delete iFooterString;
			iFooterString = NULL;
			}
		// Got somewhere to put it? Store it!
		if (iStore8BitData)
			{
			if (decoded->Length() && iBodyBuf)
				iBodyBuf->InsertL(iBodyBuf->Size(), *decoded);
			}
		else
			{
			if (decoded->Length() && iMessageBody)
				WriteToBodyL(decoded->Des());
			}
		
		
		// Got the whole thing buffered?
		if (aEndOfStream && (iMessageBody || iBodyBuf))
			{
			DBG((LogText(_L8("Doing StoreBodyTextL()"))));

			// The whole message is built in iMessageBody or iBodyBuf.  Store it.
			SetEntryL(iMessageId);
			CMsvStore *entryStore=iEntry->EditStoreL();
			CleanupStack::PushL(entryStore);
			if (iStore8BitData)
				iBodyText->StoreL(*entryStore, *iBodyBuf);
			else
				entryStore->StoreBodyTextL(*iMessageBody);
			entryStore->CommitL();
			CleanupStack::PopAndDestroy();

			// Get rid of body copy, etc
			delete iBodyBuf;
			iBodyBuf = NULL;
			delete iMessageBody;
			iMessageBody=NULL;
			}
		}
	else
		{
		// Select the entry
		SetEntryL(iMessageId);

		// Save it direct to store
		if (iAttachmentFileState==EFileNotOpen)
			{
			// Get and set Attachment File path
			TFileName filepath;
	
			// Retrieving the attachment name from an earlier saved one
			// If it's a CAF interested file then this will get overidden
			CMsvStore* store = iEntry->ReadStoreL();
			CleanupStack::PushL(store);
			MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
			if(attachmentMgr.AttachmentCount())
				{
				// get the file path 	
				CMsvAttachment* attachment = attachmentMgr.GetAttachmentInfoL(0);
				CleanupStack::PushL(attachment);
				filepath = attachment->FilePath();
				CleanupStack::PopAndDestroy(attachment);
				}

			if (iAttachmentFullPath)
				{
				delete iAttachmentFullPath;
				iAttachmentFullPath=NULL;
				}
			if(attachmentMgr.AttachmentCount())
				{
				TParse fileParser;
				User::LeaveIfError(fileParser.Set(filepath, NULL, NULL));
				iAttachmentFullPath=fileParser.DriveAndPath().AllocL();
				}
			// We've already extracted the attachment file name in
			// BuildTree so just copy it out of details
			iAttachmentName=iEntry->Entry().iDetails;			
			if(attachmentMgr.AttachmentCount())
				{
			DBG((LogText(_L8("name '%S', '%S'"),iAttachmentFullPath,&iAttachmentName)));
				}
			if (!iAttachmentFile)
					iAttachmentFile=new (ELeave) TImAttachmentFile(iFs);
			CleanupStack::PopAndDestroy(store); // store opened above
			store = iEntry->EditStoreL(); 
			CleanupStack::PushL(store);
			// Could be multiple attachments in the folder.
			TInt attachmentCount = store->AttachmentManagerL().AttachmentCount();
			for(TInt i=0;i<attachmentCount;i++)
				{
				// Remove [0] as array is shuffled. Once index [n] is removed n+1 becomes n
				store->AttachmentManagerExtensionsL().RemoveAttachmentL(0);
				}
			if(attachmentCount)			
				store->CommitL();

			// Now create the attachment entry
			CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
			CleanupStack::PushL(attachment);
			attachment->SetAttachmentNameL(iAttachmentName);
			
			// Need to create the MIME-type information - first get the MIME headers
			CImMimeHeader* mimeHeaders = CImMimeHeader::NewLC();
			mimeHeaders->RestoreL(*store);

			HBufC8* buf = HBufC8::NewLC(mimeHeaders->ContentSubType().Length() + mimeHeaders->ContentType().Length() + 1);
			TPtr8 ptr(buf->Des());
			ptr.Copy(mimeHeaders->ContentType());
			ptr.Append(KImcvForwardSlash);
			ptr.Append(mimeHeaders->ContentSubType());
		
			attachment->SetMimeTypeL(ptr);
			
			CleanupStack::PopAndDestroy(2, mimeHeaders);

			
			RFile file;	
			if(iCaf->Registered())
				{
				iCaf->PrepareProcessingL(); // Init the CAF import file session
				RFile startFile;
				TFileName suggestedFileName;
				if(iCaf->GetSuggestedAttachmentFileName(suggestedFileName) == KErrNone) // CAF agent may provide a filename
					{
					store->CreateShareProtectedAttachmentL(suggestedFileName,startFile,attachment);
					}
				else
					{
					store->CreateShareProtectedAttachmentL(iAttachmentName,startFile,attachment);
					}
				iCaf->StartProcessing(iDefaultAttachmentName->Des(),attachment->FilePath(),*iEntry,startFile); // Init the CAF session
				startFile.Close();
				}
			else
				{
				// Normal behaviour
				store->AttachmentManagerExtensionsL().CreateAttachmentL(iAttachmentName,file,attachment);
				iAttachmentFile->SetFileHandle(file,TImAttachmentFile::EImFileWrite);
				}

			// CreateAttachmentL takes ownership of CMsvAttachment so if call was successful we can pop it here
			CleanupStack::Pop(attachment);
			
			iAttachmentFileState = EFileIsOpen;	
			store->CommitL();
			CleanupStack::PopAndDestroy(store);	

			if (iAttachmentFileState!=EFileIsOpen)
				{
				DBG((LogText(_L8("Couldn't open file!"))));
				}
			}	

		if (iAttachmentFileState==EFileIsOpen && decoded->Length())
			{
			// write decoded data into a file if there is any data there to write
			TInt error=WriteToAttachmentL(decoded->Des());

			if (error!=KErrNone)
				{
				// the file write failed, (eg.there is no space left set new file state 
				// and skip any remaining encoded data in message
				iAttachmentFileState=EFileIsIncomplete;

				DBG((LogText(_L8("Failed to write %d bytes to attachment file (error=%d): deleting it"),decoded->Length(),error)));
				if(iCaf->Processing())
					{
					iCaf->EndProcessingL();
					}
				else
					{
					iAttachmentFile->CloseFile();
					}

				CMsvStore* store = iEntry->EditStoreL(); 
				CleanupStack::PushL(store);
				// Could be multiple attachments in the folder.
				TInt i;
				TInt attachmentCount = store->AttachmentManagerL().AttachmentCount();
				for(i=0;i<attachmentCount;i++)
					{
					// Remove [0] as array is shuffled. Once index [n] is removed n+1 becomes n
					store->AttachmentManagerExtensionsL().RemoveAttachmentL(0);
					}
				if(attachmentCount)			
					store->CommitL();
				CleanupStack::PopAndDestroy(store);
				TMsvEmailEntry message=iEntry->Entry();
				message.SetAttachment(EFalse);
				ChangeEntryBulkL(message);

				// Leave with the error
				User::Leave(error);
				}
			else
				{
				DBG((LogText(_L8("Written %d bytes to attachment file"),decoded->Length())));
				}
			}

		// Finished?
		if ((aEndOfStream) && (iAttachmentFileState == EFileIsOpen))
			{
			DBG((LogText(_L8("Closing attachment file"))));
			if(iCaf->Processing())
				{
				iCaf->EndProcessingL();
				}
			else
				{
				iAttachmentFile->CloseFile();
				}
			iAttachmentFileState=EFileNotOpen;
			}
		}

	// Free memory
	CleanupStack::PopAndDestroy();
	}

// Given that aId has become complete see if we can propagate the
// Complete state and partial fetch state flag up
void CImImap4Session::PropagateCompleteFlagL(TMsvId aId, TBool aDoBodyText,TBool aPartialFetched)
	{
	CMsvEntrySelection* selection=new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);
	
	// get the siblings of this id
	SetEntryL(aId);
	TMsvId parent = iEntry->Entry().Parent();

	// finish if we've managed to reach the top
	if (parent == KMsvRootIndexEntryId)
		return;

	SetEntryL(parent);

	// finish if we've reached a service
	if (iEntry->Entry().iType == KUidMsvServiceEntry)
		return;

	GetChildrenL(*selection);

	TBool complete=ETrue;
	TBool bodyTextComplete=ETrue;
	TBool partiallyFetched=EFalse;

	TBool related=((TMsvEmailEntry) iEntry->Entry()).MessageFolderType()==EFolderTypeRelated ? 
																					ETrue:EFalse;
	for (TInt i=0; i < selection->Count(); i++)
		{
		SetEntryL((*selection)[i]);
		if (!iEntry->Entry().Complete())
			{
			complete=EFalse;
			if((iEntry->Entry().iType==KUidMsvFolderEntry) && aPartialFetched)
				complete=ETrue;
			// The current part is not complete so...
			// if it is either a text part or a HTML part then the body
			// text is marked as being incomplete.
			//
			// This code means that, if present, then both the text/plain
			// and text/html alternatives need to be downloaded before
			// the body text is marked as being complete.
			if ((iEntry->Entry().iType == KUidMsvEmailTextEntry)
				|| (iEntry->Entry().iType == KUidMsvEmailHtmlEntry ) || related )
				{
				if(aPartialFetched)
					{
					complete = ETrue;
					bodyTextComplete=ETrue;
					}
				else
					bodyTextComplete=EFalse;
				}

			break;
			}
		}
	
	CleanupStack::PopAndDestroy(); // selection

	// if all the siblings were complete then make the parent
	// complete and continue up.
	if (complete || ((aDoBodyText || related) && bodyTextComplete))
		{
		SetEntryL(parent);
		TMsvEmailEntry entry = iEntry->Entry();

		// check whether parent is complete, this wil prevent us
		// checking all the messages in a real folder as they will all
		// be initialised to Complete
		if (!entry.Complete())
			{
			if (complete || ((iEntry->Entry().iType==KUidMsvFolderEntry) && aPartialFetched))
				entry.SetComplete(ETrue);
			if(aPartialFetched)
				{	
				if((iEntry->Entry().iType != KUidMsvAttachmentEntry) &&
						(iEntry->Entry().iType != KUidMsvEmailExternalBodyEntry))
					{
					entry.SetPartialDownloaded(ETrue);
					}
				partiallyFetched = ETrue;
				}
			else
				{
				entry.SetPartialDownloaded(EFalse);
				partiallyFetched = EFalse;
				}
			entry.SetBodyTextComplete(ETrue);
			ChangeEntryL(entry);
		
			PropagateCompleteFlagL(parent, related|aDoBodyText,partiallyFetched);
			}
		else if (entry.PartialDownloaded())
			{
			entry.SetPartialDownloaded(EFalse);
			ChangeEntryL(entry);
			PropagateCompleteFlagL(parent, related|aDoBodyText,partiallyFetched);
			}
		}	
	}

void CImImap4Session::CreateAttachmentInfoL(TMsvEmailEntry& aMsvEmailEntry)
	{
	// create an empty attachment to store the attachment infomation, for the case
	// where the attachment is not downloaded due to download limits.
	CMsvStore* store = iEntry->EditStoreL();
	CleanupStack::PushL(store);

	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
	
	// Check to see if this entry already has an attachment - if so, then don't
	// add it again!
	if( attachmentMgr.AttachmentCount() == 0 )
		{
		MMsvAttachmentManagerSync& attachmentMgrSync = store->AttachmentManagerExtensionsL();
		
		// Now create the attachment entry
		CMsvAttachment* attachment = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
		CleanupStack::PushL(attachment);
						
		// Need to create the MIME-type information - first get the MIME headers
		CImMimeHeader* mimeHeaders = CImMimeHeader::NewLC();
		mimeHeaders->RestoreL(*store);

		HBufC8* buf = HBufC8::NewLC(mimeHeaders->ContentSubType().Length() + mimeHeaders->ContentType().Length() + 1);
		TPtr8 ptr(buf->Des());
		ptr.Copy(mimeHeaders->ContentType());
		ptr.Append(KImcvForwardSlash);
		ptr.Append(mimeHeaders->ContentSubType());
	
		attachment->SetMimeTypeL(ptr);
		
		CleanupStack::PopAndDestroy(2, mimeHeaders);

		attachment->SetComplete(EFalse);
		attachment->SetSize(aMsvEmailEntry.iSize);
		attachment->SetAttachmentNameL(aMsvEmailEntry.iDetails);
		RFile file;
		attachmentMgrSync.CreateAttachmentL(aMsvEmailEntry.iDetails,file,attachment);
		CleanupStack::Pop(attachment); // ownership passed to attachment manager
		file.Close();
		store->CommitL();
		}
	CleanupStack::PopAndDestroy(store);
	}


// Parse fetch messages
TInt CImImap4Session::ProcessFetchL(const TUint aMsgnr, CImapAtom *aAtom)
	{
	CImapAtom *p=aAtom;
	CImapAtom *attribute;
	CImapAtom *structure=NULL;
	CImapAtom *flags=NULL;
	CImapAtom *bodydata=NULL;
	CImapAtom *header=NULL;
	TInt error=KErrNotReady;
	TInt rfc822size=0;
	TBool foundUnwantedMimeHeader=EFalse;
	TBool wholeMessage=EFalse;
	TInt fetchSizeBytes = static_cast<TInt>(iServiceSettings->FetchSize());

	// Fetch data consists of attribute/value pairs
	while(p!=NULL)
		{
		// Get attribute & value
		attribute=p;
		// broken servers can give us just an attribute rather than an
		// attribute pair, if so then just finish the scan here and
		// process what we've got
		p=p->Next();
		if (p==NULL)
			break;

		// Work on attributes
		if (attribute->Compare(KIMAP_UID))
			{
			iFoundUid = ETrue;

			// Lex it ok?
			if (p->Value(iMessageUid)!=KErrNone)
				User::Leave(KErrArgument);

			// Skip to next attribute
			p=p->Next();
			}
	
		else if (attribute->Compare(KIMAP_BODY))
			{
			// some example responses

			// expected without extra header fields
			// * 1 FETCH (UID 1 BODY[2]<0> {1024}

			// expected with extra (empty) header fields
			// * 2 FETCH (UID 35 BODY[1.HEADER.FIELDS ("CONTENT-BASE" "CONTENT-LOCATION")] "" BODY[1]<0> {60}

			// unwanted BODYSTRUCTURE info
			// * 3 FETCH (UID 2 BODY ("text" "plain" ("CHARSET" "us-ascii") NIL NIL "7bit" 4337 67) BODY[1]<0> {1024}

			// unwanted nested BODYSTRUCTURE info
			// * 4 FETCH (UID 1 BODY (("text" "plain" ("CHARSET" "us-ascii") NIL NIL "7bit" 1346 53)
			// ("text" "plain" ("NAME" "install.ins") NIL NIL "7bit" 5156 215) "MIXED")
			// BODY[2]<0> {1024}

			// unwanted BODYSTRUCTURE info and complete message
			// * 5 FETCH (UID 2 BODY ("text" "plain" ("CHARSET" "us-ascii") NIL NIL "7bit" 4337 67) BODY[1] {1550}

			// Body part is across & down one (it's in [])
			CImapAtom* bodypart=p->ToChildL();

			if (bodypart->CompareTail(KIMAP_HEADERFIELDS))
				{
				// Got BODY[HEADER.FIELDS ("header1" "header2" ...)] {LEN} data ...
				// or  BODY[<part>.HEADER.FIELDS ("header1" "header2" ...)] {LEN} data ...
				header=p->ToNextL();
#ifdef PRINTING
				TPtrC8 bpt=bodypart->Next()->Child()->Atom();
				LogText(_L8("Found header fields (%S...)"),&bpt);
#endif
				// Skip to next attribute
				p=header->Next();
				}
			else if (bodypart->CompareTail(KIMAP_MIME))
				{
				// Got BODY[MIME] {LEN} data ...
				// or  BODY[<part>.MIME] {LEN} data ...
				header=p->ToNextL();

        		DBG((LogText(_L8("Found MIME header fields"))));
        		
       				// Skip to next attribute
				p=header->Next();
				}
			else if (bodypart->Child() != NULL || bodypart->Next() != NULL)
				{
				// we've been unexpectedly returned a BODY () response
				// which we don't want, so just skip it
				DBG((LogText(_L8("Unexpected BODY response, ignoring"))));
				p=p->Next();
				}
			else
				{
				// is body data, ie BODY[part]<offset> or BODY[part]

				// Offset is next atom
				CImapAtom* offset=p->ToNextL();

				// there may not be an offset in which case offset is
				// actually bodydata
				
				// Get the offset
				TUint offsetn=0;
				// see if this is an offset
				if( offset->Child() != NULL )
					{
					TLex8 lex(offset->Child()->Atom());				
					if( lex.Val(offsetn)!=KErrNone ) 
						{
						error=KErrGeneral;
						break;
						}

					// Body data is next atom
					bodydata=offset->Next();
					}
				else
					{
					// if not an offset then this is not a partial
					// message
					bodydata=offset;
					wholeMessage=ETrue;
					}
								
				// Additional code added to address additional unrequested non-RFC data sent by the 
				// imap server caused problems in downloading the email and any attachments.
				// Upto this point the BODY tag has correctly been interpreted.
				// 
				// We may however have an unwanted MIME header data which we will need to process or get rid of.					
				// If we have  an unwanted mime header then we currently have the bodydata atom storing:
				//
				// BODY[x.MIME]<offset> 
				// MIME_HEADER_INFO
				// BODY_DATA
				//
				// or
				//
				// BODY.PEEK[x.MIME]<offset>
				// BODY_DATA
				// 
				// The problem here is that the BODY or BODY.PEEK tag will be read as the actual body data.
				// We need to move the bodydata pointer to the beginning of the MIME_HEADER_INFO.
				// When decoding the information we will send the data after the header info to be decoded.
				
				CImapAtom* nextbodypart = NULL;
				CImapAtom* mimetest =NULL;
				
				// Check if the unwanted body tag is "BODY" or "BODY.PEEK"
				if((bodydata->Compare(KIMAP_BODY)) || (bodydata->Compare(KIMAP_BODYPEEK))) //we have an extra body tag
					{
					//find out if it is MIME
					nextbodypart=bodydata->Next();
					if(nextbodypart)
						{
						mimetest = nextbodypart->Child();
						if(mimetest)
							{
							if (mimetest->CompareTail(KIMAP_MIME)) 
								{
								
								// Unrequested tag is "BODY.PEEK"														
								if((bodydata->Compare(KIMAP_BODYPEEK)))
									{
									LogText(_L8("UNEXPECTED RESPONSE: TAG \"BODY.PEEK\"- Found unwanted additional \"BODY.PEEK\" tag in the FETCH response"));
									LogText(_L8("PROCESSING UNEXPECTED RESPONSE: TAG \"BODY.PEEK\" - Additional MIME header data NOT expected to PREFIX the bodypart - unlike additional \"BODY\" tag"));
									// Not expecting additional / unwanted mime header info at the beginning of the body of the message part
									// Hence, no additional processing required on body data
									foundUnwantedMimeHeader=EFalse;
									}
								else // Unrequested tag is "BODY"
									{
									LogText(_L8("UNEXPECTED RESPONSE: TAG \"BODY\" Found unwanted additional \"BODY\" tag in the FETCH response"));
									LogText(_L8("PROCESSING UNEXPECTED RESPONSE: TAG \"BODY\"- Expecting UNWANTED MIME HEADER DATA prefixing the BODY DATA of the message part"));
									// Expecting additional / unwanted mime header info at the beginning of the body data of the message part.
									// Hence, the atom pointed to by bodydata pointer will be parsed / truncated appropriately to extract just 
									// the bodypart later.
									foundUnwantedMimeHeader=ETrue;						
									}
								//we may have an offset that we need to ignore
								CImapAtom* possOffset = nextbodypart->Next();
								TLex8 lex(possOffset->Atom());
								if (lex.Get()=='<') //has an offset
									{
									// Body data is next atom
									bodydata=possOffset->Next();
									}
								else 
									{						
									bodydata=possOffset;
									}
								}
							}							
						}
					}//end of code addressing
				

#ifdef PRINTING
				TPtrC8 bpt=bodypart->Atom();
				LogText(_L8("Found body part [%S] iSizeWait %d"),&bpt,iSizeWait);
#endif
				if (iSizeWait && bodydata!=NULL)
					{
					// No longer waiting for the size
					iSizeWait=EFalse;

					// Size of this part
					TUint sizen=bodydata->Atom().Length();

					DBG((LogText(_L8("  offset=%d, length=%d"),offsetn,sizen)));
					TInt fetchSize = fetchSizeBytes;
					
					// In CC:Mail workaround mode?
					if (iTalkingToCCMail || iTalkingToOpenMail)
						{
						// How much message is there left to fetch?
						TInt sizeleft=iSizeOfThisPart-(offsetn+sizen);
							
						if (sizeleft>0)
							{
							if( iState != EImapStateFetchCancelWait )
								{
								// Limit chunk size
								if(iFetchPartialMail)
									{
									fetchSize = GetFetchSizeL(sizeleft,offsetn+sizen);
									if(fetchSize > fetchSizeBytes )
										{
										fetchSize = fetchSizeBytes;
										}
									}
								else
									{
									if (sizeleft>fetchSizeBytes)
										{
										fetchSize=fetchSizeBytes;
										}
									}	
								// Issue new fetch command
								NewTag();
								TPtrC8 bp(bodypart->Atom());
								if (iServiceSettings->UpdatingSeenFlags())
									{
									iImapIO->SendL(iStatus,KImapFetchBodyPeek,
												iTag,iMessageFetching,&bp,(offsetn+sizen),sizeleft);
									}
								else
									{
									iImapIO->SendL(iStatus,KImapFetchBody,
												iTag,iMessageFetching,&bp,(offsetn+sizen),sizeleft);
									}

								NewTagSent();

								// Get the rest of this line uninterrupted
								error=KErrWrite;
								}
							}
						else
							{
							// Got the whole message
							TInt sizeleft=iSizeOfThisPart-(offsetn+sizen);
							if(iFetchPartialMail && (sizeleft || !iHtmlEntryPart))
								{
								ProcessFooterMessageL(sizeleft);
								}
							}
						}
					else
						{
						// Anything more to get? We decide this on wether we got near to
						// our requested packet size on the last fetch: if we were within
						// 100 bytes of the requested size, we ask for another load just
						// in case the server is serving us line by line. Otherwise, we
						// assume that was the end of the data and flush it out.
						
						// Check whether we have downloaded the message completely or not,
						// before sending the FETCH command again.
						iSizeLeftToFetch = iSizeOfThisPart-(offsetn+sizen);
						if ((fetchSizeBytes-sizen)<100 && iSizeLeftToFetch>0)
							{
							if( iState != EImapStateFetchCancelWait )
								{
								TInt sizeleft=iSizeOfThisPart-(offsetn+sizen);
								fetchSize=sizeleft;
								if(iFetchPartialMail)
									{
									fetchSize = GetFetchSizeL(sizeleft,offsetn+sizen);
									}
								if(fetchSize > fetchSizeBytes)
									{
									fetchSize = fetchSizeBytes;
									}
								// Yes, issue a new fetch command
								NewTag();
								TPtrC8 bp(bodypart->Atom());
								if (iServiceSettings->UpdatingSeenFlags())
									{
									iImapIO->SendL(iStatus,KImapFetchBodyPeek,
											   iTag,iMessageFetching,&bp,(offsetn+sizen),fetchSize);
									
									}
								else
									{
									iImapIO->SendL(iStatus,KImapFetchBody,
											   iTag,iMessageFetching,&bp,(offsetn+sizen),fetchSize);
									}
								NewTagSent();

								// Get the rest of this line uninterrupted
								error=KErrWrite;
								}
							}
						else
							{
							// Got the whole message
							TInt sizeleft = iSizeOfThisPart-(offsetn+sizen);
							
							if(iFetchPartialMail && (sizeleft || !iHtmlEntryPart))
								{
								ProcessFooterMessageL(sizeleft);
								}
							}
						}
					}

				// Finish processing here if we didn't get everything
				if (!iGotWholeLine)
					break;	

				// Skip to next attribute
				p=bodydata->Next();
				}
			}
		else if (attribute->Compare(KIMAP_BODYSTRUCTURE))
			{
			// Body structure: Save it until later when we have created
			// the message - then we can create the attachment tree
			// underneath it
			structure=p->ToChildL();

			// Skip to next attribute
			p=p->Next();
			}
		else if (attribute->Compare(KIMAP_FLAGS))
			{
			// Process flag list later
			flags=p;

			// Skip to next attribute
			p=p->Next();
			}
		else if (attribute->Compare(KIMAP_RFC822SIZE))
			{
			// Save total message size
			if (p->Value(rfc822size)!=KErrNone)
				User::Leave(KErrGeneral);

			// Skip to next attribute
			p=p->Next();
			}

		else
			{
#ifdef PRINTING
			TPtrC8 att=attribute->Atom();
			LogText(_L8("Unknown attribute '%S'"),&att);
			//showtree(attribute,0);
#endif
			}
		}

	DBG((LogText(_L8("About to process, error=%d, iGotWholeLine=%d, iSyncState=%d, bodydata=%x, wholeMessage=%d"),
			error,iGotWholeLine,iSyncState,(int)bodydata,wholeMessage)));

		// No error?
	if ((error==KErrNotReady || error==KErrWrite) && iGotWholeLine)
		{
		// check to see if uid is present in server message. If not present, we are getting a
		// message flag update
		if(!iFoundUid)
			{
#ifdef PRINTING
			DBG((LogText(_L8("UID not present in Fetch, so process flags"))));
#endif

			// this is just a message flag update	

			TInt msgnr = aMsgnr;

			// if the aMsgnr index into the local message array points to a valid message entry
			if((msgnr < iFolderIndex.Size()) && (iFolderIndex[aMsgnr-1].iMsvId != 0))
				{

#ifdef PRINTING			
				for(TInt i=0; i<iFolderIndex.Size(); i++)
					{
					DBG((LogText(_L8("iFolderIndex[%d].iMsvId=%d"), i, iFolderIndex[i].iMsvId)));	
					}
#endif
				// Mirror flags and ensure that the message is visible
				// as it might have been made invisible by
				// unsubscribing.

				// set the current entry to be the message pointed to by aMsgnr
				SetEntryL(iFolderIndex[aMsgnr - 1].iMsvId);

#ifdef PRINTING	
				DBG((LogText(_L8("SetEntry for aMsgnr: %d with msvid %d"), aMsgnr-1, iFolderIndex[aMsgnr-1].iMsvId)));
#endif

				TMsvEmailEntry message=iEntry->Entry();

				// since there is no uid associated with this server response, we just need to update flags
				if (ProcessFlagsL(flags,message)|| !message.Visible())
					{
					message.SetVisible(ETrue);
					ChangeEntryL(message);
					}

#ifdef PRINTING			
				DBG((LogText(_L8("check for deleted imap 4 flags"))));
#endif

				if (message.DeletedIMAP4Flag())
					{
					iRemoteMessagesDeleteTagged++;
					}
				}

			return(KErrNotReady);
			}

		// What synchronisation state are we in?
		switch(iSyncState)
			{
		case ENotSyncing:
			// Ignore it
			break;

		case EFetching:
			{
			if (header)
				{
				ProcessHeaderExtraL(NULL,iAttachmentMimeInfo,NULL,header->Atom());

				// Store CImMimeHeader info
				SetEntryL(iMessageId);
				CMsvStore* entryStore=iEntry->EditStoreL();
				CleanupStack::PushL(entryStore);
				iAttachmentMimeInfo->StoreL(*entryStore);
				entryStore->CommitL();
				CleanupStack::PopAndDestroy(entryStore);
				}

			// Now, decode the body data and store it: completion can't be
			// indicated by 'complete' as we may have been called with a partial line,
			// so we just rely on the same 'early a full buffer' indicator as
			// we do when issuing the pipelined fetches above

			TBool endOfStream = EFalse;
			if (iTalkingToCCMail || iTalkingToOpenMail)
				{
				// As we'll never get the 0 byte terminating read with CC:mail, we have to use
				// our own definition of "end of file", which is simply a packet smaller than
				// the maximum fetch size
				if (bodydata)
					{
					endOfStream=bodydata->Atom().Length()!=fetchSizeBytes;
					DecodeAndStoreL(bodydata->Atom(),endOfStream);
					}
				}
			else
				{
				// Here, we'll treat anything 100 or more bytes shy of the maximum packet size as
				// end of file. If we're closer than that, a new read will have been issued which will
				// return 0 bytes, which *will* cause the EOF to be signalled.
				if (bodydata)
					{
					endOfStream=wholeMessage || (!((fetchSizeBytes-bodydata->Atom().Length())<100) || (iSizeLeftToFetch == 0));
					
					// depending on whether or not we found an unwanted mime header we need to get rid of it.
					// This is done in response to the imap server issues where the server was sending additional information.
					// At this point we know how big the extra header is and we can calculate the correct size of the bodydata by subtracting the 
					// size of the header. 
					if(foundUnwantedMimeHeader)
						{
						foundUnwantedMimeHeader=EFalse;
						//calculate size of actual bodydata without the additional header
						TInt length = bodydata->Atom().Length() - header->Atom().Length();
						//call decode and store, but only pass in the data we are interested in i.e.: skip the additional header.
						DecodeAndStoreL(bodydata->Atom().Right(length),endOfStream);
						}
					else //bodydata only points to the data
						{
						DecodeAndStoreL(bodydata->Atom(),endOfStream);
						}
					}
				}

			// Update flags on message: it should have been marked as read if it wasn't
			// already
			if (flags || endOfStream && ( iState != EImapStateFetchCancelWait ))
				{
				// SJM 19990922: Previously this only set the Unread
				// flag in a rather inefficient way. Change to use new
				// return value of ProcessFlags
				SetEntryL(iMessageId);
				TMsvEmailEntry message=iEntry->Entry();

				TBool hasBodyText = message.iType == KUidMsvEmailTextEntry || message.iType == KUidMsvEmailHtmlEntry;
				TBool partiallyDownloaded = EFalse;
				if (endOfStream)
					{
					message.SetComplete(ETrue);
					if(iFetchPartialMail && iBodyPartRemainingSize && message.iType == KUidMsvEmailTextEntry) 
						{
						message.SetPartialDownloaded(ETrue);
						partiallyDownloaded = ETrue;
						}
					else
						{
						message.SetPartialDownloaded(EFalse);
						}

					if (hasBodyText)
						message.SetBodyTextComplete(ETrue);
					}
				
				// Process flags in this fetch response
				TBool changed = EFalse;
				if (flags)
					changed = ProcessFlagsL(flags,message);

				if (changed || endOfStream)
					ChangeEntryBulkL(message);

				if (endOfStream)
					{
					//iMessagePartsFetchOK++;
					PropagateCompleteFlagL(iMessageId, hasBodyText, partiallyDownloaded);
					}
				}
			break;
			}

		case ESyncListNew:
		case ESyncOld:
			// Got a message's details
			
			// check folder position is not out of bounds and that we have not run out of the local index.
			if (iFolderPosition >= iFolderIndex.Size())
				{
				// All done/ array was out of bounds
				DBG((LogText(_L8("ERROR - Position %d was out of bounds for the FolderIndex arrays size (%d)"),iFolderPosition,iFolderIndex.Size())));
				iSyncState = ENotSyncing;
				break;
				}

			// Collecting UIDs of messages in the folder
			DBG((LogText(_L8("At pos %d, expecting UID %u, got UID %u"),
							 iFolderPosition,iFolderIndex[iFolderPosition].iUid,iMessageUid)));

			// Messages deleted from remote mailbox?
			while(iFolderPosition<iFolderIndex.Size() &&
				  iMessageUid>iFolderIndex[iFolderPosition].iUid)
				{
				// Orphan this message
				DBG((LogText(_L8("Orphaning UID %u"),iFolderIndex[iFolderPosition].iUid)));

				if (iFolderIndex[iFolderPosition].iUid != KIllegalUID)
					{
					// Do it
					OrphanMessageL(iFolderIndex[iFolderPosition].iMsvId);
					}  
				else
					{
					DBG((LogText(_L8("Illegal UID, do not delete."))));
					}

				// Remove it from the index
				iFolderIndex.Expunge(iFolderPosition+1);

				// Increment stats
				iOrphanedMessages++;
				}

			// Run out of local index?
			if (iFolderPosition>=iFolderIndex.Size())
				{
				// All done
				iSyncState=ENotSyncing;
				}
			// In sync again?
			else if (iMessageUid==iFolderIndex[iFolderPosition].iUid)
				{
				// Fine, mirror flag information onto local message
				DBG((LogText(_L8("Match UID %u, mirroring flags"),iMessageUid)));

				// Mirror flags and ensure that the message is visible
				// as it might have been made invisible by
				// unsubscribing.
				SetEntryL(iFolderIndex[iFolderPosition].iMsvId);
				TMsvEmailEntry message=iEntry->Entry();
				if (ProcessFlagsL(flags,message)|| !message.Visible())
                    {
                    message.SetVisible(ETrue);
					ChangeEntryBulkL(message);
       				}

				if (message.DeletedIMAP4Flag())
					iRemoteMessagesDeleteTagged++;
				
				// Next message
				iFolderPosition++;

				// Update counters.
				iMsgsDone++;
				iHeadersFetched++;
				}
			else if (iMessageUid<iHighestUid)
				{
				// If we are seeing uids below the oldest mirrored message, then it is likely
				// that the sync limit has changed. The following code captures the range of 
				// "missing" messages.
				if (iMessageUid<iMissingUidLow || iMissingUidLow==0)
					iMissingUidLow=iMessageUid;
				if (iMessageUid>iMissingUidHigh || iMissingUidHigh==0)
					iMissingUidHigh=iMessageUid;
				}
			break;

		case EGettingStructure:
			{
			error = KErrNone;
			User::LeaveIfError(iEntry->SetEntry(iGetPart));

			if (!(iEntry->Entry().Owner()))
				{
				TMsvEmailEntry entry = iEntry->Entry();
				
				//reset flag for new message
				iParsedTime=EFalse;
				//initialise the time-stamp to the current UTC time
				entry.iDate.UniversalTime();

				// Make a CImHeader to populate with the data
				CImHeader *messageheader=CImHeader::NewLC();
				if (header)
					ProcessHeaderExtraL(messageheader,NULL,&entry,header->Atom());

				// Set correct 'remote size' in CImHeader
				messageheader->SetRemoteSize(rfc822size);

				// Create a message store.
				CMsvStore* entryStore=iEntry->EditStoreL();
				CleanupStack::PushL(entryStore);

				// Store the RFC822 header information.
				messageheader->StoreL(*entryStore);
				entryStore->CommitL();
				CleanupStack::PopAndDestroy(entryStore);

				TInt attachments=0;
				TInt relatedAttachments=0;
				TBool isMHTML=EFalse;
				iDecodedSizeOfAllParts = 0;

				// Create the message entry structure under the root message
				BuildTreeL(entry.Id(),structure,_L8(""),entry.Id(),attachments,isMHTML,relatedAttachments);
				if(isMHTML==EFalse)
					attachments+=relatedAttachments;

				// Now that the structure has been created we can set the real message attributes.
				// The MHTML, attachment flags and size were estimated (hopefully correctly) when the envelope was downloaded.
				entry.iSize = iDecodedSizeOfAllParts;
				entry.SetMHTMLEmail(isMHTML);
				entry.SetAttachment(attachments);
				entry.SetICalendar(iIsICalendar);
				entry.SetVCalendar(iIsVCalendar);

				/* If IDLE is enabled for the account , a new session is not created,
				   so iIsICalendar,iIsVCalendar are not initialised to EFalse 
				   when we fetch a new message, so do it HERE */

				iIsICalendar = EFalse;
				iIsVCalendar = EFalse;

				User::LeaveIfError(iEntry->SetEntry(entry.Id()));
				User::LeaveIfError(iEntry->ChangeEntryBulk(entry));
				CleanupStack::PopAndDestroy(messageheader);
				}

			if (ImapIdleSupported()==EFalse)
				{
				CancelDummy();
				}

			DoFetchL();
			iJustSentFetch = ETrue;
			}
			break;

		case ESyncNew:
			{
			// Got a message's details

			// First, let's check we asked for it: for example, the UW server
			// gives us messages not in the correct range!
			if (iMessageUid<=iHighestUid) 
				{
				DBG((LogText(_L8("Searching local messages for %d"),iMessageUid)));
				while (iFolderPosition<iFolderIndex.Size())
					{
					if (iMessageUid==iFolderIndex[iFolderPosition].iUid)
						{
						DBG((LogText(_L8("Got duplicate UID %d - ignoring"),iMessageUid)));
						return(error);
						}
					else if (iFolderIndex[iFolderPosition].iUid>iMessageUid || iFolderIndex[iFolderPosition].iMsvId==-1)
						break;
					iFolderPosition++;
					}
				}

			// Update counters.
			iMsgsDone++;
			iHeadersFetched++;

			// Creating messages in current folder: create this one
			SetEntryL(iMailboxId);

			// Check to see we have at least the minimum free disk space available
			if (--iCheckDiskSpaceCounter <= 0)
				{
				// If we are running low on disk space then leave
				ImCheckDiskSpace::LeaveIfLowDiskL(iFs, iCurrentDrive);
				iCheckDiskSpaceCounter = KCheckDiskSpaceEveryNMessages;
				}
	
			TMsvEmailEntry message;
			message.iType=KUidMsvMessageEntry;
			message.iMtm=KUidMsgTypeIMAP4;
			message.iServiceId=iServiceId;
			message.SetUID(iMessageUid);
			message.SetValidUID(ETrue);
			message.SetComplete(EFalse);
			message.SetUnread(ETrue);
			
			//reset flag for new message
			iParsedTime=EFalse;
			//initialise the time-stamp to the current UTC time
			message.iDate.UniversalTime();

			// Process message flags
			ProcessFlagsL(flags,message);

			if (message.DeletedIMAP4Flag())
				iRemoteMessagesDeleteTagged++;
				
			// Size of root message entry gets twiddled later
			message.iSize=0;

			// Set new flag
			message.SetNew(ETrue);

			// initialise the send state since the constructor sets it
			// to StateUnknown
			message.SetSendingState(KMsvSendStateNotApplicable);
			
			// Make a CImHeader to populate with the data
			CImHeader *messageheader=CImHeader::NewLC();

			if (header)
				{
				ProcessHeaderExtraL(messageheader,NULL,&message,header->Atom());
				}

			// Set correct 'remote size' in CImHeader
			messageheader->SetRemoteSize(rfc822size);

			// Save message size & attachment flag
  			SetMessageFlagsL(message, structure);

			// Create message
			User::LeaveIfError(iEntry->CreateEntryBulk(message));
			SetEntryL(iMessageId=message.Id());


#if SET_RELATED_ID
			// DS - Set message's iRelatedId to messageId to allow later UI kludges
			message.iRelatedId=iMessageId;
#endif
			ChangeEntryBulkL(message);
			CleanupStack::PopAndDestroy(messageheader);
			break;
			}
		default:
			// ESyncListNew and ESyncSearch should not result in a FETCH reply.
			__ASSERT_DEBUG(EFalse,gPanic(EUnknownState));
			break;
			}
		}

	return(error);
	}

// code originally from void CImRecvConvert::ParseRecipientListL(...)
void CImImap4Session::ProcessAddressListL(CDesCArray& aWhere, HBufC8** aAddresses)
 	{
	TInt length((*aAddresses)->Length());
 	HBufC8* pBuf=HBufC8::NewLC(length);
 	TPtrC8 source((*aAddresses)->Ptr(), length);
 	const TUint8* ptr(source.Ptr());
 	const TUint8* lastCharPtr(ptr + source.Length() - 1);
 	TUint8 lookFor(0);
 	TInt count(0);
 	TBool finishedEntry(EFalse);
 
 	// get past white space
 	while(*ptr&&((*ptr==KImcvSP)||(*ptr==KImcvSemiColon))) ptr++;
 
 	// Entries are separated by commas or semicolons.
 	// Separators do not count if they appear within
 	// "", <>, () or embedded series of these, eg "(one, two)"
 	// so we need to keep track of these, including nesting.
 	while(*ptr && ptr <= lastCharPtr)
 		{
 		if(pBuf->Length()==0)
 			{
 			finishedEntry = EFalse;
 			}
 
 		switch(*ptr)
 			{
 			case KImcvLeftBracket:
 				if(lookFor==KImcvRightBracket)
 					{ // We've already had a "(", so now we need another one
 					count++;
 					}
 				else if(lookFor==0)
 					{ //We weren't looking for anything else, now we need to
 					lookFor = KImcvRightBracket;
 					count = 1;
 					}
 				// else we were already looking for something else, ignore this
 				break;
 			case KImcvLeftChevron:
 				if(lookFor==KImcvRightChevron)
 					{ //We've already had a "<", so now we need another one
 					count++;
 					}
 				else if(lookFor==0)
 					{ //We weren't looking for anything else
 					lookFor = KImcvRightChevron;
 					count = 1;
 					}
 				// else we were already looking for something else, ignore this
 				break;
 			case KImcvDoubleQuote:
 				if(lookFor==KImcvDoubleQuote)
 					{ // We already had a quote, so this matches it
 					lookFor = 0;
 					}
 				else if(lookFor==0)
 					{ //We weren't looking for anything else
 					lookFor = KImcvDoubleQuote;
 					}
 				// else we were already looking for something else, ignore this
 				break;
 			case KImcvRightBracket:
 			case KImcvRightChevron:
 				if(*ptr == lookFor)
 					{ //If we have found what we were looking for, decrease the count
 					count--;
 					if(count==0)
 						{ // Got everything, now we're not looking for anything
 						lookFor = 0;
 						}
 					// else keep looking for the same thing	again
 					}
 				// else we're looking for something else, ignore it
 				break;
 			case KImcvComma:
 			case KImcvSemiColon:
 				// If we're not looking for anything, we're finished
 				if (lookFor == 0)
 					finishedEntry = ETrue;
 				// else this comma or semicolon is part of a different token, ignore it
 				break;
 			}
 
 		if(!finishedEntry)
 			{
 			pBuf->Des().Append((TChar)*ptr);
 			// move to the next character
 			ptr++;
 			}
 		else
 			{
 			// that's it! store the address away
#ifdef UNICODE
 			HBufC16* pBuf16 = HBufC16::NewLC(pBuf->Des().Length());
 			pBuf16->Des().Copy(pBuf->Des());
 			aWhere.AppendL( (HBufC16&) *pBuf16 );
 			CleanupStack::PopAndDestroy(pBuf16); // pBuf16
#else
			aWhere.AppendL( *pBuf );
#endif			
 			pBuf->Des().SetLength(0);
 			finishedEntry = EFalse; //Ready for next entry
 
 			// get past the separator
 			ptr++;
 
 			// get past white space (& any other separators)
 			while(*ptr && (*ptr==KImcvSP || *ptr==KImcvTab || *ptr==KImcvComma || *ptr==KImcvSemiColon)) ptr++;
 			}
 		}
 		// catch the last name in the list
 		if (pBuf)
 			{
 			TInt recipientLength(pBuf->Length());
 			if (recipientLength > 0)
 				{
#ifdef UNICODE
 				HBufC16* pBuf16 = HBufC16::NewLC(recipientLength);
 				pBuf16->Des().Copy(*pBuf);
 				aWhere.AppendL(*pBuf16);
 				CleanupStack::PopAndDestroy(pBuf16); // pBuf16
#else
 				aWhere.AppendL( *pBuf );
#endif
 				}
 			}
 		CleanupStack::PopAndDestroy(pBuf); // pBuf
 	}

void CImImap4Session::ProcessFooterMessageL(TInt aSizeLeft)
	{
	TUid type = iEntry->Entry().iType;	
	if (type == KUidMsvEmailTextEntry)
		{
		if(iHtmlEntrySize)
			{
			iBodyPartRemainingSize = aSizeLeft + iHtmlEntrySize;
			}
		else
			{
			iBodyPartRemainingSize = aSizeLeft;
			}
		// Message has both text and html and if sizeleft = 0 ,
		// then there could be only html part left on server
		// Message has only plain text, then footer message not required if sizeleft = 0
		if(iBodyPartRemainingSize)
			AttachFooterInfoL();
		}
	}

void CImImap4Session::AttachFooterInfoL()
	{
	DBG((LogText(_L8("AttachFooterInfoL(): Footer Sting for this partially downloaded message"))));
	RResourceFile resFile;
	OpenResourceFileL(resFile,iFs);	// NB leaves if file not found
	const TInt KNoOfDigitsInMailSize = 10; // maximum length for no of digits for remaining body parts size
	_LIT(KIntegerDirective,"%d");
	// make sure the resource file will be closed if anything goes wrong
	// CloseResourceFile is declared in IMCMMAIN.H and defined in IMCMMAIN.CPP
	TCleanupItem close(CloseResourceFile,&resFile);
	CleanupStack::PushL(close);
		
	// Read the string for remaining mail size for footer
	HBufC8* buf = NULL;
	buf = resFile.AllocReadLC( PARTIAL_DOWNLOAD_FOOTER_MESSAGE );
	TResourceReader reader;
	reader.SetBuffer(buf);
	// Check if %d is not found in the resource string (To avoid problems due to localisation)
	if(buf->Find((TDesC8&)KIntegerDirective) == KErrNotFound)
		{
		iFooterString = (reader.ReadTPtrC()).AllocL();
		}
	else
		{
		HBufC* resourceBuf = (reader.ReadTPtrC()).AllocL();
		iFooterString = HBufC::NewL(resourceBuf->Length()+KNoOfDigitsInMailSize);
		iFooterString->Des().Format(*resourceBuf,(iBodyPartRemainingSize / KKiloByteSize));
		delete resourceBuf;
		}
	CleanupStack::PopAndDestroy(2); // buf, resFile (Close resfile)
	}

// Set the following attributes of the given message entry for the IMAP information contained in atom tree:
// iSize
// 
void CImImap4Session::SetMessageFlagsL(TMsvEmailEntry& aMessageEntry, CImapAtom* aRootAtom)
	{
	CArrayFixFlat<CImapAtom*>* atomStack = new (ELeave) CArrayFixFlat<CImapAtom*>(10);
	CleanupStack::PushL(atomStack);
	atomStack->AppendL(aRootAtom);

	TBool hasAttachments = EFalse;
	TBool hasHtml = EFalse;
	TBool possibleHtml = EFalse;
	TBool afterRelated = EFalse;
	TBool afterAlternative = EFalse;
	TBool htmlAfterAltRel = EFalse;
	TBool hasICalendar = EFalse;
	TBool hasVCalendar = EFalse;
	TInt size = 0;

	CImapAtom* currentAtom;

	TBool doneAllSiblings;
	TBool base64;
	TInt index;
	TBool numeric;
	TInt atomValue;
	TBool foundSize;
	
	// the root atom is of these types, the message body is an attachment
	if(aRootAtom->Compare(KMIME_IMAGE) || 
		aRootAtom->Compare(KMIME_AUDIO) || 
		aRootAtom->Compare(KMIME_APPLICATION)||
		aRootAtom->Compare(KMIME_VIDEO))
		{
		hasAttachments = ETrue;
		}
	
	while (atomStack->Count() != 0)
   		{
		// Pop the top atom off of the stack
		currentAtom = (*atomStack)[atomStack->Count() - 1];
 		atomStack->ResizeL(atomStack->Count() - 1);
		base64 = EFalse;
 
		// Run through all the sibling atoms unless this atom is a message/rfc822
		if( currentAtom->Compare(KMIME_MESSAGE) && currentAtom->Next()->Compare(KMIME_RFC822) )
			doneAllSiblings = ETrue;
		else
			doneAllSiblings = EFalse;
		foundSize = EFalse;
		possibleHtml = EFalse;
		TInt siblingIndex = 0;
		while (!doneAllSiblings)
			{
			// If a previous Sibling Atom has a HTML tag, then this could possibly 
			// be an HTML message.  So we need to check all Sibling Atoms (ie the 
			// current atom) to see if they contain Attachments (ie check their 
			// child atoms for the Tag "ATTACHMENT"). If an attachment is found 
			// then ignore the HTML flag. If none of the siblings contain an 
			// attachment, then set the HTML flag, as this is a MHTML message.
			if (possibleHtml)
				{
				// Check if the Child has an Attachment
				if (currentAtom->Child() != NULL)
					{
					if (currentAtom->Child()->Compare(KMIME_ATTACHMENT))
						{
						// This Sibling contains an Attachment, so ignore the HTML flag.
						possibleHtml = EFalse;
						}
					}

				// Check if we have searched all Sibling Atoms
				if (possibleHtml && currentAtom->Next() == NULL)
					{
					// None of the Siblings have attachments, so set the HTML flag.
					hasHtml = ETrue;
					possibleHtml = EFalse;
					}
					
				}

			// If there is a child atom then add it to the stack, we will check it later
			if (currentAtom->Child() != NULL)
				atomStack->AppendL(currentAtom->Child());

			// If we pass a related atom then we may have an html message
			// remember for later
			if (currentAtom->Compare(KMIME_RELATED))
				afterRelated = ETrue;

			// If we pass an alternative atom then we may have an html message
			// remember for later
			if (currentAtom->Compare(KMIME_ALTERNATIVE))
				afterAlternative = ETrue;

			// If we find an html under a related or alternative then we
			// can ignore the mixed section and assume that we have an html mail
			if (afterAlternative || afterRelated)
				{
				if (currentAtom->Compare(KMIME_HTML))
					htmlAfterAltRel = ETrue;
				}

			// MIXED ? If so then this email probably contains attachments
			// or if there is a message/delivery-status then that is an
			// attachment. 
			if (currentAtom->Compare(KMIME_MIXED) || currentAtom->Compare(KMIME_DELIVERY_STATUS))
				{
				hasAttachments = ETrue;
				}
		
			// HTML ?  If so this email could be an HTML message.  To make sure 
			// we need to check this Atom's siblings
			if (currentAtom->Compare(KMIME_HTML))
				possibleHtml = ETrue;		

			// Does this sibling atom say that the data is base64 encoded ?
			// If so then we need to remember it to calculate the size.
			if (currentAtom->Compare(KMIME_BASE64))
				base64 = ETrue;
			
			if (currentAtom->Compare(KMIME_ICALENDAR))
				hasICalendar = ETrue;
			
			if (currentAtom->Compare(KMIME_VCALENDAR))
				hasVCalendar = ETrue;

			// If this is the first numeric value of the current siblings then it is a size
			// and must be added to the message size total
			// Note that this size must be multiplied by 3/4 if it is base64
			if (!foundSize && siblingIndex == 6)
				{
				index = currentAtom->Atom().Length();
				
				if (index != 0)
					numeric = ETrue;
				else
					// If the atom is of 0 length then it can't possibly be numeric.
					numeric = EFalse;

				while ((index--) && (numeric))
					{
					if ((currentAtom->Atom()[index] < '0')
						|| (currentAtom->Atom()[index] > '9'))
						numeric = EFalse;
					}

				if (numeric)
					{
					TLex8 lex(currentAtom->Atom());
					User::LeaveIfError(lex.Val(atomValue));
					if (base64)
						atomValue = (atomValue * 3) / 4;
					size += atomValue;
					foundSize = ETrue;
					}
				}

			siblingIndex++;
			currentAtom = currentAtom->Next();
			if (currentAtom == NULL)
				doneAllSiblings = ETrue;
			}
   		}

	// Set the size
	aMessageEntry.iSize = size;

	// Set the Attachment, MHTML, ICalendar and VCalendar flags, if required.
	if (hasAttachments)
		{
		aMessageEntry.SetAttachment(ETrue);
		}

	if( hasHtml || htmlAfterAltRel )
		{
		aMessageEntry.SetMHTMLEmail(ETrue);
		}

	if(hasICalendar)
		{
		aMessageEntry.SetICalendar(ETrue);
		}
		
	if(hasVCalendar)
		{
		aMessageEntry.SetVCalendar(ETrue);
		}
		
	CleanupStack::PopAndDestroy(atomStack);
   	}

TInt32 CImImap4Session::GetFetchSizeL(TInt32 aSizeLeft, TInt32 aSizeDownLoaded)
{
	TInt fetchSizeBytes = static_cast<TInt>(iServiceSettings->FetchSize());	
	TInt32 minimumLimit = fetchSizeBytes;
	TInt32 fetchSize = fetchSizeBytes;
	TUid type = iEntry->Entry().iType;

	if(iGetPartialMailInfo.iPartialMailOptions == ENoSizeLimits)
		{
		return KMaxTInt;
		}
	
	if (type == KUidMsvEmailTextEntry  || type == KUidMsvEmailHtmlEntry)
		{
		if(type == KUidMsvEmailHtmlEntry)
			{
//			iHtmlEntrySize = iSizeOfThisPart;
			minimumLimit = Minimum(iGetPartialMailInfo.iBodyTextSizeLimit,iGetPartialMailInfo.iTotalSizeLimit-iBodyTextSize);
			}
		else
			{
			// store body text size so that we can check whether html part for 
			// this message can be downloaded 
			// text size + html size < iBodyTextSizeLimit then download html part
			iBodyTextSize = iSizeOfThisPart;
			minimumLimit = Minimum(iGetPartialMailInfo.iBodyTextSizeLimit,iGetPartialMailInfo.iTotalSizeLimit);
			}
		// check disk space
		if(!iIsDiskSpaceChecked)
			{
			CheckForDiskSpaceL(minimumLimit);
			iIsDiskSpaceChecked = ETrue;
			}

		fetchSize = FetchSize(minimumLimit,aSizeDownLoaded,aSizeLeft);
		}
	else if (type == KUidMsvAttachmentEntry || type == KUidMsvEmailExternalBodyEntry)
		{
		minimumLimit = Minimum(iGetPartialMailInfo.iAttachmentSizeLimit,
						iGetPartialMailInfo.iTotalSizeLimit-(iBodyTextSize+iHtmlEntrySize));

		if(!iIsDiskSpaceChecked)
			{
			CheckForDiskSpaceL(minimumLimit);
			iIsDiskSpaceChecked = ETrue;
			}

		fetchSize = FetchSize(minimumLimit,aSizeDownLoaded,aSizeLeft);
		}
	return fetchSize;
	}

TInt32 CImImap4Session::FetchSize(TInt32 aMinimumLimit,TInt32 aSizeDownLoaded, TInt32 aSizeLeft)
	{
	TInt fetchSizeBytes = static_cast<TInt>(iServiceSettings->FetchSize());	
	TInt32 fetchSize = fetchSizeBytes;
	if(aSizeLeft > (aMinimumLimit-aSizeDownLoaded))
		{
		if((aMinimumLimit-aSizeDownLoaded) < fetchSizeBytes)
			{
			fetchSize = aMinimumLimit-aSizeDownLoaded;
			}
		}
	else
		{
		if(aSizeLeft < fetchSizeBytes)
			{
			fetchSize = aSizeLeft;
			}
		}
	return fetchSize;
	}

// Process an address list structure into a CImHeader
void CImImap4Session::ProcessAddressListL(HBufC8 **aBufferPtr, CDesCArray& aWhere, CImapAtom *aAtom)
	{
	while(aAtom)
		{
		// Process this address and add it to ToRecipients()
		ProcessAddressL(aBufferPtr,aAtom);

#ifdef UNICODE
		HBufC *newaddress=HBufC::NewL((*aBufferPtr)->Length());
		CleanupStack::PushL(newaddress);
		newaddress->Des().Copy((*aBufferPtr)->Des());
		aWhere.AppendL(newaddress->Des());
		CleanupStack::PopAndDestroy();
#else
		aWhere.AppendL((*aBufferPtr)->Des());
#endif

		// Next address
		aAtom=aAtom->Next();
		}
	}

void CImImap4Session::AppendExtendL(HBufC8** aBufferPtr, const TDesC8& aText, TBool aOnStack)
	{
	HBufC8 *buffer = *aBufferPtr;
	TInt32 space = buffer->Des().MaxLength() - (buffer->Des().Length() + aText.Length());
	if (space < 0)
		{
		TInt32 inc = (-space) + KImapAddressSizeInc - ((-space) % KImapAddressSizeInc);
		TInt32 newSize = buffer->Des().MaxLength() + inc;
		HBufC8 *newBuf = buffer->ReAllocL(newSize);

		if (aOnStack && newBuf!=buffer)
			{
			CleanupStack::Pop();
			CleanupStack::PushL(newBuf);
			}
		
		*aBufferPtr = buffer = newBuf;
		}
	buffer->Des().Append(aText);
	}

// Process an address structure into a buffer
void CImImap4Session::ProcessAddressL(HBufC8** aBufferPtr, CImapAtom *aAtom)
	{
	// Erase buffer
	(*aBufferPtr)->Des().Zero();

	// Descend into address
	aAtom=aAtom->ToChildL();

	// Save name
	CImapAtom* name=aAtom;
	aAtom=aAtom->ToNextL();

	// Skip route
	aAtom=aAtom->ToNextL();

	// Save user
	CImapAtom* user=aAtom;
	aAtom=aAtom->ToNextL();

	// Save host
	CImapAtom* host=aAtom;

	// Build address string: is there a name?
	if (name->Compare(_L8("")) || name->Compare(_L8("NIL")))
		{
		// No, just save user@host
		AppendExtendL(aBufferPtr, user->Atom());
		AppendExtendL(aBufferPtr, _L8("@"));
		AppendExtendL(aBufferPtr, host->Atom());
		}
	else
		{
		// Yes, in the form 'Name <user@host>'
		AppendExtendL(aBufferPtr, _L8("\""));
		AppendExtendL(aBufferPtr, name->Atom());
		AppendExtendL(aBufferPtr, _L8("\""));
		AppendExtendL(aBufferPtr, _L8(" <"));
		AppendExtendL(aBufferPtr, user->Atom());
		AppendExtendL(aBufferPtr, _L8("@"));
		AppendExtendL(aBufferPtr, host->Atom());
		AppendExtendL(aBufferPtr, _L8(">"));
		}

#ifdef PRINTING
	TPtrC8 addr=(*aBufferPtr)->Des();
	if (addr.Length() > 256)
		addr.Set(addr.Ptr(), 256);
	LogText(_L8("  address '%S'"),&addr);
#endif
	}

// SJM 19990922. Previous version of this function used SetUnread
// rather than SetIMAP4Unread or SetIMAP4Flags. This meant that the
// IMAP4Unread flag itself was never set. This seems like a bug.

TBool CImImap4Session::ProcessFlagsL(CImapAtom* aAtom, TMsvEmailEntry& aMessage)
	{
	TBool unread = EFalse;
    TBool seen = EFalse;
	TBool answered = EFalse;
	TBool flagged = EFalse;
	TBool deleted = EFalse;
	TBool draft = EFalse;
	TBool recent = EFalse;
	TBool flagsUpdated = EFalse;

	// Descend
	aAtom=aAtom->Child();

	// Process flags
	while(aAtom!=NULL)
		{
		// Check for standard IMAP flags
		if (aAtom->Compare(KIMAPFLAG_ANSWERED))
			answered = ETrue;
		else if (aAtom->Compare(KIMAPFLAG_DELETED))
			deleted = ETrue;
		else if (aAtom->Compare(KIMAPFLAG_DRAFT))
			draft = ETrue;
		else if (aAtom->Compare(KIMAPFLAG_FLAGGED))
			flagged = ETrue;
		else if (aAtom->Compare(KIMAPFLAG_RECENT))
			recent = ETrue;
		else if (aAtom->Compare(KIMAPFLAG_SEEN))
			seen = ETrue;
		else if (aAtom->Compare(KIMAPFLAG_UNREAD))
			{
			unread = ETrue;

			// There is at least one unread message in this folder
			iSomeUnread=ETrue;
			}

		// Next atom
		aAtom=aAtom->Next();
		}


	TBool oUnread, oSeen, oAnswered, oFlagged, oDeleted, oDraft, oRecent;
	aMessage.GetIMAP4Flags(oUnread, oSeen, oAnswered, oFlagged, oDeleted, oDraft, oRecent);

	// Are we configured to update the \seen flag on the server?
	if (iServiceSettings->UpdatingSeenFlags())
		{
		// Make a note to update the servers \Seen flag if CHANGED on the client
		//  and different to the servers version
		if ( FIXBOOL(aMessage.Unread()) == seen && FIXBOOL(oSeen) == seen)
			{
			if (aMessage.Unread())
				iClearSeenList->AppendL(aMessage.Id());
			else
				iSetSeenList->AppendL(aMessage.Id());
			}
		}

	if ( FIXBOOL(oUnread) != unread || FIXBOOL(oSeen) != seen || FIXBOOL(oAnswered) != answered
		 || FIXBOOL(oFlagged) != flagged || FIXBOOL(oDeleted) != deleted
		 || FIXBOOL(oDraft) != draft || FIXBOOL(oRecent) != recent )
		{
		aMessage.SetIMAP4Flags(unread, seen, answered, flagged, deleted, draft, recent);
		flagsUpdated = ETrue;
		}

	// Are we configured to update the \seen flag on the server?
	if (iServiceSettings->UpdatingSeenFlags())
		{
		// Now copy the inverse of the \Seen flag down to the clients Unread flag 
		//  except when LastSyncSeen is set (ie when the client version is more up to date)
		//  This means that the client Read status ALWAYS reflects the IMAP \Seen state
		if ( FIXBOOL(aMessage.Unread()) == seen && FIXBOOL(oSeen) != seen)
			{
			aMessage.SetUnread(!seen);
			flagsUpdated = ETrue;
			}
		}

	return flagsUpdated;
	}

// Process output from list command
void CImImap4Session::ProcessListL(CImapAtom *aAtom)
	{
	// Just getting hierarchy separator?
	if (iState==EImapStateSeparatorWait)
		{
		// Save it away
		aAtom=aAtom->ToNextL();

		// Is it one character long? (all should be) - if not, ignore it
		if (aAtom->Atom().Length()!=1)
			return;

		// Save it for local use
		iHierarchySeparator=aAtom->Atom();

		// Update service entry
  		CEmailAccounts* account = CEmailAccounts::NewLC();
		TImapAccount id;
		id.iImapAccountId = iEntry->Entry().MtmData2();  // iMtmData2 of the service entry contains TImapAccountId
		id.iImapAccountName = iEntry->Entry().iDetails;
		id.iImapService = iEntry->Entry().iServiceId;
		id.iSmtpService = iEntry->Entry().iRelatedId;

  		account->LoadImapSettingsL(id, *iServiceSettings);
  		
  		// Set new path separator
  		iServiceSettings->SetPathSeparator(iHierarchySeparator[0]);
  		id.iImapAccountName = KNullDesC;    // So that account name is not updated
  		account->SaveImapSettingsL(id, *iServiceSettings);
  		CleanupStack::PopAndDestroy(account);    
		return;
		}

	// Anywhere to save it?
	if (!iList) return;

	// Getting hierarchy from server - process the lot
	CImapAtom *fl;
	CImImap4DirStruct *entry=new (ELeave) CImImap4DirStruct;
	CleanupStack::PushL(entry);

	// List reply is of the form: * LIST (flags) "pathsep" "path"

	// At the start, it's a mailbox
	entry->iIsMailbox=ETrue;
	entry->iIsFolder=ETrue;

	// Process flags
	if ((fl=aAtom->Child())!=NULL)
		{
		while(fl)
			{
			// Check flags
			if (fl->Compare(KIMAPFLAG_NOSELECT))
				{
				// \Noselect means it isn't a mailbox
				entry->iIsMailbox=EFalse;
				}
			else if (fl->Compare(KIMAPFLAG_NOINFERIORS))
				{
				// \Noinferiors means it can't be a folder
				entry->iIsFolder=EFalse;
				}

			// Next flag
			fl=fl->Next();
			}
		}

	// Path separator: only save this if it is a valid length
	aAtom=aAtom->ToNextL();
	if (aAtom->Atom().Length()==1)
		iHierarchySeparator=aAtom->Atom();

	// Path
	aAtom=aAtom->ToNextL();
	TPtrC8 path(aAtom->Atom());

	// In case server has returned us the parent when we asked for children,
	// ignore items which are too small to be path(separator)child names.
	if (path.Length()>iCommandBuf.Length())
		{
		// CC:Mail has an annoying habit of not listening to us when we ask for
		// children, and returning siblings instead: this takes some extra CPU
		// time, so we only do it if we're talking to a CC:Mail server, but here
		// we check that the returned path is actually what we asked for before
		// proceding.
		if (iTalkingToCCMail)
			{
			// Check path starts with iCommandBuf's contents
#ifdef UNICODE
			HBufC8 *narrow=HBufC8::NewL(iCommandBuf.Length());
			narrow->Des().Copy(iCommandBuf);
			if (path.Find(*narrow)!=0)
				{
				// Nope. Ignore this. Silly server.
				delete narrow;
				CleanupStack::PopAndDestroy();
				return;
				}
			delete narrow;
#else
			if (path.Find(iCommandBuf)!=0)
				{
				// Nope. Ignore this. Silly server.
				CleanupStack::PopAndDestroy();
				return;
				}
#endif
			}
			
		// Add leaf, ignoring the path that all servers return before
		// the leaf.

		// Removed the UNICODE special case since we might need to
		// modify the buffer now to do the UnModUTF7.
#if 1
		TPtrC8 leaf = path.Mid(iCommandBuf.Length());
		HBufC* text = DoUnModUTF7LC( leaf );
		entry->SetLeafnameL(text->Des());

		CleanupStack::PopAndDestroy();
#else
		HBufC *newleaf=HBufC::NewL(path.Length()-iCommandBuf.Length());
		CleanupStack::PushL(newleaf);
		newleaf->Des().Copy(path.Mid(iCommandBuf.Length()));
		entry->SetLeafnameL(newleaf->Des());
		CleanupStack::PopAndDestroy();
#endif

		// If there's anything there (might just be the path), then add it to array
		if (entry->Leafname().Length()>0)
			{
			// Add to array
			iList->AppendL(entry);

			// It's in the array now
			CleanupStack::Pop();
			}
		else
			{
			// Get rid of it - it's not needed
			CleanupStack::PopAndDestroy();
			}
		}
	else
		{
		// Get rid of it - it's not needed
		CleanupStack::PopAndDestroy();
		}
	}

// Process output from list command
void CImImap4Session::ProcessLsubL(CImapAtom *aAtom)
	{
	// Process the lot: it'll be a full path which we need to cut up
	// Reply is of the form: * LSUB (flags) "pathsep" "path"

	// Skip flags
	aAtom=aAtom->ToNextL();

	// We already know the path separator, so skip it
	aAtom=aAtom->ToNextL();

	// Strip folder path: to do this we just remove the start of the path from
	// the returned string, so we need the length of it.
	TInt pathlength=iFolderPath.Length();

	// If the length is non-zero, this means that there is a folderpath, and
	// we will need to strip both it, and the following path separator character.
	// If there is no path, we don't want to strip anything, so we don't
	// increment it at all.
	if (pathlength)
		pathlength++;

	// Get a (possibly wide) copy of the path, skipping the prefix
	TPtrC8 skippedPrefix(aAtom->Atom().Mid(pathlength));
	HBufC* basePath = DoUnModUTF7LC( skippedPrefix );
	TPtrC path(basePath->Des());
	
	// Go down through path
	TMsvId where=iServiceId;
	while(where)
		{
		// Truncate this bit of path: look for a hierarchy separator
		TInt separator=path.Locate(iHierarchySeparator[0]);
		TPtrC element(path);

		// Anything?
		if (separator!=KErrNotFound)
			{
			// Truncate this element there
			element.Set(element.Ptr(),separator);
			}

		// Try to find this element at the search level
		SetEntryL(where);
		GetChildrenL(*iSelection);

		// Nothing? Give up.
		if (!iSelection->Count())
			break;				// out of while

		// Check all the children
		TInt a;
		for(a=0;a<iSelection->Count();a++)
			{
			// This one?
			SetEntryL((*iSelection)[a]);

			if (iEntry->Entry().iDetails.Compare(element)==0)
				{
				// Found it! Are we at the end, ie no more elements?
				if (separator==KErrNotFound)
					{
					// Set subscribed flag
					TMsvEmailEntry message=iEntry->Entry();
					if (!message.Subscribed())
						{
						// It needs changing, do it
						message.SetSubscribed(ETrue);
						ChangeEntryL(message);
						}

					// All done: this will exit the loop
					where=0;
					break;
					}
				else
					{
					// Update path
					where=(*iSelection)[a];
					path.Set(path.Ptr()+separator+1,path.Length()-separator-1);

					// Exit loop
					break;
					}
				}
			}

		if (a==iSelection->Count())
			{
			// Didn't find it. Humm
			DBG((LogText(_L8("Didn't find entry '%S': tree out of date?"),&element)));

			break;				// out of while
			}
		}

	CleanupStack::PopAndDestroy();
	}

void CImImap4Session::SendLoginL()
	{
	// We need to login in as few steps as possible, ie avoid using
	// literals if possible as this incurrs a RTT delay as we have to
	// wait for server's OK before sending the literal.

	// Set up to send a command
	NewTag();

	// No need to quote? Do it in one line if we can,
	if (!iLiteralUsername && !iLiteralPassword)
		{
		// Send login line. Turn off logging before sending
		iImapIO->PerformLogging(EFalse);
		iImapIO->SendL(iStatus,_L8("%d LOGIN %S %S\r\n"),iTag,iUsername,iPassword);
		iImapIO->PerformLogging(ETrue);
		}
	else
		{
		if (iLiteralUsername)
			{
			// Send literal username
			iImapIO->SendL(iStatus,_L8("%d LOGIN {%d}\r\n"),iTag,iUsername->Length());
			iState=EImapStateLoginSendUser;
			}
		else
			{
			// Send username and literal password. Turn off logging before sending
			iImapIO->PerformLogging(EFalse);
			iImapIO->SendL(iStatus,_L8("%d LOGIN %S {%d}\r\n"),iTag,iUsername,iPassword->Length());
			iImapIO->PerformLogging(ETrue);
			
			iState=EImapStateLoginSendPassword;
			}
		}

	// Don't complete yet!
	NewTagSent();
	}

void CImImap4Session::SendCapabilityL()
	{
	// Check server capabilities
	iState=EImapStateCapabilityWait;
	iSeenVersion=EFalse;

	// Send the command
	NewTag();
	iImapIO->SendL(iStatus,_L8("%d CAPABILITY\r\n"),iTag);
	NewTagSent();
	}

void CImImap4Session::StartIdle(TRequestStatus& aRequestStatus)
	{
        DBG((LogText(_L8("CImImap4Session::StartIdle()"))));
	TInt err=KErrNone;
	if (!Connected() || iState==EImapStateIdleWait)
		{
		Queue(aRequestStatus);
		err=KErrDisconnected;
		}
	else
		TRAP(err,StartIdleL(aRequestStatus));
	if (err!=KErrNone)
                {
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::StartIdle(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		

		Complete(err);
                }
	}

void CImImap4Session::StartIdleL(TRequestStatus& aRequestStatus)
	{
        DBG((LogText(_L8("CImImap4Session::StartIdleL()"))));
	Queue(aRequestStatus);
	DoStartIdleL();
	}

void CImImap4Session::DoStartIdleL()
	{
        DBG((LogText(_L8("CImImap4Session::DoStartIdleL()"))));
 	__ASSERT_DEBUG(ImapIdleSupported(), gPanic(EBadUseOfImap4Op));
 	if(!ImapIdleSupported())
	 	{
	 	User::LeaveIfError(KErrGeneral);//Bad use of Imap4Op
	 	}
 	__ASSERT_DEBUG(iState==EImapStateSelected, gPanic(ESyncWhenNotSelected));
	if(!(iState==EImapStateSelected))
		{
		User::LeaveIfError(KErrArgument);// Sync when not selected .
		}
	// Reset flags 	
	iMailboxReceivedExists=EFalse;
	iMailboxReceivedExpunge=EFalse;
	iMailboxReceivedFlags=EFalse;
	
	iState=EImapStateIdleWait;
	SendMessageL(KIMAPC_IDLE);
	}

void CImImap4Session::StopIdle(TRequestStatus& aRequestStatus)
	{
        DBG((LogText(_L8("CImImap4Session::StopIdle()"))));
	TInt err(KErrNone);
	if (!Connected())
		{
		Queue(aRequestStatus);
		err=KErrDisconnected;
		}
	else
		{
		// stop idle is only called by compound command
		// so flag this 
		iCompoundStopIdle = ETrue;
		TRAP(err,StopIdleL(aRequestStatus));
		}
	if (err!=KErrNone)
		{
                DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::StopIdle(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		Complete(err);
		}
	}

void CImImap4Session::SyncStopIdleL(TRequestStatus& aRequestStatus)
	{
	DBG(LogText(_L8("CImImap4Session::SyncStopIdleL (state=%d)"), iState));
	iStoppingIdleForSync = ETrue;
	StopIdleL(aRequestStatus);
	}

void CImImap4Session::StopIdleL(TRequestStatus& aRequestStatus)
	{
        DBG((LogText(_L8("CImImap4Session::StopIdleL()"))));
	Cancel();
	Queue(aRequestStatus);
	DoStopIdleL();
	}

void CImImap4Session::DoStopIdleL()
	{
	DBG(LogText(_L8("CImImap4Session::DoStopIdleL(state=%d)"),iState)) ;
	__ASSERT_DEBUG(ImapIdleSupported(), gPanic(EBadUseOfImap4Op));
	if(!ImapIdleSupported())
		{
		User::LeaveIfError(KErrGeneral);//Bad Use of Imap4Op
		}
	__ASSERT_DEBUG(IsIdling(), gPanic(EInvalidStatus));
	if(!IsIdling())
		{
		User::LeaveIfError(KErrGeneral); // Invalid Status
		}

	iIdleTimer->Cancel();
	iState=EImapStateStopIdleWait;

	SendUntaggedMessageWithTimeoutL(KIMAPC_DONE, KImapDoneInactivityTimeSeconds);
	}

// Process output from search command
// V2 version could alter significantly and become #ifdef-unreadable
void CImImap4Session::ProcessSearchL(CImapAtom *aAtom)
	{
	// Process the reply from a uid search command into an fixed array of uids.
	// The data is recieved as a series of atoms containing numerical only data.
	// If non-numerical data is recieved, then this function will leave.
	// Use the key to do inserts in sequence. Takes care of duplicates
	TKeyArrayFix key(0,ECmpTInt32);
	TUint atomUid;
	while (aAtom)
		{
		if (aAtom->Atom().Length()>0)
			{
			// Check for numerical value.
			TChar atomChar(aAtom->Atom()[0]);
			if (atomChar.IsDigit() && aAtom->Value(atomUid)==KErrNone)
				{
				// Append it to the end search UID list
				// Put in sequence no duplicates
				TRAPD(err,iSearchList->InsertIsqL(static_cast<TUint32>(atomUid),key));
				if(err != KErrNone && err != KErrAlreadyExists)
					{
					User::Leave(err);
					}
				}
			else
				// Not a number or cant get its value.
				User::Leave(KErrArgument);
			}
		else
			{
			// Null atom.
			User::Leave(KErrArgument);
			}
		aAtom=aAtom->Next();
		}

	DBG((LogText(_L8("UID search found %d UIDs in remote folder"),iSearchList->Count())));
	}

// A tag has completed OK
void CImImap4Session::CommandCompleteL(TInt aResult)
	{
	DBG((LogText(_L8("CImImap4Session::CommandComplete(state=%d, result=%d)"),iState,aResult)));
	iCommandFailure=aResult;
	
	// Data has been received from the remote server - can cancel the cancel-timer,
	// cancel wasn't cos of hanging due to GPRS suspend.
	iCancelTimer->Cancel();

	switch(iState)
		{
	case EImapStateCapabilityWait:
		// Did we see the correct version ID?
		if (!iSeenVersion)
			{
			// No - not in any of the lines between issuing the capability
			// command and the completion of the command
			Fail(KErrImapServerVersion);
			return;
			}

		// Move into whatever logon state is required
		iState=iSavedState;

		// Need to login?
		if (iState==EImapStateLoginWait)
			{
			switch (iSecurityState)
				{
			case ESecure:
			case EUnsecure:
				// shouldn't ever get here...
				SendLoginL();
				break;

			case ENegotiating:
				SendLoginL();
				iSecurityState=ESecure;
				break;

			case EUnknown:
				if (iServiceSettings->SecureSockets())
					{
					if (iCapabilityStartTLS)
						{
						// send StartTLS
						NewTag();
						iImapIO->SetTLSResponseL();
						iImapIO->SendL(iStatus,_L8("%d STARTTLS\r\n"),iTag);
						NewTagSent();
						iState=EImapStateStartTLSWait;
						iSendQueued=EFalse; // no need to queue 'send' cause session will take care of the response.
						}
					else
						Fail(KErrImapServerNoSecurity);
					}
				else
					{
					if (iCapabilityLoginDisabled)
						Fail(KErrImapServerLoginDisabled);
					else
						{
						SendLoginL();
						iSecurityState=EUnsecure;
						}
					}
				break;
				}

			return;
			}
		break;

	case EImapStateStartTLSWait:
		SendCapabilityL();
		iSecurityState = ENegotiating;
		return;
	
	case EImapStateSelectWait:
		switch(aResult)
			{
		case KErrNone:
			// Select OK
			DBG((LogText(_L8("CImImap4Session::CommandCompleteL(): setting iState to EImapStateSelected"))));
			iState=EImapStateSelected;
			if( iCancelAndIdle )
				{
				if( iReissueIdle )
					{
					// Need to re-issue the IDLE command - probably due to a cancel
					// during a populate commmand and session was IDLE before the
					// command.
					// As the populate could have been for a message not in the Inbox,
					// (and this is the completion of SELECT command for appropriate
					// mailbox) need to ensure IDLE started in inbox.

					// Do the select (if we really need to) or skip if possible.
					if( iMailboxId==GetInbox() && iMailboxWritable )
						{
						DBG((LogText(_L8("Need to re-issue IDLE command"))));

						// No need to do the select - so re-issue the IDLE
						DoStartIdleL();				
						}
					else
						{
						DBG((LogText(_L8("Need to issue IDLE - first select Inbox (writable)"))));

						// Looks like we need to do the select...
						DoSelectL(GetInbox(), ETrue);
						}
					}
				else
					{
					DBG((LogText(_L8("Do not re-issue IDLE - cancel completed"))));
					
					// Cancelling completed - stay in this mailbox and do not issue idle.
					iCancelAndIdle = EFalse;
					}
				}
			break;

		case KErrIMAPNO:
			DBG((LogText(_L8("CImap4Session::CommandComplete(): iState=EImapStateSelectWait"))));
			DBG((LogText(_L8("-----------------------------------------------------------"))));
			DBG((LogText(_L8("CImap4Session::CommandComplete(): calling Complete()"))));
			DBG((LogText(_L8("-----------------------------------------------------------"))));
			// Select failed
			Complete(KErrIMAPNO);
			return;
			}
		break;

	case EImapStateSeparatorWait:
	case EImapStateCloseWait:
		// Back to unselected
		iState=EImapStateNoSelect;
		break;

	case EImapStateCommandWait:
	case EImapStateListWait:
	case EImapStateLsubWait:
	case EImapStateAppendResultWait:
		// Restore old state: could have been selected/unselected
		iState=iSavedState;
		break;
		
	case EImapStateFetchCancelWait:
		// No more parts to fetch - reset the fetch list
		iFetchList->Reset();

		// Dispose of any buffers we've got left
		delete iPartialLine;
		iPartialLine=NULL;
		delete iAttachmentFile;
		iAttachmentFile=NULL;
		delete iAttachmentFullPath;
		iAttachmentFullPath=NULL;
		delete iAttachmentMimeInfo;
		iAttachmentMimeInfo=NULL;
		
		// Ensure attachment file state correct so that any re-fetch will be ok
		iAttachmentFileState=EFileNotOpen;

		// All done - ensure any changes are committed to disk
		iEntry->CompleteBulk();

		// Back in selected state - do we need to move back to inbox?
		iState=EImapStateSelected;
		iSyncState=ENotSyncing;
		if( iCancelAndIdle )
			{
			if( iReissueIdle )
				{
				// Need to re-issue the IDLE command - probably due to a cancel
				// during a populate commmand and session was IDLE before the
				// command.
				// As the populate could have been for a message not in the Inbox,
				// (and this is the completion of SELECT command for appropriate
				// mailbox) need to ensure IDLE started in inbox.

				// Do the select (if we really need to) or skip if possible.
				if( iMailboxId==GetInbox() && iMailboxWritable )
					{
					DBG((LogText(_L8("Need to re-issue IDLE command"))));

					// No need to do the select - so re-issue the IDLE
					DoStartIdleL();				
					}
				else
					{
					DBG((LogText(_L8("Need to issue IDLE - first select Inbox (writable)"))));

					// Looks like we need to do the select...
					DoSelectL(GetInbox(), ETrue);
					}
				}
			else
				{
				DBG((LogText(_L8("Do not re-issue IDLE - cancel completed"))));
				
				// Cancelling completed - stay in this mailbox and do not issue idle.
				iCancelAndIdle = EFalse;
				}
			}
		break;

	case EImapStateFetchWait:
		// Fetch has completed: any more parts to go?
		iProgress.iPartsDone++;

		if (iFetchList->Count())
			{
			// Yes, fetch next one
			FetchAnItemL((*iFetchList)[0]);
			iFetchList->Delete(0,1);
			return;
			}

		// Dispose of any buffers we've got left
		delete iPartialLine;
		iPartialLine=NULL;
		delete iAttachmentFile;
		iAttachmentFile=NULL;
		delete iAttachmentFullPath;
		iAttachmentFullPath=NULL;
		delete iAttachmentMimeInfo;
		iAttachmentMimeInfo=NULL;
		// All done - ensure any changes are committed to disk
		iEntry->CompleteBulk();
		iState=EImapStateSelected;
		iSyncState=ENotSyncing;
		break;

	case EImapStateLoginWait:
		switch(aResult)
			{
		case KErrNone:
			// Login OK: do we know the separator character?
			if (!iHierarchySeparator.Length())
				{
				// No, ask server for it
				NewTag();
				iImapIO->SendL(iStatus,_L8("%d LIST \"\" \"\"\r\n"),iTag);
				NewTagSent();
				iState=EImapStateSeparatorWait;
				return;
				}
				
			iState=EImapStateNoSelect;
			break;

		case KErrIMAPNO:
		case KErrImapBadLogon:
			// Bad username/password
			Fail(KErrImapBadLogon);
			return;
			}
		break;
	case EImapStateSelected:
		{
		// reset flag after stop idle completes
		iCompoundStopIdle = EFalse;
		iStoppingIdleForSync = EFalse;
		}
	case EImapStateCreateWait:
	case EImapStateRenameWait:
	case EImapStateDeleteWait:
	case EImapStateSubscribeWait:
		// Modification operation has happened: did it go OK?
		if (aResult==KErrNone)
			{
			TMsvEmailEntry message;
			SetEntryL(iCommandIds[0]);

			switch(iState)
				{
			case EImapStateCreateWait:
				{
				// We need to create the actual folder, as it now exists on the server
				message.iType=KUidMsvFolderEntry;
				message.iMtm=KUidMsgTypeIMAP4;
				message.iServiceId=iServiceId;
				message.SetMtmData1(0);
				message.SetMtmData2(0);
				message.SetMtmData3(0);
				message.SetValidUID(EFalse);
				message.SetMailbox(ETrue); // Default to creating a mailbox
				message.SetComplete(ETrue);
				message.iSize=0;
				message.iDetails.Set(iCommandBuf);
				iEntry->CreateEntry(message);
				// Save the created id in the progress buffer
				iProgress.iReturnedMsvId=message.Id();
				break;
				}

			case EImapStateRenameWait:
				{
				// Modify the entry
				message=iEntry->Entry();
				message.iDetails.Set(iCommandBuf);
				ChangeEntryL(message);
				break;
				}

			case EImapStateDeleteWait:
				// Delete the entry: we are set to the parent currently
				iEntry->DeleteEntry(iCommandIds[1]);
				break;

			case EImapStateSubscribeWait:
				// Set/reset the subscription flag
				message=iEntry->Entry();
				message.SetSubscribed(iCommandFlags[0]);
				ChangeEntryL(message);
				break;

			default: // To stop AER warnings
				break;
				}

			// Back to previous state
			iState=iSavedState;
			}
		DBG((LogText(_L8("CImap4Session::CommandComplete(): iState=EImapStateSubscribeWait"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::CommandComplete(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		Complete(aResult);
		return;

	case EImapStateDeleteAllWait:
		// Expunge the folder by closing
		NewTag();
		iImapIO->SendL(iStatus,_L8("%d CLOSE\r\n"),iTag);
		NewTagSent();
		iState=EImapStateDeleteFolderWait;
		return;

	case EImapStateDeleteFolderWait:
		{
		// Delete all children in folder locally as they have all been expunged
		SetEntryL(iMailboxId);
		GetChildrenL(*iSelection);
		TInt a=0;
		while(a<iSelection->Count())
			iEntry->DeleteEntry((*iSelection)[a++]);
		iSelection->Reset();

		// Back to unselected as we've closed the folder
		iState=EImapStateNoSelect;
		break;
		}

	case EImapStateSynchroniseWait:
		{
		if (aResult==KErrNone && iSyncState==ESyncSearch)
			{
			iFolderPosition+=KImapUidSearchSize;
			// Check to see if we have all the messages.
			if (iFolderPosition>=iMailboxSize)
				{
				DBG((LogText(_L8("UID search complete"))));

				// Set the mailbox size to the number of UIDs returned by the search
				iMailboxSize = iSearchList->Count();

				iSyncState=ESyncListOld;
				// We have the full list of remote UIDs - fall through.
				}
			else
				{
				// Should be able to hit this code if KImapUidSearchSize is reduced to < the size
				// of the remote mailbox (iMailboxSize)
				// SearchString non 0 means a refined UID SEARCH is required
				DBG((LogText(_L8("UID search - get next manageable block of UIDs"))));
				NewTag();
				if(iServiceSettings->SearchString().Length() != 0)
					{
					// Refined search required
					// Still uses the indexes but appends user specified search criteria
					_LIT8(KSearchString,"%d UID SEARCH %d:%d %S\r\n");
					TPtrC8 ptr = iServiceSettings->SearchString();
					iImapIO->SendL(iStatus,KSearchString,iTag,iFolderPosition+1,Min(iMailboxSize,iFolderPosition+KImapUidSearchSize),&ptr);
					}
				else
					// Normal unrefined SEARCH. Will pull back all the UIDs between indexes
					{
					_LIT8(KSearchString,"%d UID SEARCH %d:%d\r\n");
					iImapIO->SendL(iStatus,KSearchString,iTag,iFolderPosition+1,Min(iMailboxSize,iFolderPosition+KImapUidSearchSize));
					}
				NewTagSent();
				return;
				}
			}

		if (aResult==KErrNone && iSyncState==ESyncListOld)
			{
			// At this point the remote command to search out folder UIDs has completed. This
			// list will contain all UIDs for messages in the remote folder. This may be too
			// many, and if so, the list is truncated to only include the N most recent 
			// messages in accordance with the synchronisation limit.

			TInt local=0;
			TInt remote=0;
			// if no UID Search string defined then the old logic is used
			TInt syncThresh = 0;
			if(iServiceSettings->SearchString().Length() != 0)
				{
				syncThresh = 0;
				}
			else	// If no search string is set we will use the old behaviour
				{
				syncThresh=(iSyncLimit<iMailboxSize)?iMailboxSize-iSyncLimit:0;	
				}
			TBool orphanThis=EFalse;
			TBool folderPositionFound=EFalse;
			iFolderPosition=0;

			// At this stage (prior to truncation), we have a full list of all the message
			// UIDs in the remote folder. We also have a list of all UIDs for messages in
			// the local folder. With these lists, we can establish which local messages
			// have been orphaned and which have not using the following rules.

			// * If the remote message is no longer there:
			//		(1) Then the local message is orphaned (always).
			// * If the remote message is still there and it is not one of the N most recent:
			//		(2) If the local message has body parts do nothing.
			//		(3) If the local message does not have body parts, then it is orphaned
			//		    unless is is a message we have selected for download.
			// * If the remote message is still there and it is one of the N most recent:
			//		(4) Do nothing.

			// Search through the local folder list while checking the remote folder list.
			while (local<iFolderIndex.Size() && iFolderIndex[local].iMsvId!=-1)
				{
				orphanThis=EFalse;	// Default orphaning of this message to false.

				// Find next uid in remote folder uid list that is >= local entry uid.
				while (remote<iMailboxSize)
					{
					if (remote < iSearchList->Count() &&
					    (*iSearchList)[remote]>=iFolderIndex[local].iUid)
						break;
					remote++;
					}

				// Within scope of search list?
				if (remote<iMailboxSize)
					{
					TBool inSyncRange=(remote>=syncThresh);
					TBool uidMatch=((*iSearchList)[remote]==iFolderIndex[local].iUid);
					TBool uidNewer=((*iSearchList)[remote]>iFolderIndex[local].iUid);

					// Folder position must point to 1st old local message that matches a
					// remote message that will be sync'ed.
					if (uidMatch && inSyncRange && !folderPositionFound)
						{
						iFolderPosition=local;
						folderPositionFound=ETrue;
						}

					if (uidNewer)
						{
						// Here the next remote uid is greater than the local uid indicating that
						// a message has been removed on the remote folder and that this local
						// message should be orphaned. See case (1) above.
						
						DBG((LogText(_L8("Message no longer available on remote server, orphaning"))));
						orphanThis=ETrue;
						}
					else if (uidMatch && !inSyncRange)
						{
						// Here the remote uid matches the local uid, but the message falls outside
						// of the N most recent messages. See cases (2) & (3).

						DBG((LogText(_L8("Local message old (%u)"),iFolderIndex[local].iUid)));

						SetEntryL(iFolderIndex[local].iMsvId);
						TMsvEmailEntry message(iEntry->Entry());
						TBool inSyncSelection = EFalse;

						// Is the message part of the synchronisation selection?
						// If so, we will want to view the message after the sync so don't delete.
						if (iSynchronisationSelection && iSynchronisationSelection->Count() > 1)
							{
							if (iSynchronisationSelection->Find(iFolderIndex[local].iMsvId) != KErrNotFound)
								{
								inSyncSelection = ETrue;
								}
							}

						// Does message have any downloaded parts?
						if (!message.Complete() && 
							!message.BodyTextComplete() && 
							!inSyncSelection)
							{
							// The local message does not have any body parts and
							// is not selected for download, so it is orphaned.
							// See case (3) above.

							DBG((LogText(_L8("Local message (%u) is only header and not selected for download, deleting"),iFolderIndex[local].iUid)));
							orphanThis=ETrue;
							}
						}
					}
				else
					{
					// Outside of scope of search list, so none of the remaining local
					// messages are present in the remote folder and therefore are orphaned.
					// See case (1) above.

					DBG((LogText(_L8("Message no longer available on remote server, orphaning"))));
					orphanThis=ETrue;
					}

				// Orphan this one?
				if (orphanThis)
					{
					OrphanMessageL(iFolderIndex[local].iMsvId);
					iFolderIndex.Expunge(local+1);
					iOrphanedMessages++;
					}
				else
					{
					// If we have arrive here, then the local message is one of the N most 
					// recent and still exists remotely OR it exists remotely, is not one of
					// the N most recent and the local message had body parts.
					// See cases (2) & (4) above.

					local++;
					}
				} // End of the big while()

			// Trim the list down to the most recent UIDs consistant with the sync limit.
			if (syncThresh && iSearchList->Count() > syncThresh)
				iSearchList->Delete(0,syncThresh);

			// So now "iFolderIndex" will only have messages that are in the remote folder.
			// And therefore, the highest UID stored in "iFolderIndex" will be the most 
			// recent old message. Also, the lowest UID in "iSearchList" will be the oldest
			// sync'able remote message.

			// Are there any old messages left?
			if (iFolderIndex.Size() && folderPositionFound)
				{
				DBG((LogText(_L8("Updating flags for %d old messages (UIDs %u to %u)"),
						iFolderIndex.Size(),(*iSearchList)[0],iHighestUid)));

				// Re-assign highest UID, the previous highest may no longer exist.
				iHighestUid=iFolderIndex[iFolderIndex.Size()-1].iUid;
				iSyncState=ESyncListNew;

				// Fetch old messages.
				NewTag();
				// If a UID search string has been specified, the we should create the UID FETCH
				// string from the UID integer list otherwise they'll all come down.
				if(iServiceSettings->SearchString().Length() != 0)
					{
					CreateUidStringL(); // Construct the UID string from the UID list
					TPtrC8 ptr(iUidString->Des());
					_LIT8(KFetchString,"%d UID FETCH %S (UID FLAGS)\r\n");
					iImapIO->SendL(iStatus,KFetchString,iTag,&ptr);
					}
				else
					{
					_LIT8(KFetchString,"%d UID FETCH %d:%d (UID FLAGS)\r\n");
					iImapIO->SendL(iStatus,KFetchString,iTag,(*iSearchList)[0],iHighestUid);
					}
				NewTagSent();
				return;
				}
			else
				{
				DBG((LogText(_L8("No old message headers to update"))));

				iSyncState=ESyncListNew;
				iHighestUid=0;
				// All remote messages are new - fall through.
				}
			}

		if (aResult==KErrNone && iSyncState==ESyncListNew)
			{
			// At this point, the remote command to fetch all old messages has completed. 
			// Now we can look at fetching all new messages. 'iHighestUid' will contain the
			// highest UID of the old messages. The top entry in 'iSearchList' will contain
			// the highest UID in the remote folder. This gives us the range of UID to fetch
			// for new messages.

			// First check are there any new messages to fetch? If 'iHighestUid' is the highest
			// UID locally and remotely, then we finished sync'ing when we completed the old
			// sync.
			if (iSearchList->Count() == 0)
				{
				DBG((LogText(_L8("Search List is empty"))));				
				}
			else if (iHighestUid<(*iSearchList)[iSearchList->Count()-1])
				{
				TUint32 uidLow=iHighestUid;
				TUint32 uidHigh=(*iSearchList)[iSearchList->Count()-1];

				// Only want new messages.
				uidLow++;

				// Are there only new messages (and no old)?
				if (iHighestUid==0)
					{
					// Set this to ensure range is correct.
					uidLow=(*iSearchList)[0];
					}
				
				// Perform the new sync.
				SynchroniseNewL(uidLow,uidHigh);
				return;
				}
			else
				{
				DBG((LogText(_L8("No new message headers to sync"))));

				// Synchronisation complete - fall through.
				}
			iSyncState=ESyncNew;
			}

		// Synchronising: moving on to the next state?
		if (aResult==KErrNone && iSyncState==ESyncOld)
			{
			// At this point, the remote command to list old messages has
			// completed: however, if there was a 'gap' at the end (ie messages at
			// the end of our copy of the mailbox had been deleted) we won't have
			// noticed it until this point as we woulnd't have got back in sync which
			// happens when we get a UID which isn't sequential.
			// So, as we now know there are no more messages in the range we know about
			// locally, anything else must have been deleted remotely.
			while(iFolderPosition<iFolderIndex.Size())
				{
				// Orphan this message
				DBG((LogText(_L8("Orphaning UID %u"),iFolderIndex[iFolderPosition].iUid)));

				// Do it
				OrphanMessageL(iFolderIndex[iFolderPosition].iMsvId);

				// Remove it from the index
				iFolderIndex.Expunge(iFolderPosition+1);

				// Increment stats
				iOrphanedMessages++;
				}

			// Anything to new sync? If we've processed all the messages up to
			// iMailboxSize, then there's nothing left that might possibly be
			// new
			if (iMsgsDone<iMailboxSize)
				{
				// Do new sync
				SynchroniseNewL();
				return;
				}

			iSyncState=ESyncNew;
			}

		if (aResult==KErrNone && iSyncState==ESyncNew)
			{
			// If there were any "missing" messages found during the sync that we should have
			// mirrored previously, get these now.
			if (iMissingUidLow!=0 && iMissingUidHigh!=0)
				{
				DBG((LogText(_L8("Missing messages detected %d - %d"),iMissingUidLow,iMissingUidHigh)));

				SynchroniseNewL(iMissingUidLow,iMissingUidHigh);
				iMissingUidLow=iMissingUidHigh=0;
				return;
				}
			}

		// If we got a NO response in the course of the sync, we've not fully sync'ed the folder,
		// so we shouldn't update the last sync date, etc. BUT as NO isn't a fatal response, we should
		// not disconnect.
		if (aResult!=KErrIMAPNO)
			{
			// Are we configured to update the \seen flag on the server?
			if (iServiceSettings->UpdatingSeenFlags())
				{
				DBG((LogText(_L8("Sync completed: updating servers flags"))));

				// Now drop through to EImapStateSetSeenWait case
				iState = EImapStateSetSeenWait;
				iSyncState=ENotSyncing;
				}
			else
				{
				// Not updating servers \seen flags - use old code to continue after sync
				// Back to selected state
				iState=EImapStateSelected;
				iSyncState=ENotSyncing;

				// We've synchronised the folder. Update iDate field to
				// indicate last sync date of this folder.
				// This also updates the 'folders done' count
				SyncCompleteL();
                                DBG((LogText(_L8("CImap4Session::CommandComplete(): iState=EImapStateSynchroniseWait"))));
				DBG((LogText(_L8("-----------------------------------------------------------"))));
				DBG((LogText(_L8("CImap4Session::CommandComplete(): calling Complete()"))));
				DBG((LogText(_L8("-----------------------------------------------------------"))));
				Complete(aResult);		
				return;
				}
			}
		else
			{
			// NO isn't a fatal response.Ignore it,continue...
			iState=EImapStateSelected;
			iSyncState=ENotSyncing;
            DBG((LogText(_L8("CImap4Session::CommandComplete(): iState=EImapStateSynchroniseWait, aResult == KErrIMAPNO"))));
			DBG((LogText(_L8("-----------------------------------------------------------"))));
			DBG((LogText(_L8("CImap4Session::CommandComplete(): calling Complete()"))));
			DBG((LogText(_L8("-----------------------------------------------------------"))));
			Complete(aResult);
			return;
			}
		}

	case EImapStateSetSeenWait:    // Only a valid state when iServiceSettings->UpdatingSeenFlags() is true
		// We may not be able to set all the seen flags due to a large list, 
		//  so call ProcessSeenFlagsL until False is returned
		if (ProcessSeenFlagsL(ESetSeenFlag))
			{
			// More have been processed - wait for response in same state.
			return;
			}
		// Now drop through to EImapStateClearSeenWait state
		iState=EImapStateClearSeenWait;
		iSyncState=ENotSyncing;

	case EImapStateClearSeenWait:  // Only a valid state when iServiceSettings->UpdatingSeenFlags() is true
		// We may not be able to clear all the seen flags due to a large list, 
		//  so call ProcessSeenFlagsL until False is returned
		if (ProcessSeenFlagsL(EClearSeenFlag))
			{
			// More have been processed - wait for response in same state.
			return;
			}

		// All seen flags processed - return to selected state
		// Will only arrive here if iServiceSettings->UpdatingSeenFlags() is true
		iState=EImapStateSelected;
		iSyncState=ENotSyncing;

		// We've synchronised the folder. Update iDate field to
		// indicate last sync date of this folder.
		// This also updates the 'folders done' count
		SyncCompleteL();
		DBG((LogText(_L8("CImap4Session::CommandComplete(): iState=EImapStateClearSeenWait"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::CommandComplete(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		Complete(aResult);
		return;

	case EImapStateLogoutWait:
		DoDisconnect();
		break;

	case EImapStateDeleteMarkWait:
		// Messages marked for deletion OK, send close command
		SendMessageL(KIMAPC_CLOSE);
		iState=EImapStateExpungeWait;
		return;

	case EImapStateExpungeWait:
		{
		DBG((LogText(_L8("Close completed OK: expunging messages locally"))));

		// Expunge the messages locally, the close was successful
		for(TInt a=0;a<iFolderIndex.Size();a++)
			{
			// If it's got a valid MsvId, delete it!
			if (iFolderIndex[a].iMsvId)
				{
				DBG((LogText(_L8("Expunging message %x"),iFolderIndex[a].iMsvId)));

				DeleteMessageL(iFolderIndex[a].iMsvId);
				}
			}

		// Back to unselected state (close sucessful)
		iState=EImapStateNoSelect;
		break;
		}

	case EImapStateIdleWait:
		{
		DBG((LogText(_L8("CImImap4Session::CommandCompleteL(): iState = EImapStateIdleWait"))));
 		if (iCompoundStopIdle) 
			{
			// the reply from server is missed if
			// compound command
			GetReply(EFalse);
			}
		break;		
		}

	case EImapStateIdling:
		{
		DBG((LogText(_L8("CImImap4Session::CommandCompleteL(): iState = EImapStateIdling"))));
 		if (iReissueIdle)
			{
			// IDLE command has been re-issued after synchronise OR a re-issue
			// after a cancel-and-idle (if idling before).
			// NOTE - also unset cancel-and-idle flag here too.
			iReissueIdle = EFalse;
			iCancelAndIdle	= EFalse;
			IssueIdleRead();
			}
		break;		
		}

	case EImapStateStopIdleWait:
		{
		DBG((LogText(_L8("Idling completed OK"))));
		
		iState=EImapStateSelected;

		if (FolderChanged())
			{
			DBG((LogText(_L8("CImImap4Session::CommandCompleteL(): FolderChanged"))));

			// If the folder has changed then we are going to do a synchronise to
			// ensure we have up to date message information. At the end of the
			// synchronise we would normally reissue the idle command, however
			// we don't want to do this if we are stopping idle due to a
			// compound command or a sync command. The compound / sync active
			// objects will reissue the idle themselves.
			if(!iCompoundStopIdle && !iStoppingIdleForSync)
				{
				DBG((LogText(_L8("CImImap4Session::CommandComplete(): ReissueIdle = true"))));
				iReissueIdle=ETrue;
				}
			DoSynchroniseL(EFalse);
			return;
			}
		else if (iReissueIdle)
			{
			if( !iCancelAndIdle && !iIdleTimerExpired )
				{
				// Don't unset the iReissueIdle flag - need it to be true to ensure
				// that IssueIdleRead is issued once 'Done' is received.
				// FYI - IssueIdleRead is normally called in the DoComplete method 
				// when the StartIdle request completes. If we've cancel-and-idle-ed,
				// then DoComplete won't be called when we do actually go idle.
				DBG((LogText(_L8("CImImap4Session::CommandComplete(): ReissueIdle = false"))));
				iReissueIdle = EFalse;
				}
			iIdleTimerExpired = EFalse;
			DoStartIdleL();
			return;
			}
		else if (iDisconnectAfterIdleStopped)
			{
			iDisconnectAfterIdleStopped=EFalse; 
			DoDisconnectL();
			return;
			}

		break;		
		}
		
	default:
		gPanic(ECommandCompleteInUnknownState);
		return;
		}

	// Completing an operation upwards: ensure iEntry is not pointing to anything
	SetEntryL(NULL);
        
        DBG((LogText(_L8("CImap4Session::CommandComplete(): End of method"))));
	DBG((LogText(_L8("-----------------------------------------------------------"))));
	DBG((LogText(_L8("CImap4Session::CommandComplete(): calling Complete()"))));
	DBG((LogText(_L8("-----------------------------------------------------------"))));
	
	Complete(KErrNone);
	}
	
	
/* 
Overrides CMsgActive::RunL() to allow check for errored completion of
autonomously initiated IO requests, specifically the reissue of IDLE
commands on idle timeout.
*/
void CImImap4Session::RunL()
	{
	TInt status=iStatus.Int();
	if (status < KErrNone &&
		iIdleTimerExpired && 
		(iState==EImapStateIdleWait || iState==EImapStateStopIdleWait))
		{
		DoComplete(status);
		}
	else
		{
		// Behave exactly as CMsgActive::RunL
		if (status>=KErrNone)
			{
			TRAPD(error,DoRunL());      // continue operations, may re-queue
			__ASSERT_DEBUG(error==KErrNone || !IsActive(),User::Invariant());   // must not requeue in error situations
			if (IsActive())             // requeud
				return;
			status=error;
			}
		Complete(status);
		}
	}

	

// A child async process has completed
void CImImap4Session::DoRunL()
	{
	DBG((LogText(_L8("CImImap4Session::DoRunL(status=%d, state=%d)"),iStatus.Int(),iState)));

	// Did we have a send queued? If so, this is just the send completion: we
	// now queue a receive and clear the send flag
	if (iSendQueued)
		{
                DBG((LogText(_L8("CImImap4Session::DoRunL(): iSendQueued"))));

		// No send queued anymore...
		iSendQueued=EFalse;
		// Any problems sending?
		if(iStatus.Int()!=KErrNone)
			{
			// Yes, humm...
			DBG((LogText(_L8("Error during send %d"),iStatus.Int())));

			// We have to Fail() here, as the connection might be screwed:
			// we just don't know. Fail() will disconnect us and reset our
			// internal state.
			Fail(KErrImapSendFail);
			}
		else
			{
			// Have we just issued a partial fetch, ie waiting for body
			// size?
			if (iSizeWait)
				{
				// Queue partial line fetch
				GetReply(ETrue);
				}
			else
				{
				// Queue full line read
				GetReply(EFalse);
				}
			}	
		return;
		}

	// Was a receive queued? If so, we need to get the new root pointer
	if (iReceiveQueued)
		{
                DBG((LogText(_L8("CImImap4Session::DoRunL(): iReceiveQueued"))));
 		// Not anymore
		iReceiveQueued=EFalse;

		// Get root
		iRootAtom=iImapIO->RootAtom();
                 
                DBG((LogText(_L8("CImImap4Session::DoRunL(): Got root"))));

		// Did we get the whole line?
		if (iStatus.Int()==KErrFoundEOL)
			{
                        DBG((LogText(_L8("CImImap4Session::DoRunL(): got the whole line"))));
			iGotWholeLine=ETrue;
			iStatus=KErrNone;
			}
		else
                        {
			DBG((LogText(_L8("CImImap4Session::DoRunL(): didn't got the whole line"))));			
			iGotWholeLine=EFalse;
                        }
		}

	// Problems with connection?
	if (iStatus.Int()!=KErrNone)
		{
		DBG((LogText(_L8("CImImap4Session::DoRunL(): Problems with connection"))));
                if (iState==EImapStateConnectWait)
			Fail(KErrImapConnectFail);
		else
			Fail(KErrImapServerFail);

		return;
		}
        
        DBG((LogText(_L8("CImImap4Session::DoRunL(): checking iState"))));

	switch(iState)
		{
	// Waiting for connect response
	case EImapStateConnectWait:
		{
		// Get the bearer idle timeout
		TUint32 timeout;
		User::LeaveIfError(iImapIO->GetLastSocketActivityTimeout(timeout));

		// Sets timeout to iMtmData1. This is used by Imcm.
		SetEntryL(iServiceId);
		TMsvEntry entry = iEntry->Entry();
		entry.SetMtmData1(timeout);
		iEntry->ChangeEntry(entry);
		DBG((LogText(_L8("IdleTimeout %d"),timeout)));

		// Connected, queue receive for greeting line
		iState=EImapStateGreetingWait;
		GetReply(ETrue);
		break;
		}

	// Waiting for greeting line
	case EImapStateGreetingWait:
		{
		TInt result=KErrNone; // To stop .AER warnings...

		// Process line
		TRAPD(err,result=ProcessGreetingL());
		if (err!=KErrNone)
			Fail(KErrImapServerFail);
		else if (result!=KErrNone)
			{
			// Greeting process returned an error
			DBG((LogText(_L8("CImap4Session::CommandComplete(): iState=EImapStateGreetingWait"))));
			DBG((LogText(_L8("-----------------------------------------------------------"))));
			DBG((LogText(_L8("CImap4Session::CommandComplete(): calling Complete()"))));
			DBG((LogText(_L8("-----------------------------------------------------------"))));

                        Complete(result);
			}
		else
			{
			SendCapabilityL();
			}
		break;
		}

	// Waiting for continuation response to send username
	case EImapStateLoginSendUser:
		{
		// Looking for a '+'
		CImapAtom *p=iRootAtom->ToChildL();

		// '*' indicates an untagged message
		if (p->Compare(KIMAP_UNTAGGED))
			{
			// Process it
			TRAPD(err,ProcessUntaggedL(p->ToNextL(),EFalse));

			// A problem here is a server fail, nothing else
			if (err!=KErrNone)
				Fail(KErrImapServerFail);
			}
		else if (p->Compare(KIMAP_CONTINUATION))
			{
			// Send username literal

			// Literal password?
			if (iLiteralPassword)
				{
				// Turn off logging before sending
				iImapIO->PerformLogging(EFalse);
				iImapIO->SendL(iStatus,_L8("%S {%d}\r\n"),iUsername,iPassword->Length());
				iImapIO->PerformLogging(ETrue);
	
				iState=EImapStateLoginSendPassword;
				}
			else
				{
				// Turn off logging before sending
				iImapIO->PerformLogging(EFalse);
				iImapIO->SendL(iStatus,_L8("%S %S\r\n"),iUsername,iPassword);
				iImapIO->PerformLogging(ETrue);
				
				iState=EImapStateLoginWait;
				}

			iSendQueued=ETrue;
                        DBG((LogText(_L8("*******************************************************"))));
			DBG((LogText(_L8("CImap4Session::DoRunL(): waiting for iImapIO to wake me"))));
			DBG((LogText(_L8("*******************************************************"))));

			SetActive();
			}
		break;
		}

	// Waiting for continuation response to send password
	case EImapStateLoginSendPassword:
		{
		// Looking for a '+'
		CImapAtom *p=iRootAtom->ToChildL();

		// '*' indicates an untagged message
		if (p->Compare(KIMAP_UNTAGGED))
			{
			// Process it
			TRAPD(err,ProcessUntaggedL(p->ToNextL(),EFalse));

			// A problem here is a server fail, nothing else
			if (err!=KErrNone)
				Fail(KErrImapServerFail);
			}
		else if (p->Compare(KIMAP_CONTINUATION))
			{
			// Send password literal
			iSendQueued=ETrue;
                        DBG((LogText(_L8("*******************************************************"))));
			DBG((LogText(_L8("CImap4Session::DoRunL(): waiting for iImapIO to wake me"))));
			DBG((LogText(_L8("*******************************************************"))));

			SetActive();
			// Turn off logging before sending
			iImapIO->PerformLogging(EFalse);
			iImapIO->SendL(iStatus,_L8("%S\r\n"),iPassword);
			iImapIO->PerformLogging(ETrue);
			
			iState=EImapStateLoginWait;
			}
		break;
		}

	case EImapStateSetSeenWait:		// Wait for setting the send flags response
	case EImapStateClearSeenWait:   // Wait for clearing the send flags response
	case EImapStateCapabilityWait:	// Wait for reply to capability command
	case EImapStateLoginWait:		// Wait for reply to login command
	case EImapStateCommandWait:		// Generic OK/BAD command issued, wait for response
	case EImapStateCreateWait:		// Create command issued, wait for response
	case EImapStateRenameWait:		// Rename command issued, wait for response
	case EImapStateDeleteWait:		// Delete command issued, wait for response
	case EImapStateDeleteAllWait:	// DeleteAll command issued, wait for response
	case EImapStateDeleteFolderWait:// DeleteAll has issued a folder delete, wait for response
	case EImapStateSeparatorWait:	// Wait for reply to null list command
	case EImapStateSubscribeWait:	// Wait for subscription command reply
	case EImapStateSelectWait:		// Wait for all select information
	case EImapStateSynchroniseWait:	// Wait for synchronise
	case EImapStateListWait:		// Wait for all list information
	case EImapStateLsubWait:		// Wait for all lsub information
	case EImapStateAppendResultWait:// Wait for append result
	case EImapStateDeleteMarkWait:	// Marking messages for deletion before CLOSE
	case EImapStateExpungeWait:		// Messages expunged: remove them locally
	case EImapStateExpungeAllWait:	// Messages expunged from remote, remove them
	case EImapStateCloseWait:		// Wait for close to finish
	case EImapStateLogoutWait:		// Wait for logout
	case EImapStateStartTLSWait:	// Wait for starttls to return
	case EImapStateIdleWait:
	case EImapStateIdling:
	case EImapStateStopIdleWait:
		{
		TInt result=KErrNone; // To stop .AER warnings...

		// Process it
		TRAPD(err,result=ProcessCommandReplyL());
		if (err!=KErrNone)
			{
			// check for TLS errors
			if (iState == EImapStateCapabilityWait && iSecurityState == ENegotiating)
				err = KErrImapTLSNegotiateFailed;
			
			// We've had a fail during reply processing: pass it on
			Fail(err);
			}
		else
			{
			switch(result)
				{
			case KErrNotReady:
				{
				if (IsIdling() && FolderChanged())
					{
                    DBG((LogText(_L8("CImImap4Session::DoRunL(): Calling DoStopIdleL"))));
					DoStopIdleL();
					}
				else if (IsIdling())
					{
					IssueIdleRead();
					return;
					}
				else
					{
					// Need the next line/part of line
					GetReply(EFalse);
					}
				break;
				}

			default:
				// Complete it if there's no error or if it's just a 'NO'
				// reply (ie, KErrNotSupported)
				if (result>=KErrNone || result==KErrNotSupported)
					{
					DBG((LogText(_L8("CImImap4Session::DoRunL(): No error - calling CommandComplete()"))));
					CommandCompleteL(result);
					}
				else
					Fail(result);
				break;
				}
			}
		break;
		}

	case EImapStateAppendSizeWait:	// Work out the size of the append
		{
		// Sizing operation completed, get the size
		iCommandSize=iMessageSizer->MessageSize();
		delete iMessageSizer;
		iMessageSizer=NULL;

		DBG((LogText(_L8("Sized message to %d bytes"),iCommandSize)));

		iProgress.iBytesToDo=iCommandSize;
		iProgress.iBytesDone=0;
		
		// Make a sender, to actually send the text
		delete iMessageSender;
		iMessageSender=NULL;
		iMessageSender=CImSendMessage::NewL(/*iFs,*/*iEntry);

		// Change in API, should be retrieving iSettings.SendCopyToSelf(), 
		// instead using default ESendNoCopy.
		iMessageSender->InitialiseL(iCommandIds[0], ESendAsMimeEmail, 
									iMessageDate, iHost, iCharset, ESendNoCopy);

		// Make path to the message's destination, and send command
		HBufC8* path=MakePathL(iCommandIds[1],ETrue);

		// Reset the entry to first param(message id), as it will be changed to 
		// second param(directory id) in the MakePathL function. This entry 
		// will be sent to destination as message(line by line) by CImSendMessage class.
		SetEntryL(iCommandIds[0]);
		
		CleanupStack::PushL(path);
		DoQuoteL(path);
		TPtrC8 pathptr=path->Des();
		NewTag();
		iImapIO->SendL(iStatus,_L8("%d APPEND \"%S\" {%d}\r\n"),iTag,&pathptr,iCommandSize);
		NewTagSent();
		CleanupStack::PopAndDestroy();

		// Append the actual message
		iState=EImapStateAppendPromptWait;
		break;
		}

	case EImapStateAppendPromptWait: // Wait for prompt before sending message body
		{
		// Looking for a '+'
		CImapAtom *p=iRootAtom->ToChildL();

		// '*' indicates an untagged message
		if (p->Compare(KIMAP_UNTAGGED))
			{
			// Process it
			TRAPD(err,ProcessUntaggedL(p->ToNextL(),EFalse));
			if (err!=KErrNone)
				Fail(err);
			}
		else if (p->Compare(KIMAP_CONTINUATION))
			{
			// Start sending message body: move into sending body state
			iState=EImapStateAppendWait;

			// Make line buffer
			delete iLineBuffer;
			iLineBuffer=NULL;
			iLineBuffer=HBufC8::NewL(KImMailMaxBufferSize);

			// Send first line: Must be more than one line, so we don't bother
			// with checking the return code.
			TInt padcount=0;
			TPtr8 line=iLineBuffer->Des();
			iMessageSender->NextLineL(line,padcount);
			iImapIO->SendL(iStatus,_L8("%S"),&line);

			DBG((LogText(_L8("CImap4Session::DoRunL(): iState = EImapStateAppendWait"))));
			DBG((LogText(_L8("*******************************************************"))));
			DBG((LogText(_L8("CImap4Session::DoRunL(): waiting for iImapIO to wake me"))));
			DBG((LogText(_L8("*******************************************************"))));

                        SetActive();
			iSendQueued=EFalse; // We want to come back to here - we've not sent a command
			iProgress.iBytesDone+=line.Length();

			DBG((LogText(_L8("Sent: %d '%S'"),line.Length(),&line)));
			}
		break;
		}

	case EImapStateAppendWait:
		{
		// Send line of message
		TInt padcount=0;
		TPtr8 line=iLineBuffer->Des();
		if (iMessageSender->NextLineL(line,padcount)==KImCvFinished)
			{
			// Send last line, plus CRLF to terminate command
			iImapIO->SendL(iStatus,_L8("%S\r\n"),&line);
			iProgress.iBytesDone+=line.Length();

			DBG((LogText(_L8("Sent: %d '%S'"),line.Length(),&line)));
			DBG((LogText(_L8("Total bytes sent %d plus the CRLF"),iProgress.iBytesDone)));

			delete iLineBuffer;
			iLineBuffer=NULL;
			delete iMessageSender;
			iMessageSender=NULL;
			iSendQueued=ETrue;
			iState=EImapStateAppendResultWait;
			}
		else
			{
			// Send a line
			iImapIO->SendL(iStatus,_L8("%S"),&line);
			iSendQueued=EFalse;
			iProgress.iBytesDone+=line.Length();

			DBG((LogText(_L8("Sent: %d '%S'"),line.Length(),&line)));
			}

                DBG((LogText(_L8("CImap4Session::DoRunL(): iState = EImapStateAppendWait"))));
		DBG((LogText(_L8("*******************************************************"))));
		DBG((LogText(_L8("CImap4Session::DoRunL(): waiting for iImapIO to wake me"))));
		DBG((LogText(_L8("*******************************************************"))));

		SetActive();
		break;
		}

	case EImapStateFetchCancelWait:
	case EImapStateFetchWait:// Wait for body length/data
		{
		TInt result=KErrNone; // To stop .AER warnings...

		// Process it
		TRAPD(err,result=ProcessCommandReplyL());
	
		if (err!=KErrNone)
			Fail(err);
		else
			{
			//LogText(_L8("ProcessCommandReplyL() returned %d\n"),result);
			switch(result)
				{
			case KErrNotReady:
				// Still waiting for body size, another partial fetch
				if (iSizeWait)
					GetReply(ETrue);
				else
					GetReply(EFalse);
				break;
			case KErrImapInvalidServerResponse:
				// Nothing to do , return back
				break;

			case KErrWrite:
				// Process has issued a command of its own, nothing for us to
				// do here
				break;

			default:
				// Complete it
				if (!iCommandsOutstanding)
					{
					DBG((LogText(_L8("CImap4Session::DoRunL(): No commands outstanding- calling CommandComplete()"))));
 					CommandCompleteL(result);
					}
				else
					{
					if (iJustSentFetch)
 						{
						DBG((LogText(_L8("CImap4Session::DoRunL(): Just sent fetch"))));
   						iJustSentFetch = EFalse;
  						}
  					else
  						{
  						// Get next fetch result
						DBG((LogText(_L8("CImap4Session::DoRunL(): NOT just sent fetch"))));
   						iSizeWait=ETrue;
  						GetReply(ETrue);
  						}
					}
				break;
				}
			}
		break;
		}

	case EImapStateMoveEntryWait:
		{
		// We're done with the moveentry

		// Park the move entry again
		iMoveEntry->SetEntry(NULL);

		// Copy the structure: the MsvId is still the same, it's just not in the same
		// place.

		//DS - Selectively using a copy or a move depending on user intention, so no need for this
		//"copy structure back to mirror" malarky.
		//		TMsvId newid=CopyLevelL(iMoveSource,iMoveSourceFolder);
	
		// Note this ID in the iRelatedId, so that higher levels know where we are again...

		//DS iMoveSource could be either the same message moved to a different place
		//or still the original message - could employ some logic for differentiating if there
		//is some need to twiddle flags etc, but I'll leave that for someone else to do...
		SetEntryL(iMoveSource);
		TMsvEntry entry=iEntry->Entry();

		entry=iEntry->Entry();
		entry.SetNew(EFalse);
		ChangeEntryL(entry);

		// Park iEntry
		SetEntryL(NULL);

		// Inform caller of new ID
		//DS as far as I can tell, this isn't used!
		//*iNewSource=newid;

		// Back to previous state
		iState=iSavedState;
		break;
		}

	default:
		gPanic(ERunLInUnknownState);
		return;
		}
        DBG((LogText(_L8("CImImap4Session::DoRunL(): exiting..."))));
	}


// The IMAP Idle case for doing a "GetReply". 
// Called by CImImap4SessionIdleRead::Start() to get the whole thing going.
void CImImap4Session::DoIdleRead(TRequestStatus& aIdleReadStatus)
	{
	// Unlike a normal read, give the reponse to the CImImap4SessionIdleRead when done
	iImapIO->GetReply(aIdleReadStatus);
	iReceiveQueued=ETrue;
	}

void CImImap4Session::CancelIdleRead()
	{
	// Undo what DoIdleRead() did
	iImapIO->Cancel();
	iReceiveQueued=EFalse;
	}

// The standard case for a normal transaction "GetReply"
// Queue request from IO layer to get the next atom
void CImImap4Session::GetReply(const TBool aPartialReturn)
	{
	// Cancel any dummy operation
	if (ImapIdleSupported()==EFalse)
		{
		CancelDummy();
		}

	// Queue the receive
	if (aPartialReturn)
		{
		// Get a partial line, as we need to see how much data is on its way
		// for flood control/issuing next fetch command to keep it streaming.
		iImapIO->GetReply(iStatus,80,ETrue);
		}
	else
		{
		// Get a *whole* line, we don't want a partial return
		iImapIO->GetReply(iStatus);
		}

	// Note that we have ???
	iReceiveQueued=ETrue;
        DBG((LogText(_L8("*******************************************************"))));
	DBG((LogText(_L8("CImap4Session::GetReply(): waiting for iImapIO to wake me"))));
	DBG((LogText(_L8("*******************************************************"))));

	SetActive();
	}

// Make a new tag, and queue send
void CImImap4Session::NewTag()
	{
	if (ImapIdleSupported()==EFalse)
		{
		// Cancel any dummy operation that might be outstanding
		CancelDummy();
		}

	// Make a new tag
	iTag++;

	// One more outstanding command
	iCommandsOutstanding++;
	}

void CImImap4Session::NewTagSent()
	{
	// Go active and note that a send has been queued
	SetActive();
	iSendQueued=ETrue;
	}

// Queue sending of a command line
void CImImap4Session::SendMessageL(const TDesC8& aMessage)
	{
	__ASSERT_DEBUG(iSendQueued==EFalse,gPanic(EAlreadySending));
	if(!(iSendQueued==EFalse))
		{	
		User::LeaveIfError(KErrInUse);//Already Sending
		}
	// Make a new tag
	NewTag();
	iImapIO->SendL(iStatus,_L8("%d %S\r\n"),iTag,&aMessage);
	NewTagSent();
	}

void CImImap4Session::SendUntaggedMessageL(const TDesC8 &aMessage)
	{
	__ASSERT_DEBUG(iSendQueued==EFalse,gPanic(EAlreadySending));
	if(!(iSendQueued==EFalse))
		{
		User::LeaveIfError(KErrInUse); // Already Sending
		}
	if (ImapIdleSupported()==EFalse)
		{
		// Cancel any dummy operation that might be outstanding
		CancelDummy();
		}

	iImapIO->SendL(iStatus,KImapCommand,&aMessage);
	DBG((LogText(_L8("*******************************************************"))));
	DBG((LogText(_L8("CImap4Session::SendUntaggedMessageL(): waiting for iImapIO to wake me"))));
	DBG((LogText(_L8("*******************************************************"))));

	SetActive();
	iSendQueued=ETrue;
	}

/**
Allows a untagged message to be sent to the server with a short idle timeout applied.
This is used when a fast response is expected
*/
void CImImap4Session::SendUntaggedMessageWithTimeoutL(const TDesC8 &aMessage, TInt aTimeout)
	{
	__ASSERT_ALWAYS(iSendQueued==EFalse,gPanic(EAlreadySending));
	if(!(iSendQueued==EFalse))
		{
		User::LeaveIfError(KErrInUse); // Already Sending
		}
	if (ImapIdleSupported()==EFalse)
		{
		// Cancel any dummy operation that might be outstanding
		CancelDummy();
		}

	iImapIO->SendWithTimeoutL(iStatus, aTimeout, KImapCommand, &aMessage);
	DBG((LogText(_L8("*******************************************************"))));
	DBG((LogText(_L8("CImap4Session::SendUntaggedMessageWithTimeoutL(): waiting for iImapIO to wake me"))));
	DBG((LogText(_L8("*******************************************************"))));

	SetActive();
	iSendQueued=ETrue;
	}

// Construct a full mailbox path, given a TMsvId
// This is expensive in memory movement terms, as it works UP the path,
// inserting new data at the start. This is based on the principle that it's
// more expensive to find an entry in the index with SetEntryL() than it is to
// move some bytes about, otherwise we'd find the path upwards then create the
// string downwards.
HBufC8* CImImap4Session::MakePathL(const TMsvId aTarget, const TBool aIncludeLeaf)
	{
	__ASSERT_DEBUG(iState>=EImapStateNoSelect,gPanic(ENotLoggedOn));
	if(!(iState>=EImapStateNoSelect))
		{
		User::LeaveIfError(KErrGeneral);
		}
	// Making a path: we start with nothing
	HBufC8 *path=HBufC8::NewLC(256);
	TBool skipfirst=ETrue;
	TMsvId traverse=aTarget;

	// Move to the entry
	SetEntryL(traverse);

	// Skipping the leaf?
	if (!aIncludeLeaf && iEntry->Entry().iType!=KUidMsvServiceEntry)
		{
		// Up a level before we generate the path
		SetEntryL(traverse=iEntry->Entry().Parent());
		}

	// check and see if we are dealing with the INBOX, in which case
	// return immediately
	if (iEntry->Entry().Parent()==iServiceId &&
		iEntry->Entry().iDetails.CompareF(KIMAP_INBOX)==0)
		{
		path->Des().Insert(0,_L8("INBOX"));
		CleanupStack::Pop();
		return path;
		}

	iCharConv->PrepareToConvertToFromOurCharsetL(KCharacterSetIdentifierImapUtf7);
	
	// While we can still go up within this service...
	while(iEntry->Entry().iType!=KUidMsvServiceEntry)
		{
		// Add the name of this component to the path
   		if (!skipfirst)
			path->Des().Insert(0,iHierarchySeparator);
		else
			skipfirst=EFalse;

		// this should be a better sized allocation but the path is
		// fixed to 256 anyway so this will do
		HBufC8* utf7=HBufC8::NewL(256);
		CleanupStack::PushL(utf7);

		TInt numUC, indexUC;
		TPtr8 des = utf7->Des();
		iCharConv->ConvertFromOurCharsetL(iEntry->Entry().iDetails, des, numUC, indexUC);
		path->Des().Insert(0,utf7->Des());

		CleanupStack::PopAndDestroy();

		// Go up a level
		SetEntryL(traverse=iEntry->Entry().Parent());
		}
	
	// Add the path at the very start, if it exists
	if (iFolderPath.Length())
		{
		// Anything there already? If not, don't bother with the separator
		if (path->Des().Length()) path->Des().Insert(0,iHierarchySeparator);
		path->Des().Insert(0,iFolderPath);
		}

	// Pop it off cleanup stack
	CleanupStack::Pop();

	// Return the path
	return(path);
	}

// Queue a connection
void CImImap4Session::ConnectL(TRequestStatus& aRequestStatus, const TMsvId aService)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Connect(%x)"),aService)));
	__ASSERT_DEBUG(iState==EImapStateDisconnected,gPanic(EOpenWhenNotClosed));
	if(!(iState==EImapStateDisconnected))
		{
		User::LeaveIfError(KErrInUse); // Open when not closed.
		}
	// Any progress we give now should be related to this connect, so we need to 
	// clear any previous progress first.
	ResetStats();

	Queue(aRequestStatus);

	// Get host details from server
	iServiceId=aService;
	SetEntryL(iServiceId);

  	// get the iap preferences
  	CEmailAccounts* account = CEmailAccounts::NewLC();
  	if (iPrefs == NULL)
  		{
  		iPrefs = CImIAPPreferences::NewLC();
  		CleanupStack::Pop(iPrefs);
  		}	

	TImapAccount id;
	id.iImapAccountId = iEntry->Entry().MtmData2();  // iMtmData2 of the service entry contains TImapAccountId
	id.iImapAccountName = iEntry->Entry().iDetails;
	id.iImapService = iEntry->Entry().iServiceId;
	id.iSmtpService = iEntry->Entry().iRelatedId;

  	account->LoadImapSettingsL(id, *iServiceSettings);
    account->LoadImapIapSettingsL(id, *iPrefs);	
  	CleanupStack::PopAndDestroy(account);

	// Copy details
	delete iUsername;	//need to delete iUsername first, just in case already it has a value.
	iUsername = NULL;
	iUsername=iServiceSettings->LoginName().AllocL();
	delete iPassword;	//need to delete iPassword first, just in case already it has a value.
	iPassword = NULL;
	iPassword=iServiceSettings->Password().AllocL();
	iFolderPath=iServiceSettings->FolderPath();
	iHost=iServiceSettings->ServerAddress();
	iPort=iServiceSettings->Port();
	iUseIdleCommand = iServiceSettings->ImapIdle();

	iIdleTimeout = iServiceSettings->ImapIdleTimeout();	
	DBG((LogText(_L8("ImapIdleTimeout %d"),iIdleTimeout)));
	// convert from seconds to microseconds
	iIdleTimeout *= 1000000;

	// Path separator: we store it as a string locally
	iHierarchySeparator.Zero();
	if (iServiceSettings->PathSeparator())
		{
		// Append to string
		iHierarchySeparator.Append(iServiceSettings->PathSeparator());
		}

	// Any characters that will need quoting in them?
	iLiteralUsername=EFalse;
	int a;
	TPtr8 userName = iUsername->Des();
	for(a=0;a<iUsername->Length();a++)
		{
		if (userName[a]<=32  || userName[a]>=127 || userName[a]=='\"' || userName[a]=='%'  ||
		    userName[a]=='(' || userName[a]==')' || userName[a]=='*'  || userName[a]=='\\' ||
		    userName[a]=='{' || userName[a]=='}' )
			{
			iLiteralUsername=ETrue;
			break;
			}
		}

	iLiteralPassword=EFalse;
	TPtr8 passWord = iPassword->Des();
	for(a=0;a<iPassword->Length();a++)
		{
		if (passWord[a]<=32  || passWord[a]>=127 || passWord[a]=='\"' || passWord[a]=='%'  ||
		    passWord[a]=='(' || passWord[a]==')' || passWord[a]=='*'  || passWord[a]=='\\' ||
		    passWord[a]=='{' || passWord[a]=='}' )
			{
			iLiteralPassword=ETrue;
			break;
			}
		}
	
	// Until we know we're seeing CC:Mail...
	iTalkingToCCMail=EFalse;
	iTalkingToOpenMail=EFalse;

	// Start the connect
	iState=EImapStateConnectWait;
	iSendQueued=EFalse;
	TBool sslWrappedSocket=iServiceSettings->SSLWrapper();
	
	// if local primarysession is active then set the local textserversion of ImapIO object.
	if(iPrimarySession)
		{
		// Setting of PrimaryTextServerSession, Going to be set on the secondary session.
		iImapIO->SetPrimaryTextServerSession(iPrimarySession->GetImap4Session()->GetTextServerSession());		
		}
	iImapIO->ConnectL(iStatus,iHost,iPort,*iPrefs, sslWrappedSocket);
	
#ifdef PRINTING
	// Log version number now logfile is open
	LogText(_L8("IMPS release 022.8"));

	// Log connection destination
	LogText(_L8("Connection queued to %S, port %d"),&iHost,iPort);

	// Note any literal usage
	if (iLiteralUsername)
		LogText(_L8("Username contains unusual characters: using literal for username"));
	if (iLiteralPassword)
		LogText(_L8("Password contains unusual characters: using literal for password"));
#endif
	SetActive();
	}

// Queue a disconnection
void CImImap4Session::DisconnectL(TRequestStatus& aRequestStatus)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Disconnect"))));
	Cancel();

	Queue(aRequestStatus);

	// What are we doing at the moment?
	if (iState<EImapStateNoSelect)
		{
		DoDisconnect();
                DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::DisconnectL(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));

		Complete(KErrNone);
		}
	else if (IsIdling())
		{
		iDisconnectAfterIdleStopped = ETrue;
		DoStopIdleL();
		}
	else
		{
		DoDisconnectL();
		}
	}

void CImImap4Session::DoDisconnectL()
	{
	// Send logout command
	iState=EImapStateLogoutWait;
	SendMessageL(KIMAPC_LOGOUT);
	}

// Are we connected?
TBool CImImap4Session::Connected()
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Connected?"))));

	if (iState>=EImapStateNoSelect)
		return(ETrue);
	return(EFalse);
	}

// Are we busy?
TBool CImImap4Session::Busy()
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Busy?"))));

	if (iState==EImapStateDisconnected ||
		iState==EImapStateNoSelect ||
		iState==EImapStateSelected ||
		iState==EImapStateIdling)
		return(EFalse);
	return(ETrue);
	}

// Setting of PrimarySession, Going to be set on the secondary session.
void CImImap4Session::SetPrimarySession(CActiveWrapper* aPrimarySession)
	{
	iPrimarySession=aPrimarySession;
	}

// Return of current textserversession
CImTextServerSession* CImImap4Session::GetTextServerSession()
	{
	return iImapIO->GetTextServerSession();
	}

// Return the service settings
CImImap4Settings* CImImap4Session::ServiceSettings()
	{
	// Return them
	return(iServiceSettings);
	}

// List folder structure
void CImImap4Session::ListL(TRequestStatus& aRequestStatus, const TMsvId aFolder, CArrayPtr<CImImap4DirStruct>* aList)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND List(%x)"),aFolder)));
	__ASSERT_DEBUG(iState==EImapStateNoSelect || iState==EImapStateSelected,gPanic(ESelectWhenNotReady));
	if(!(iState==EImapStateNoSelect || iState==EImapStateSelected))
		{
		User::LeaveIfError(KErrNotReady);// Select when not ready
		}
	Queue(aRequestStatus);

	// Form the path
	HBufC8* path=NULL;
	TRAPD(err,path=MakePathL(aFolder,ETrue));
	if (err!=KErrNone)
		{
                DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::ListL(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));

		Complete(err);
		return;
		}
	CleanupStack::PushL(path);

	// Empty path? If not, append hierarchy separator
	if (path->Length())
		{
		// Get more space
		HBufC8 *rpath=path->ReAllocL(path->Length()+1);

		// Moved?
		if (path!=rpath)
			{
			// Get rid of old one and push new one
			CleanupStack::Pop();
			CleanupStack::PushL(path=rpath);
			}

		path->Des().Append(iHierarchySeparator);
		}

	// Save path, as we take this off replies to get the leaf: we need to
	// do this *before* we quote it as replies will be unquoted by the
	// time we process them.
	iCommandBuf.Copy(*path);

	// Quote it
	DoQuoteL(path);

	// Send the command
	NewTag();	
	iImapIO->SendL(iStatus,_L8("%d LIST \"\" \"%S%%\"\r\n"),iTag,path);
	NewTagSent();

	// Save list pointer to add to, and reset it
	iList=aList;
	iList->ResetAndDestroy();
	
	// Dispose of path
	CleanupStack::PopAndDestroy();

	// Save last state (selected/unselected) for restoring afterwards
	iSavedState=iState;
	iState=EImapStateListWait;
	}
	
// Update subscribed bits on local folder structure
// Local structure must have been refreshed before this is used:
// if folders listed in the LSUB reply don't exist, they're ignored
// silently.
void CImImap4Session::LsubL(TRequestStatus& aRequestStatus)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Lsub"))));
	__ASSERT_DEBUG(iState==EImapStateNoSelect || iState==EImapStateSelected,gPanic(ESelectWhenNotReady));
	if(!(iState==EImapStateNoSelect || iState==EImapStateSelected))
		{
		User::LeaveIfError(KErrNotReady);
		}
	
	Queue(aRequestStatus);

	// First, we need to go through the entire service, resetting all
	// the 'remote subscribed' flags.
	ResetSubscriptionFlagsL(iServiceId);

	// Build a buffer to quote the folder path (it may be necessary)
	HBufC8* path=HBufC8::NewL(iFolderPath.Length()+1);
	CleanupStack::PushL(path);
	path->Des().Append(iFolderPath);
	if (iFolderPath.Length())
		path->Des().Append(iHierarchySeparator);

	// Quote it
	DoQuoteL(path);

	// Send the command to list all of the folders in the tree: we
	// can't do it hierarchically, as the servers aren't clever enough
	// to tell us at parent levels about folders that contain subscribed
	// children. Pah.
	NewTag();

	// Make a Des & send it
	TPtrC8 pathdes(path->Des());
	iImapIO->SendL(iStatus,_L8("%d LSUB \"\" \"%S*\"\r\n"),
				  iTag,&pathdes);
	NewTagSent();

	// Clear up
	CleanupStack::PopAndDestroy();

	// Wait for reply
	iSavedState=iState;
	iState=EImapStateLsubWait;
	}


// Create a mailbox or folder
void CImImap4Session::Create(TRequestStatus& aRequestStatus, const TMsvId aParent, const TDesC& aLeafName, const TBool aFolder)
	{
	TInt err=KErrNone;
	if (!Connected())
		{
		Queue(aRequestStatus);
		err=KErrDisconnected;
		}
	else
		TRAP(err,CreateL(aRequestStatus, aParent, aLeafName, aFolder));
	if (err!=KErrNone)
                {
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::Create(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));

		Complete(err);
                }
	}

// Create a mailbox or folder
void CImImap4Session::CreateL(TRequestStatus& aRequestStatus, const TMsvId aParent, const TDesC& aLeafName, const TBool aFolder)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Create(%x,%S,%d)"),aParent,&aLeafName,aFolder)));
	__ASSERT_DEBUG(iState==EImapStateNoSelect || iState==EImapStateSelected,gPanic(ECreateWhenNotReady));
	if(!(iState==EImapStateNoSelect || iState==EImapStateSelected))
		{
		User::LeaveIfError(KErrNotReady);
		}
	Queue(aRequestStatus);

	// Make the path
	iSavedState=iState;
	iState=EImapStateCreateWait;
	HBufC8 *path=NULL; // To stop .AER warnings...
	TRAPD(err,path=MakePathL(aParent,ETrue));
	if (err!=KErrNone)
		{
                DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::CreateL(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));

		Complete(err);
		return;
		}
	CleanupStack::PushL(path);

	// Increase length of buffer for full name
	TInt encodedLeafMaxSize = 2+aLeafName.Size()*4/3 + 1;
	HBufC8* rpath=path->ReAllocL(path->Length()+2+encodedLeafMaxSize);
	
	// Moved?
	if (rpath!=path)
		{
		// Destroy old one and push new one
		CleanupStack::Pop();
		CleanupStack::PushL(path=rpath);
		}

	// Path not blank? Put a separator in there
	if (path->Des().Length())
		path->Des().Append(iHierarchySeparator);

	iCharConv->PrepareToConvertToFromOurCharsetL(KCharacterSetIdentifierImapUtf7);

	// Add leafname, via the utf7 encoder
	HBufC8* utf7=HBufC8::NewL(encodedLeafMaxSize);
	CleanupStack::PushL(utf7);
	
	TInt numUC, indexUC;
	TPtr8 des = utf7->Des();
	iCharConv->ConvertFromOurCharsetL(aLeafName, des, numUC, indexUC);
	path->Des().Append(des);

	CleanupStack::PopAndDestroy(); // utf7

	// Put a trailing hierarchy separator on there too if necessary
	if (aFolder)
		path->Des().Append(iHierarchySeparator);

	// Quote it if necessary
	DoQuoteL(path);
	TPtrC8 pathdes(path->Des());

	// Creating a folder
	NewTag();
	iImapIO->SendL(iStatus,_L8("%d CREATE \"%S\"\r\n"),iTag,&pathdes);
	NewTagSent();

	// Save what type of create we were doing
	iCommandFlags[0]=aFolder;
	iCommandIds[0]=aParent;
	iCommandBuf=aLeafName;

	// Free memory
	CleanupStack::PopAndDestroy();
	}

// Rename a mailbox
void CImImap4Session::Rename(TRequestStatus& aRequestStatus, const TMsvId aTarget, const TDesC& aNewName)
	{
	TInt err=KErrNone;
	if (!Connected())
		{
		Queue(aRequestStatus);
		err=KErrDisconnected;
		}
	else
		TRAP(err,RenameL(aRequestStatus, aTarget, aNewName));
	if (err!=KErrNone)
		Complete(err);
	}

// Rename a mailbox
void CImImap4Session::RenameL(TRequestStatus& aRequestStatus, const TMsvId aTarget, const TDesC& aNewName)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Rename(%x,%S)"),aTarget,&aNewName)));
	__ASSERT_DEBUG(iState==EImapStateNoSelect || iState==EImapStateSelected,gPanic(ERenameWhenNotReady));
	if(!(iState==EImapStateNoSelect || iState==EImapStateSelected))
	{
		User::LeaveIfError(KErrNotReady);
	}
	
	Queue(aRequestStatus);

	// Make the paths
	HBufC8* path=NULL; // To stop .AER warnings...
	TRAPD(err,path=MakePathL(aTarget,ETrue));
	if (err!=KErrNone)
		{
		Complete(err);
		return;
		}
	CleanupStack::PushL(path);

	DoQuoteL(path);

	HBufC8* newpath=NULL; // To stop .AER warnings...
	TRAP(err,newpath=MakePathL(aTarget,EFalse));
	if (err!=KErrNone)
		{
		CleanupStack::PopAndDestroy();
		Complete(err);
		return;
		}
	CleanupStack::PushL(newpath);

	// Extend buffer for new name
	TInt encodedLeafMaxSize = 2+aNewName.Size()*4/3 + 1;
	HBufC8* rnewpath=newpath->ReAllocL(newpath->Length()+2+encodedLeafMaxSize);

	// Moved?
	if (rnewpath!=newpath)
		{
		// Destroy old one and push new one
		CleanupStack::Pop();
		CleanupStack::PushL(newpath=rnewpath);
		}

	if (newpath->Length())
		newpath->Des().Append(iHierarchySeparator);
	
	iCharConv->PrepareToConvertToFromOurCharsetL(KCharacterSetIdentifierImapUtf7);

	// Add leafname, via the utf7 encoder
	HBufC8* utf7=HBufC8::NewL(encodedLeafMaxSize);
	CleanupStack::PushL(utf7);
	
	TInt numUC, indexUC;
	TPtr8 des = utf7->Des();
	iCharConv->ConvertFromOurCharsetL(aNewName, des, numUC, indexUC);
	newpath->Des().Append(des);

	CleanupStack::PopAndDestroy(); // utf7

	DoQuoteL(newpath);

	// Save rename parameters
	iCommandIds[0]=aTarget;
	iCommandBuf=aNewName;			// this is still the original, unencoded name

	// Set states	
	iSavedState=iState;
	iState=EImapStateRenameWait;

	// Send the command
	NewTag();
	TPtrC8 pathdes(path->Des());
	TPtrC8 newpathdes(newpath->Des());
	iImapIO->SendL(iStatus,_L8("%d RENAME \"%S\" \"%S\"\r\n"),iTag,&pathdes,&newpathdes);
	NewTagSent();

	// Free memory
	CleanupStack::PopAndDestroy(2);
	}

// Delete a message/mailbox
void CImImap4Session::Delete(TRequestStatus& aRequestStatus, const CMsvEntrySelection& aTargetSel)
	{
	TInt err=KErrNone;
	if (!Connected())
		err=KErrDisconnected;
	else
		TRAP(err,DeleteL(aRequestStatus,aTargetSel));
	if (err!=KErrNone)
		{
		Queue(aRequestStatus);
		Complete(err);
		}
	}

// Delete a message/mailbox
void CImImap4Session::Delete(TRequestStatus& aRequestStatus, const TMsvId aTarget)
	{
	TInt err=KErrNone;
	if (!Connected())
		err=KErrDisconnected;
	else
		TRAP(err,DeleteL(aRequestStatus,aTarget));
	if (err!=KErrNone)
		{
		Queue(aRequestStatus);
		Complete(err);
		}
	}
void CImImap4Session::DeleteEntryL( const TMsvId aTarget)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Delete(%x)"),aTarget)));

	// Move to the entry in question
	SetEntryL(aTarget);

	LOG_COMMANDS((	LogText(_L8("COMMAND Delete(message)"))));
	__ASSERT_DEBUG(iState==EImapStateSelected,gPanic(EDeleteWhenNotReady));	
	if(!(iState==EImapStateSelected))
	{
		User::LeaveIfError(KErrNotReady);
	}
	
	// SJM, remove check for right mailbox as we may be trying to
	// delete a moved entry which is in fact no longer in the
	// right mailbox.
#if 0
	// Check we're in the right mailbox
	if (iEntry->Entry().Parent()!=iMailboxId)
		{
		// Nope.
		Queue(aRequestStatus);
		Complete(KErrImapWrongFolder);
		return;
		}
#endif
	
	// Set deleted flag on this entry
	TMsvEmailEntry entry=iEntry->Entry();
	entry.SetDeletedIMAP4Flag(ETrue);
	ChangeEntryL(entry);
	}

void CImImap4Session::DeleteL(TRequestStatus& aRequestStatus, const CMsvEntrySelection& aTargetSel)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Delete (%x)"),aTargetSel[0])));

	// Move to the entry in question
	SetEntryL(aTargetSel[0]);

    CMsvEntrySelection* sel=aTargetSel.CopyL();
	delete iSelection;
	iSelection=sel;

	// Only deleting message seletion currently
	if (iEntry->Entry().iType==KUidMsvMessageEntry)
		{
		// Set delete flag on all selected entries.
		TInt count=iSelection->Count();
		while (count--)
			DeleteEntryL((*iSelection)[count]);

		// Force a folder close with expunge
		CloseL(aRequestStatus,ETrue);
		}
	else
		{
		LOG_COMMANDS((	LogText(_L8("COMMAND Delete - Can only delete selection of Messages"))));

		// Deleting selection of entries whicxh are not messages
		Queue(aRequestStatus);
		Complete(KErrNotSupported);
		}
	}

void CImImap4Session::DeleteL(TRequestStatus& aRequestStatus, const TMsvId aTarget)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Delete(%x)"),aTarget)));

	// Move to the entry in question
	SetEntryL(aTarget);

	// A message?
	if (iEntry->Entry().iType==KUidMsvMessageEntry)
		{
		DeleteEntryL(aTarget);

		// Temporary: force a folder close with expunge
		CloseL(aRequestStatus,ETrue);
		}
	// A folder?
	else if (iEntry->Entry().iType==KUidMsvFolderEntry)
		{
		LOG_COMMANDS((	LogText(_L8("COMMAND Delete(folder)"))));
                __ASSERT_DEBUG(iState==EImapStateNoSelect,gPanic(EDeleteWhenNotReady));
		if(!(iState==EImapStateNoSelect))
			{
			User::LeaveIfError(KErrNotReady);
			}
		
		Queue(aRequestStatus);

		// Save IDs of parent and target for actually doing the local delete when
		// the remote one completes successfully.
		iCommandIds[0]=iEntry->Entry().Parent();
		iCommandIds[1]=aTarget;

		// Get path to delete
		HBufC8* path=NULL; // To stop .AER warnings...
		path=MakePathL(aTarget,ETrue);
		CleanupStack::PushL(path);
		DoQuoteL(path);

		// Set state
		iSavedState=iState;
		iState=EImapStateDeleteWait;

		// Send command
		NewTag();
		TPtrC8 pathdes(path->Des());
		iImapIO->SendL(iStatus,_L8("%d DELETE \"%S\"\r\n"),iTag,&pathdes);
		NewTagSent();

		// Destroy buffer
		CleanupStack::PopAndDestroy();
		}
	// Something else?
	else
		{
		LOG_COMMANDS((	LogText(_L8("COMMAND Delete(unknown)"))));

		// Delete of something that isn't a folder or a message. Erk!
		Queue(aRequestStatus);
		Complete(KErrNotSupported);
		}
	}

// Delete everything in this folder
void CImImap4Session::DeleteAllMessagesL(TRequestStatus& aRequestStatus)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND DeleteAllMessages(%x)"),iMailboxId)));

	// We have to be selected
	__ASSERT_DEBUG(iState==EImapStateSelected,gPanic(EDeleteWhenNotReady));
	if(!(iState==EImapStateSelected))
		{
		User::LeaveIfError(KErrNotReady);
		}
	
	Queue(aRequestStatus);

	// We're going to send a command of one style or another
	NewTag();

	// Are there any messages remotely to delete?
	if (iMailboxSize==0)
		{
		// No: just do a close
		iState=EImapStateCommandWait;
		iSavedState=EImapStateNoSelect;
		iImapIO->SendL(iStatus,_L8("%d CLOSE\r\n"),iTag);
		}
	else
		{
		// DeleteAllMessages is a special case: we want to delete everything,
		// regardless of wether it's in the mirror or not. So, we set deleted
		// flags on everything then expunge the folder
		iState=EImapStateDeleteAllWait;

		// Send command: we go into deleteall wait as the next
		iImapIO->SendL(iStatus,_L8("%d STORE 1:* +FLAGS (\\Deleted)\r\n"),iTag);
		}

	// Sent it
	NewTagSent();
	}

// Select a folder
void CImImap4Session::Select(TRequestStatus& aRequestStatus, const TMsvId aFolder, const TBool aReadWrite)
	{
	TInt err=KErrNone;
	if (!Connected())
		{
		Queue(aRequestStatus);
		err=KErrDisconnected;
		}
	else
		TRAP(err,SelectL(aRequestStatus, aFolder, aReadWrite));
	if (err!=KErrNone)
                {
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::Select(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));

		Complete(err);
                }
	}

void CImImap4Session::SelectL(TRequestStatus& aRequestStatus, const TMsvId aFolder, const TBool aReadWrite)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Select(%x (rw=%d), in state %d. Current mailbox=%x)"),aFolder,aReadWrite?1:0,iState,iMailboxId)));

	if (!(iState==EImapStateNoSelect || iState==EImapStateSelected))
		{
		User::LeaveIfError(KErrNotReady);
		}
	Queue(aRequestStatus);
	
	// reset counts to safe values here to avoid reporting left over
	// values from previous fetch. Correct values will be set up once
	// headers have been fetched and parts counted.
	iProgress.iPartsToDo=iProgress.iBytesToDo=1;
	iProgress.iPartsDone=iProgress.iBytesDone=0;
	
	// Do the select

	// Is it already selected and the read/write state is compatible?
	// Skip the command if possible!
	if (iMailboxId==aFolder && iState==EImapStateSelected &&
		((aReadWrite && iMailboxWritable) || !aReadWrite))
		{
		if (ImapIdleSupported())
			{
                        DBG((LogText(_L8("-----------------------------------------------------------"))));
			DBG((LogText(_L8("CImap4Session::SelectL(): calling Complete()"))));
			DBG((LogText(_L8("-----------------------------------------------------------"))));

			Complete(KErrNone);
			return;
			}
		else
			{
			// Just NOOP it so that we know about any mailbox size changes
			NewTag();
			iImapIO->SendL(iStatus,_L8("%d NOOP\r\n"),iTag);
			iState=EImapStateSelectWait;
			NewTagSent();
			return;
			}
		}
	
	// Ok looks as if the SELECT actually needs to be done.
	DoSelectL(aFolder, aReadWrite);
	}

void CImImap4Session::DoSelectL(const TMsvId aFolder, const TBool aReadWrite)
	{
	NewTag();	

	// We should always get an EXISTS after a SELECT but just in case
	// we will force it true here. This ensures that a NewOnlySync
	// will always do the sync when it involves selecting a new folder
	iMailboxReceivedExists=ETrue;

	// Store name of new mailbox
	iMailboxId=aFolder;

	// Get rid of old index and reset everything
	iFolderIndex.Reset();
	iMailboxSize=0;
	iMsgsDone=0;
	iMailboxRecent=0;
	iUidValidity=0;
	iUidNext=0;

	// Is it special-case inbox?
	SetEntryL(iMailboxId);
#if 0
	if (iEntry->Entry().Parent()==iServiceId && iEntry->Entry().iDetails.CompareF(KIMAP_INBOX)==0)
		{
		// Inbox: no path prepended
		iImapIO->SendL(iStatus,aReadWrite?_L8("%d SELECT INBOX\r\n"):_L8("%d EXAMINE INBOX\r\n"),iTag);
		iMailboxIsInbox=ETrue;
		}
	else
		{
#endif
		// Create path and send select command
		HBufC8* path=MakePathL(iMailboxId,ETrue);
		CleanupStack::PushL(path);
		DoQuoteL(path);
		TPtrC8 pathptr=path->Des();
		iImapIO->SendL(iStatus,aReadWrite?_L8("%d SELECT \"%S\"\r\n"):_L8("%d EXAMINE \"%S\"\r\n"),iTag,&pathptr);
		CleanupStack::PopAndDestroy();
#if 0
		iMailboxIsInbox=EFalse;
		}
#endif
	// Sent command
	iState=EImapStateSelectWait;
	NewTagSent();
	}

// Copy a message to a new folder
void CImImap4Session::Copy(TRequestStatus& aRequestStatus, const TMsvId aSource, const TMsvId aDestination, TBool aUnSelectIfSameFolder)
	{
	TInt err=KErrNone;
	if (!Connected())
		{
		Queue(aRequestStatus);
		err=KErrDisconnected;
		}
	else
		TRAP(err,CopyL(aRequestStatus, aSource, aDestination, aUnSelectIfSameFolder));
	if (err!=KErrNone)
                {
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::Copy(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));

		Complete(err);
                }
	}

void CImImap4Session::CopyL(TRequestStatus& aRequestStatus, const TMsvId aSource, const TMsvId aDestination, TBool aUnSelectIfSameFolder)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Copy(%x,%x)"),aSource,aDestination)));
	__ASSERT_DEBUG(iState==EImapStateSelected,gPanic(ECopyWhenNotSelected));
	if(!(iState==EImapStateSelected))
		{
		User::LeaveIfError(KErrArgument);//Copy when not selected
		}
	
	Queue(aRequestStatus);

	// Make destination folder path
	HBufC8 *command=NULL; // To stop .AER warnings...
	TRAPD(err,command=MakePathL(aDestination,ETrue));
	if (err!=KErrNone)
		{
                DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::CopyL(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));

		Complete(err);
		return;
		}
	CleanupStack::PushL(command);
	DoQuoteL(command);

	// Check that source is in the folder we have got selected: this also
	// ensures it's actually a message, as the parent of attachments wouldn't
	// be the folder we have selected...
	SetEntryL(aSource);
	__ASSERT_DEBUG(iEntry->Entry().Parent()==iMailboxId,gPanic(ECopyNotFromSelectedFolder));
	if(!(iEntry->Entry().Parent()==iMailboxId))
			{
			User::LeaveIfError(KErrGeneral);
			}
	// Make command
	TMsvEmailEntry entry=iEntry->Entry();
	NewTag();
	TPtrC8 commanddes(command->Des());
	iImapIO->SendL(iStatus,_L8("%d UID COPY %u \"%S\"\r\n"),iTag,entry.UID(),&commanddes);
	NewTagSent();

	// If we're copying to the currently selected folder, pretend we've unselected it.
	// This ensures we'll pick up on any changes in the next new sync
	if (aUnSelectIfSameFolder && iMailboxId==aDestination && iState==EImapStateSelected)
		{
		// Return to 'no select' state after this
		iSavedState=EImapStateNoSelect;
		}
	else
		{
		// Save existing selected/unselected state
		iSavedState=iState;
		}

	// Set up state
	iState=EImapStateCommandWait;

	// Delete buffer
	CleanupStack::PopAndDestroy();
	}

// Copy a message to a new folder
void CImImap4Session::Append(TRequestStatus& aRequestStatus, const TMsvId aSource, const TMsvId aDestination)
	{
	TInt err=KErrNone;
	if (!Connected())
		{
		Queue(aRequestStatus);
		err=KErrDisconnected;
		}
	else
		TRAP(err,AppendL(aRequestStatus, aSource, aDestination));
	if (err!=KErrNone)
		Complete(err);
	}
	
void CImImap4Session::AppendL(TRequestStatus& aRequestStatus, const TMsvId aSource, const TMsvId aDestination)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Append(%x,%x)"),aSource,aDestination)));

	Queue(aRequestStatus);

	// Check that source is a complete message
	SetEntryL(aSource);

	// Check it's a message
	if (iEntry->Entry().iType!=KUidMsvMessageEntry)
		{
		// Can't do it!
		Complete(KErrGeneral);
		return;
		}

	// Cancel any dummy operation that might be outstanding
	if (ImapIdleSupported()==EFalse)
		{
		CancelDummy();
		}

	// Size message for sending using a CImCalculateMessageSize
	delete iMessageSizer;
	iMessageSizer=NULL;
	iMessageSizer=CImCalculateMsgSize::NewL(iFs,*iEntry);

	// Start sizing operation, using MIME: save source & destination for next step
	// Use iHost (hostname of remote server) as the domain name seed for the MsgId
	iCommandIds[0]=aSource;
	iCommandIds[1]=aDestination;
	iMessageDate=iEntry->Entry().iDate;
	iMessageSizer->StartL(iStatus,iCommandIds[0], ESendAsMimeEmail, 
												  iMessageDate, iHost, iCharset);

	// If we're appending to the currently selected folder, pretend we've unselected it.
	// This ensures we'll pick up on any changes in the next new sync
	if (iMailboxId==aDestination && iState==EImapStateSelected)
		{
		// Return to 'no select' state after this
		iSavedState=EImapStateNoSelect;
		}
	else
		{
		// Save existing selected/unselected state
		iSavedState=iState;
		}

	iState=EImapStateAppendSizeWait;
	if (!IsActive()) SetActive();
	}

// Close a selected folder
void CImImap4Session::Close(TRequestStatus& aRequestStatus, const TBool aExpunge)
	{
	TInt err=KErrNone;
	if (!Connected())
		{
		Queue(aRequestStatus);
		err=KErrDisconnected;
		}
	else
		TRAP(err,CloseL(aRequestStatus, aExpunge));
	if (err!=KErrNone)
		Complete(err);
	}

void CImImap4Session::CloseL(TRequestStatus& aRequestStatus, const TBool aExpunge)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Close(%d)"),aExpunge)));

	Queue(aRequestStatus);

	// Folder currently open? Just complete if we're not selected
	if (iState==EImapStateNoSelect)
		{
		Complete(KErrNone);
		return;
		}

	// Expunging? If not, just send close command
	if (!aExpunge)
		{
		// Send close
		SendMessageL(KIMAPC_CLOSE);
		iSavedState=EImapStateNoSelect;
		iState=EImapStateCommandWait;
		}
	else
		{
		// The deletion strategy will build up runs of UIDs in order to make the
		// most efficient use of bandwidth. However, as UIDs are not necessarily
		// contiguous, so we use their sorted position in the mirror to decide on
		// runs. UIDs will be allocated on an increasing basis,as the server cannot
		// have magically invented new UIDs between 2 existing ones.
		// UIds can miss from run by using email sync limits.
		//
		// We use TInt64's because AppendNum() cannot take a TUint32, which is
		// what a UID is.
		SetEntryL(iMailboxId);
		GetChildrenL(*iSelection);
		TRAPD(err,MakeSortedFolderIndexL());
		if (err!=KErrNone)
			{
			Complete(err);
			return;
			}
		TInt pos=0;
		TInt run=0;
		TInt64 last=0;
		TInt deleted=0;

		// Build command
		HBufC8* command=HBufC8::NewLC(256);

		// Start command
		command->Des().Append(_L8("UID STORE "));

		while(pos<iFolderIndex.Size())
			{
			// Look for messages with deleted flag set
			SetEntryL(iFolderIndex[pos].iMsvId);
			if (((TMsvEmailEntry)iEntry->Entry()).DeletedIMAP4Flag())
				{
				LOG_COMMANDS((LogText(_L8("Message #%d marked as deleted"),pos+1)));

				deleted++;
				// If uids are missing from run
				if ((pos > 0 )&& (((TUint)iFolderIndex[pos].iUid - (TUint)iFolderIndex[pos-1].iUid) > 1))
				{
				// Breaking a run
				if(run > 1)
					{
					// A run of at least 2 is a range. Append 'last' UID,
					// after removing comma and append a comma 
					command->Des().Delete(command->Des().Length()-1,1);
					command->Des().Append(_L8(":"));
					command->Des().AppendNum(last);
					command->Des().Append(_L8(","));
					}
				// run broken
				run = 0;
				}

				// This one is deleted. Are we in a run?
				if (!run)
					{
					// No, start of a run/single item. Add to command
					// Enough room for this?
					if ((command->Length()+32)>command->Size())
						{
						// Extend buffer
						HBufC8* rcommand=command->ReAllocL(command->Size()+64);
						
						// Moved?
						if (rcommand!=command)
							{
							// Destroy old one and push new one
							CleanupStack::Pop();
							CleanupStack::PushL(command=rcommand);
							}
						}

					// Single number, plus a comma to terminate
					TInt64 uid=(TUint)((TMsvEmailEntry)iEntry->Entry()).UID();
					command->Des().AppendNum(uid);
					command->Des().Append(_L8(","));

					// A run of 1 :-)
					run++;
					}
				else
					{
					// We're in a run already. Extend it: it will be terminated
					// when the run is broken or we exit.
					last=(TUint)iFolderIndex[pos].iUid;
					run++;
					}
				}
			else
				{
				// Mark this MsvId as 0 (so it will be kept in local expunge)
				iFolderIndex[pos].iMsvId=0;

				// Breaking a run?
				if (run>1)
					{
					// A run of at least 2 is a range. Append 'last' UID,
					// after removing comma
					command->Des().Delete(command->Des().Length()-1,1);
					command->Des().Append(_L8(":"));
					command->Des().AppendNum(last);
					command->Des().Append(_L8(","));
					}

				// Run broken
				run=0;
				}

			// Next message
			pos++;
			}

		// Anything deleted?
		if (deleted)
			{
			// Remove the last character in the command string
			command->Des().Delete(command->Des().Length()-1,1);	

			// A run to complete?
			if (run>1)
				{
				// A run of at least 2 is a range. Append 'last' UID.
				command->Des().Append(_L8(":"));
				command->Des().AppendNum(last);
				}

			// Append flags & send command
			command->Des().Append(_L8(" +FLAGS (\\Deleted)"));
			SendMessageL(command->Des());
			iState=EImapStateDeleteMarkWait;
			}
		else
			{
			// Nothing to do: Just close the folder
			iState=EImapStateCloseWait;
			SendMessageL(KIMAPC_CLOSE);
			}

		// Get rid of command buffer
		CleanupStack::PopAndDestroy();
		}
	}

// Orphan a local message
void CImImap4Session::OrphanMessageL(const TMsvId aMessage)
	{
	DBG((LogText(_L8("OrphanMessageL(%x)"),aMessage)));

	// We no longer orphan the messages based on whether they have been downloaded.
 	// That was just delaying the inevitable and causing other problems
 	DeleteMessageL(aMessage);
	DBG((LogText(_L8("  Deleting message"))));
	}

// Delete a local message
void CImImap4Session::DeleteMessageL(const TMsvId aMessage)
	{
	DBG((LogText(_L8("CImImap4Session::DeleteMessageL(%x)"),aMessage)));

 	if(aMessage == KMsvNullIndexEntryId)
		{
		DBG((LogText(_L8("Attempted delete of null entry(%d)"),aMessage)));
 		return;
		}
	// Delete message and all subparts: first, move to parent
	DBG((LogText(_L8("  SetEntry(%x)"),aMessage)));
	SetEntryL(aMessage);

	DBG((LogText(_L8("  SetEntry(%x)"),iEntry->Entry().Parent())));
	SetEntryL(iEntry->Entry().Parent());

	// Do it
	DBG((LogText(_L8("  About to DeleteEntry(%x)"),aMessage)));
	// Do not leave when entry is in use 
	TInt err (iEntry->DeleteEntry(aMessage));
	if(err==KErrInUse)
		{
		DBG((LogText(_L8("CImImap4Session::DeleteMessageL() dont leave if err = KErrInUse"))));
		}
	else
		{
		User::LeaveIfError(err);
		}
	
	DBG((LogText(_L8("  Done!"))));
	}

// Get MESSAGE ONLY children of a folder. Ignore shadows as they are
// not going to be synced against the server
void CImImap4Session::GetMessageChildrenL(const TMsvId aFolder, CMsvEntrySelection* aChildren)
	{	
	// Get *all* the children
	SetEntryL(aFolder);
	GetChildrenL(*aChildren);

	if(iCachedEntryData)
		{
		delete iCachedEntryData;
		iCachedEntryData = 0;
		}
	iCachedEntryData = new(ELeave) CArrayFixFlat<TMsvCacheData>(5);

	// Go through them, checking to see if they're messages and removing ones that aren't
	TInt pos=0;
	while(pos<aChildren->Count())
		{
		TMsvEntry* entryPtr;
		TMsvId id = (*aChildren)[pos];
		User::LeaveIfError(iEntry->GetEntryFromId(id,entryPtr));

		// Is it a message? And is it real (not shadow)
		if (entryPtr->iType!=KUidMsvMessageEntry ||
			entryPtr->iRelatedId != KMsvNullIndexEntryId )
			{
			// No, remove it
			aChildren->Delete(pos,1);
			}
		else
			{
			//cache two parts of the TMsvEntry data to avoid having to refind it later
			TMsvCacheData data;
			data.iOrphan = ((TMsvEmailEntry)(*entryPtr)).Orphan();
			data.iUid = ((TMsvEmailEntry)(*entryPtr)).UID();
			iCachedEntryData->AppendL(data);
			// Next entry
			pos++;
			}
		}
	}

// Synchronise a folder
void CImImap4Session::Synchronise(TRequestStatus& aRequestStatus, TBool aNewOnly)
	{
        DBG((LogText(_L8("CImImap4Session::Synchronise()"))));
	TInt err=KErrNone;
	if (!Connected())
		{
		Queue(aRequestStatus);
		err=KErrDisconnected;
		}
	else
		TRAP(err,SynchroniseL(aRequestStatus, aNewOnly));
	if (err!=KErrNone)
                {
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::Synchronise(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));


		Complete(err);
	        }
       }

void CImImap4Session::SynchroniseL(TRequestStatus& aRequestStatus, TBool aNewOnly)
	{
	__ASSERT_DEBUG(iState==EImapStateSelected,gPanic(ESyncWhenNotSelected));
	if(!(iState==EImapStateSelected))
		{
		User::LeaveIfError(KErrArgument);// Sync when not selected .
		}
	
	Queue(aRequestStatus);

	DoSynchroniseL(aNewOnly);
	}

void CImImap4Session::DoSynchroniseL(TBool aNewOnly)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND Synchronise"))));
	__ASSERT_DEBUG(iState==EImapStateSelected,gPanic(ESyncWhenNotSelected));
	if(!(iState==EImapStateSelected))
			{
			User::LeaveIfError(KErrArgument);// Sync when not selected .
			}
	// clear flags that may have been be set by SELECT or NOOP to say
	// that a sync is required
	iMailboxReceivedExists=EFalse;
	iMailboxReceivedExpunge=EFalse;
	iMailboxReceivedFlags=EFalse;

	// Some pre-bits that need doing - get the children & count them
	SetEntryL(iMailboxId);

	TMsvEmailEntry message=iEntry->Entry();
	GetMessageChildrenL(iMailboxId,iSelection);
	TInt noofchildren=iSelection->Count();

	if (!aNewOnly && noofchildren)
		{
		// Delete any orphaned messages completely at this point
		DBG((LogText(_L8("Looking for orphans in %d local messages"),noofchildren)));

		TInt pos=0;
		while(pos<noofchildren)
			{
			if((*iCachedEntryData)[pos].iOrphan)
				{
				DBG((LogText(_L8("  Deleting orphan %x"),(*iSelection)[pos])));

				// Delete it
				SetEntryL(iMailboxId);
				iEntry->DeleteEntry((*iSelection)[pos]);				
				// Remove it from selection
				iSelection->Delete(pos,1);
				noofchildren--;
				}
			else
				{
				// Move on to next entry
				pos++;
				}
			}
		}

	// First thing we have to do: check the UIDVALIDITY of the mirror and the
	// remote folder match. If not, we have to orphan everything in the mirror
	// and start again.
	// We also do this if there are 0 messages in the remote mailbox (0 EXISTS)
	// and there are messages locally
	if (!message.ValidUID() || iUidValidity!=message.UID() || iMailboxSize==0)
		{
		// They don't match: do we have local children?
#ifdef PRINTING
		if (!iMailboxSize)
			LogText(_L8("No remote messages"));
		else
			LogText(_L8("UIDVALIDITY changed: local %u, remote %u"),
			    message.UID(),iUidValidity);
#endif

		// If we were doing a new-only sync, change this to a full sync as the
		// UIDVALIDITY shows major changes
		aNewOnly=EFalse;

		if (noofchildren)
			{
		    // We've got local children: orphan them
			DBG((LogText(_L8("Orphaning %d local messages"),noofchildren)));

			for(TInt a=0;a<noofchildren;a++)
				{
				// ...we should be skipping locally generated messages
				OrphanMessageL((*iSelection)[a]);
				}

			// Reget the number of children as this may have changed due to
			// the orphaning process
			GetMessageChildrenL(iMailboxId,iSelection);
			noofchildren=iSelection->Count();
			}

		// Now, we match the remote's UIDVALIDITY: reset the pointer as it may
		// well have been used by the orphaning process above.
		SetEntryL(iMailboxId);
		if (message.UID()!=iUidValidity || !message.ValidUID())
			{
			// Do the change if necessary
			message.SetUID(iUidValidity);
			message.SetValidUID(ETrue);
			ChangeEntryBulkL(message);
			}
		}

	// We've processed none of the remote messages yet
	iMsgsDone=0;

	// Any remote messages? If not, complete now as there's nothing else to do
	if (iMailboxSize==0)
		{
		// This folder is now sync'ed
		// No need to set seen flags as no messages in remote mailbox
		SyncCompleteL(); 
		DBG((LogText(_L8("CImap4Session::DoSynchroniseL(): iMailboxSize=0, folder now synched"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::DoSynchroniseL(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));

		Complete(KErrNone);
		return;
		}

	// Start the synchronise with sync'ing old messages: are there any
	// messages in our mirror folder?
	DBG((LogText(_L8("CImImap4Session::DoSynchroniseL(): Setting iState to EImapStateSynchroniseWait"))));
 	iState=EImapStateSynchroniseWait;
	iSomeUnread=EFalse;
	iHighestUid=0;

	// Zero the "missing" message range limits.
	iMissingUidLow=0;
	iMissingUidHigh=0;

	// Clear RX byte counter
	iImapIO->RXbytes(ETrue);

	// Any children?
	iFolderIndex.Reset();
	if (noofchildren>0) 
		{
		// Children exist, we need to do an old-sync to check all the messages
		// are still there.
		
		// Build an index of UIDs/TMsvIds currently in the mirror folder, and
		// sort this by UID: this is the order in which we expect the fetch to
		// return UIDs - any missing have been deleted on the server. They may
		// well not be in UID order in the index because locally-appended
		// messages will not have been added to the index in UID order.
		TRAPD(err,MakeSortedFolderIndexL(ETrue));
		if (err!=KErrNone)
			{
			DBG((LogText(_L8("CImap4Session::DoSynchroniseL(): children exist, need to do old sync"))));
			DBG((LogText(_L8("-----------------------------------------------------------"))));
			DBG((LogText(_L8("CImap4Session::DoSynchroniseL(): calling Complete()"))));
			DBG((LogText(_L8("-----------------------------------------------------------"))));

			Complete(err);
			return;
			}

		// Find the highest UID in the index
		iHighestUid=iFolderIndex[noofchildren-1].iUid;
		}

	// Retrieve folder synchronisation limit.
	if (iEntry->Entry().Parent()==iServiceId && iEntry->Entry().iDetails.CompareF(KIMAP_INBOX)==0)
		{
		DBG((LogText(_L8("Folder sync of inbox folder"))));

		// Leave iSyncLimit at the maximum if a Search String is set
		// If no Search String is set and this is the inbox, then use the inbox sync limit.
		if(iServiceSettings->SearchString().Length() == 0)
			{
			iSyncLimit=iServiceSettings->InboxSynchronisationLimit();	
			}
		}
	else
		{
		// Otherwise use the folder sync limit.
		// Leave iSyncLimit at the maximum if a Search String is set
		DBG((LogText(_L8("Folder sync of non-inbox folder"))));
		
		if(iServiceSettings->SearchString().Length() == 0)
			{
			iSyncLimit=iServiceSettings->MailboxSynchronisationLimit();
			}
		}
		
	// Get the user defined UID SEARCH string if there is one
	// Do a refined search if there's a string
	if(iServiceSettings->SearchString().Length() != 0)
		{
		iSyncState=ESyncSearch;
		iFolderPosition=0;
		iSearchList->Reset();
		NewTag();
		// Refined search
		_LIT8(KSearchString,"%d UID SEARCH 1:%d %S\r\n");
		TPtrC8 ptr = iServiceSettings->SearchString();
		iImapIO->SendL(iStatus,KSearchString,iTag,Min(iMailboxSize,KImapUidSearchSize),&ptr);
		NewTagSent();
		return;
		}
	else // if no search string we use the old behaviour
	// Check the folder synchronisation limit.
	if (iSyncLimit>KImImapSynchroniseNone)
		{
		DBG((LogText(_L8("Folder sync limited to %d messages"),iSyncLimit)));

		// Limited folder synchronisation, perform a UID search.
		iSyncState=ESyncSearch;
		iFolderPosition=0;

		// Reset the search list.
		iSearchList->Reset();

		// Perform a UID search on this folder.
		NewTag();
		iImapIO->SendL(iStatus,_L8("%d UID SEARCH 1:%d\r\n"),iTag,Min(iMailboxSize,KImapUidSearchSize));
		NewTagSent();
		return;
		}
	else if (iSyncLimit==KImImapSynchroniseNone)
		{
		DBG((LogText(_L8("No folder sync required"))));

		// No synchronisation required.
		// This folder is now sync'ed
		SyncCompleteL();
		// Back to selected state
		iState=EImapStateSelected;
		iSyncState=ENotSyncing;
		DBG((LogText(_L8("CImap4Session::DoSynchroniseL(): iSyncLimit=KImImapSynchroniseNone, no sync required"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::DoSynchroniseL(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));
 
		Complete(KErrNone);
		return;
		}
	else if (iSyncLimit<=KImImapSynchroniseAll)
		{
		DBG((LogText(_L8("Full folder sync required"))));
   
		// Full synchronisation required - fall through.
		}

	if (noofchildren>0) 
		{
		if (!aNewOnly && iHighestUid>0)
			{
			// Do old sync
			iSyncState=ESyncOld;
			iFolderPosition=0;
			NewTag();
			// If a UID Search String is used it looks like this is FULL sync only 
			// so leave as is
			iImapIO->SendL(iStatus,_L8("%d UID FETCH 1:%d (UID FLAGS)\r\n"),
						  iTag,iHighestUid);
			NewTagSent();
			return;
			}
		}

	// Do new sync
	SynchroniseNewL();
	}

void CImImap4Session::ColonSeparatorToSpace(TDes8& buf)
	{
	TInt colon;
	while ((colon = buf.Locate(':')) != KErrNotFound)
		buf.Replace(colon,1,_L8(" "));
	buf.TrimRight();
	}

// Do second stage of sync with uid list, ie new synchronise.
void CImImap4Session::SynchroniseNewL(const TUint32 aLowUid,const TUint32 aHighUid)
	{
        DBG((LogText(_L8("CImImap4Session::SynchroniseNewL()"))));
 	iSyncState=ESyncNew;
	iCheckDiskSpaceCounter = 0;
	iFolderPosition = 0;

	// First, resize folder index to hold all messages in the folder,
	// as opposed to the old sync list. This will preserve the old
	// contents of the index, which is what we want as it's up-to-date
	// and correct.
	iFolderIndex.SetSizeL(iMailboxSize);
	
	DBG((LogText(_L8("Synchronising new messages (UIDs %u to %u)"),aLowUid,aHighUid)));

	// Create list of priority fields to request
	TBuf8<256> priorityFields;
	CDesC8ArrayFlat* array = new(ELeave) CDesC8ArrayFlat(4);
	CleanupStack::PushL(array);
	CImcvUtils::PriorityFieldsL(*array);
	for (TInt i=0; i<array->Count(); i++)
		{
		priorityFields.Append((*array)[i]);
		}
	CleanupStack::PopAndDestroy(array);
	ColonSeparatorToSpace(priorityFields);
	
	// Send command
	NewTag();

	// If a UID search string has been specified, the we should create the UID FETCH
	// string from the UID integer list.
	if(iServiceSettings->SearchString().Length() != 0)
		{
		CreateUidStringL();
		TPtrC8 ptr(iUidString->Des());
		iImapIO->SendL(iStatus,KImapFetchSmallHeaderRangeRefined,iTag,&ptr, &priorityFields);
		}
	else
		{
		iImapIO->SendL(iStatus,KImapFetchSmallHeaderRange,iTag,aLowUid,aHighUid, &priorityFields);
		}
	NewTagSent();
	}

// Do second stage of sync, ie new synchronise.
void CImImap4Session::SynchroniseNewL()
	{
	iSyncState=ESyncNew;
	iCheckDiskSpaceCounter = 0;
	iFolderPosition = 0;

	// First, resize folder index to hold all messages in the folder,
	// as opposed to the old sync list. This will preserve the old
	// contents of the index, which is what we want as it's up-to-date
	// and correct.
	iFolderIndex.SetSizeL(iMailboxSize);

	// fetch just the header of the new mails
	FetchHeaderL(iHighestUid+1);
	}

// Build the fetch list
void CImImap4Session::AddFetchItemL(TMsvId aPart, TImap4GetMailOptions aPartTypes, TBool& aHasTextParts)
	{
	DBG((LogText(_L8("AddFetchItemL(id %x, parts=%d)"),aPart,aPartTypes)));

	// Is this part fetchable?
	SetEntryL(aPart);

	// if the part is complete, then this means everything below it is
	// complete and therefore we don't need to fetch anything.
	if ( iEntry->Entry().iType != KUidMsvFolderEntry && iEntry->Entry().Complete()
		&& !(((TMsvEmailEntry)iEntry->Entry()).PartialDownloaded()))
		{
		DBG((LogText(_L8("Skipping, already complete"))));

		// If this is an attachment which has been marked complete because it has
  		// zero size, we still need to add it to the attachment manager.
 		if ((iEntry->Entry().iType == KUidMsvAttachmentEntry ||
 		      iEntry->Entry().iType == KUidMsvEmailExternalBodyEntry) &&
 		     (iEntry->Entry().iSize == 0))
 			{
 				DBG((LogText(_L8("Creating zero length attachment"))));
   				CreateAttachmentInfoL((TMsvEmailEntry&)iEntry->Entry());
  			}

		return;
		}

	TBool addChildren = EFalse;
	TBool addPart = EFalse;
	
	TUid type = iEntry->Entry().iType;
	if (type == KUidMsvFolderEntry || type == KUidMsvMessageEntry)
		{
		// don't fetch anything - just let it recurse
		addChildren = ETrue;
		}
	else if (type == KUidMsvEmailTextEntry || type == KUidMsvEmailHtmlEntry)
		{
		aHasTextParts = ETrue;
		
		if (aPartTypes == EGetImap4EmailBodyText ||
			aPartTypes == EGetImap4EmailBodyTextAndAttachments ||
			aPartTypes == EGetImap4EmailBodyAlternativeText)
			{
			addPart = ETrue;
			}
		}
	else if (type == KUidMsvAttachmentEntry || type == KUidMsvEmailExternalBodyEntry)
		{
		if (aPartTypes == EGetImap4EmailBodyTextAndAttachments ||
			aPartTypes == EGetImap4EmailAttachments)
			{
			addPart = ETrue;
			}
		else
			{
			SetEntryL(iEntry->Entry().Parent());
			TImEmailFolderType folderType = static_cast<TMsvEmailEntry>((iEntry->Entry())).MessageFolderType();
			SetEntryL(aPart);
			
			if( folderType==EFolderTypeRelated )
				{
			// if asked for bodytext and it is an attachment then
			// fetch it if attachment is in a folder of
			// Multipart/Related as it is most likely part of an MHTML
			// document
				addPart = ETrue;
				}
			else if( ( folderType == EFolderTypeAlternative || folderType == EFolderTypeUnknown ) && aPartTypes == EGetImap4EmailBodyAlternativeText)
				{
				// if non-HTML text alternative parts are requested, the alternative
				// folder is checked and get the mime content type for the part
				CMsvStore* store = iEntry->ReadStoreL();
				CleanupStack::PushL(store);
				CImMimeHeader* mimeHeaders = CImMimeHeader::NewLC();
				mimeHeaders->RestoreL(*store);
				
				if( mimeHeaders->ContentType().CompareF(KMIME_TEXT)==0 )
					{
					// This is a alternative text part, and should be treated
					// as a text part
					addPart = ETrue;
					}
					
				CleanupStack::PopAndDestroy(2, store); // mimeHeaders, store
				}

			// Store needs to be closed before calling CreateAttachmentInfoL
			if(!addPart)
				{
				CreateAttachmentInfoL((TMsvEmailEntry&)iEntry->Entry());
				}		
			}
		}
	else
		{
		__ASSERT_DEBUG(0, gPanic(EUnknownMsvType));

		// for anything else, if not debug mode then fetch anyway
		addPart = ETrue;
		}

	if (addPart)
		{
		iFetchList->AppendL(aPart);
		
		// Add this part's size to the size total
		iProgress.iBytesToDo+=iEntry->Entry().iBioType;
		}
	
	if (addChildren)
		{
		// Check the children
		CMsvEntrySelection *selection=new (ELeave) CMsvEntrySelection;
		CleanupStack::PushL(selection);
		GetChildrenL(*selection);
		
		DBG((LogText(_L8("...ID %x has %d children"),iEntry->Entry().Id(),selection->Count())));
		
		for(TInt a=0;a<selection->Count();a++)
			{
			// Process child
			AddFetchItemL((*selection)[a],aPartTypes,aHasTextParts);
			}
		CleanupStack::PopAndDestroy();
		}
	}

void CImImap4Session::AddFetchItemL(TMsvId aPart, TImImap4GetPartialMailInfo aGetPartialMailInfo, TBool& aHasTextParts)
	{
	DBG((LogText(_L8("AddFetchItemL(id %x, parts=%d)"),aPart,aGetPartialMailInfo.iPartialMailOptions)));

	// Is this part fetchable?
	SetEntryL(aPart);

	//if the part is complete, then this means everything below it is
	// complete and therefore we don't need to fetch anything.
	if (iEntry->Entry().iType != KUidMsvFolderEntry && iEntry->Entry().Complete())
		{
		DBG((LogText(_L8("Skipping, already complete"))));
		// If this is an attachment which has been marked complete because it has
  		// zero size, we still need to add it to the attachment manager.
 		if ((iEntry->Entry().iType == KUidMsvAttachmentEntry ||
 		      iEntry->Entry().iType == KUidMsvEmailExternalBodyEntry) &&
 		     (iEntry->Entry().iSize == 0))
 			{
 				DBG((LogText(_L8("Creating zero length attachment"))));
   				CreateAttachmentInfoL((TMsvEmailEntry&)iEntry->Entry());
	  		}

		return;
		}

	TBool addChildren = EFalse;
	TBool addPart = EFalse;
	
	TUid type = iEntry->Entry().iType;
	if (type == KUidMsvFolderEntry || type == KUidMsvMessageEntry)
		{
		// don't fetch anything - just let it recurse
		addChildren = ETrue;
		}
	else if (type == KUidMsvEmailTextEntry) 
		{
		aHasTextParts = ETrue;
		
		if(aGetPartialMailInfo.iPartialMailOptions == ENoSizeLimits)
			addPart = ETrue;
		else if (aGetPartialMailInfo.iPartialMailOptions == ECumulative)
			{
			if(iGetPartialMailInfo.iTotalSizeLimit > 0)
				{
				addPart = ETrue;
				iBodyTextSize = iEntry->Entry().iBioType;
				}
			}
		else if (aGetPartialMailInfo.iPartialMailOptions == EBodyTextOnly ||
				aGetPartialMailInfo.iPartialMailOptions == EBodyTextAndAttachments ||
				aGetPartialMailInfo.iPartialMailOptions == EBodyAlternativeText )
			{
			addPart = ETrue;
			iBodyTextSize = iEntry->Entry().iBioType;
			}
		}
	else if (type == KUidMsvEmailHtmlEntry)
		{
		aHasTextParts = ETrue;

		iHtmlEntrySize = iEntry->Entry().iBioType;
		if(aGetPartialMailInfo.iPartialMailOptions == ENoSizeLimits)
			addPart = ETrue;
		else if (aGetPartialMailInfo.iPartialMailOptions == ECumulative)
			{
			if((iGetPartialMailInfo.iTotalSizeLimit > 0 ) && 
				((iBodyTextSize + iEntry->Entry().iBioType) <= iGetPartialMailInfo.iTotalSizeLimit))
				{
				addPart = ETrue;
				}
			}
		else if (aGetPartialMailInfo.iPartialMailOptions == EBodyTextOnly ||
				aGetPartialMailInfo.iPartialMailOptions == EBodyTextAndAttachments ||
				aGetPartialMailInfo.iPartialMailOptions == EBodyAlternativeText )
				
			{	
			if(iBodyTextSize + iEntry->Entry().iBioType <=
				Minimum(iGetPartialMailInfo.iBodyTextSizeLimit,iGetPartialMailInfo.iTotalSizeLimit))
				{
				addPart = ETrue;
				}
			}
		// In case of html entry, store html entry id to check later,(when attaching partial footer 
		// message)if whole body text is downloaded and the html size is not to be downloaded 
		if(addPart)
			iHtmlEntryPart = aPart;
		}
	else if (type == KUidMsvAttachmentEntry || type == KUidMsvEmailExternalBodyEntry)
		{
		
		if(aGetPartialMailInfo.iPartialMailOptions == ENoSizeLimits)
			addPart = ETrue;
		else if (aGetPartialMailInfo.iPartialMailOptions == ECumulative)
			{
			if(iGetPartialMailInfo.iTotalSizeLimit > 0 && 
				((iBodyTextSize + iSizeOfToBeFetchedAttachments + iEntry->Entry().iBioType) <= iGetPartialMailInfo.iTotalSizeLimit))
				{
				addPart = ETrue;
				if((iBodyTextSize + iSizeOfToBeFetchedAttachments + iEntry->Entry().iBioType + iHtmlEntrySize) 
						>= iGetPartialMailInfo.iTotalSizeLimit)
					{
					RemoveHtmlPart(iHtmlEntryPart);
					}
				iSizeOfToBeFetchedAttachments+=iEntry->Entry().iBioType;
				}	
			else
				{
				CreateAttachmentInfoL((TMsvEmailEntry&)iEntry->Entry());
				// for Ecumulative option ,after the body part downloading, check if there is any 
				// attachment which can be downloaded , then check if the html part can be included.
				if((iBodyTextSize + iSizeOfToBeFetchedAttachments + iHtmlEntrySize) >= iGetPartialMailInfo.iTotalSizeLimit)
					{
					RemoveHtmlPart(iHtmlEntryPart);
					}
				}
			}
		else if (aGetPartialMailInfo.iPartialMailOptions == EAttachmentsOnly ||
				aGetPartialMailInfo.iPartialMailOptions == EBodyTextAndAttachments)
				
			{
			if(iEntry->Entry().iBioType <= 
				Minimum(iGetPartialMailInfo.iAttachmentSizeLimit,iGetPartialMailInfo.iTotalSizeLimit))
				{
				addPart = ETrue;
				}
			else
				{
				CreateAttachmentInfoL((TMsvEmailEntry&)iEntry->Entry());
				}
			}
		else
			{
			SetEntryL(iEntry->Entry().Parent());
			TImEmailFolderType folderType = static_cast<TMsvEmailEntry>((iEntry->Entry())).MessageFolderType();
			SetEntryL(aPart);
			
			if( folderType==EFolderTypeRelated )
				{
				// if asked for bodytext and it is an attachment then
				// fetch it if attachment is in a folder of
				// Multipart/Related as it is most likely part of an MHTML
				// document
				addPart = ETrue;
				}
			else if( folderType==EFolderTypeAlternative && 
					 aGetPartialMailInfo.iPartialMailOptions==EBodyAlternativeText &&
					 iEntry->Entry().iBioType <= Minimum(iGetPartialMailInfo.iAttachmentSizeLimit,
					 									 iGetPartialMailInfo.iTotalSizeLimit) )
				{
				// if non-HTML text alternative parts are requested, the alternative
				// folder is checked and get the mime content type for the part
				CMsvStore* store = iEntry->ReadStoreL();
				CleanupStack::PushL(store);
				CImMimeHeader* mimeHeaders = CImMimeHeader::NewLC();
				mimeHeaders->RestoreL(*store);
				
				if( mimeHeaders->ContentType().CompareF(KMIME_TEXT)==0 )
					{
					// This is a alternative text part, and should be treated
					// as a text part
					addPart = ETrue;
					}
					
				CleanupStack::PopAndDestroy(2, store); // mimeHeaders, store
				}
				
			if(!addPart)
				{
				CreateAttachmentInfoL((TMsvEmailEntry&)iEntry->Entry());
				}
			}
		}
	else
		{
		__ASSERT_DEBUG(0, gPanic(EUnknownMsvType));

		// for anything else, if not debug mode then fetch anyway
		addPart = ETrue;
		}

	if (addPart)
		{
		iFetchList->AppendL(aPart);
		// Add this part's size to the size total
		iProgress.iBytesToDo+=iEntry->Entry().iBioType;
		}

	if (addChildren)
		{
		// Check the children
		CMsvEntrySelection *selection=new (ELeave) CMsvEntrySelection;
		CleanupStack::PushL(selection);
		GetChildrenL(*selection);
			
		DBG((LogText(_L8("...ID %x has %d children"),iEntry->Entry().Id(),selection->Count())));
		
		for(TInt a=0;a<selection->Count();a++)
			{
			// Process child
			AddFetchItemL((*selection)[a],aGetPartialMailInfo,aHasTextParts);
			}
		CleanupStack::PopAndDestroy();
		}
	}

void CImImap4Session::RemoveHtmlPart(TMsvId aPart)
	{
	// removes the html part from the download list only if it exists in the list
	if(aPart)
		{
		TInt aIndex = 0;
		TKeyArrayFix sortKey(0, ECmpTInt32);
		iFetchList->Find(aPart,sortKey,aIndex);
		iFetchList->Delete(aIndex,1);
		iHtmlEntryPart=0;
		}
	}

// Checks for the minimum size limit between message type size limit 
// (attachment size limit/body text sizelimit)
TInt32 CImImap4Session::Minimum(TInt32 aThisPartTypeSizeLimit,TInt32 aTotalMailSizeLimit)
	{
	if(aTotalMailSizeLimit > 0)
		{
		if(aThisPartTypeSizeLimit > aTotalMailSizeLimit)
			return aTotalMailSizeLimit;
		else
			return aThisPartTypeSizeLimit;
		}
	else
		return aThisPartTypeSizeLimit;
	}

// Issue command to fetch an item in the fetch list
void CImImap4Session::FetchAnItemL(const TMsvId aPart)
	{
	DBG((LogText(_L8("FetchAnItemL(%x)"),aPart)));

	// set the iFoundUid member variable to false to show that a UID has not been
	// found in the fetch response yet
	iFoundUid = EFalse;

	// Get part ID and read the MIME header so we can work out encoding style
	iMessageId=aPart;
	SetEntryL(iMessageId);
	CMsvStore *store=iEntry->ReadStoreL();
	CleanupStack::PushL(store);
	
	iIsDiskSpaceChecked = EFalse;
	// Get MIME header
	if (!iAttachmentMimeInfo)
		iAttachmentMimeInfo=CImMimeHeader::NewL();
	iAttachmentMimeInfo->RestoreL(*store);
		

	// We don't reset the stats here, as they're stats for a single
	// fetch operation, which may include multiple parts of the same
	// message. Stats initialisation is done in the FetchBody()
	// function.

	// Find the UID we need to fetch
	TMsvEmailEntry entry = iEntry->Entry();
	iMessageFetching=entry.UID();

	// check there is enough disk space for this part (plus slop)
	// iBioType contains remote size which will never be less than the
	// local size so is a safe value to use
	if(!iFetchPartialMail)
		{
		CheckForDiskSpaceL(entry.iBioType);
		}

	// Save encoding type
	iEncodingType=iAttachmentMimeInfo->ContentTransferEncoding();
	iB64Decoder.Initialise();

	// Clear QP buffer
	if (iPartialLine && iPartialLine->Length())
		iPartialLine->Des().Zero();

	// Is this a text fetch? (if so, save as a richtext in the database, as
	// opposed to a separate file). Default to being an attachment.

	// SJM: This code used to check for an attachment filename and
	// force FetchIsTest False in that case. This is unnecessary as
	// this check is already done in BuildTreeOne and reflected in the
	// entry.iType.

	iFetchIsText = EFalse;
	if (entry.iType == KUidMsvEmailTextEntry)
		{
		iFetchIsText = ETrue;
		// New message body
		if (iStore8BitData)
			{
			delete iBodyBuf;
			iBodyBuf = NULL;
			iBodyBuf = CBufSeg::NewL(KBodyTextChunkSizeBytes);
			delete iBodyText;
			iBodyText = NULL;
			iBodyText = CMsvBodyText::NewL();
			}
		else
			{
			delete iMessageBody;
			iMessageBody = NULL;
			iMessageBody=CRichText::NewL(iParaLayer, iCharLayer);
			}
		}

	// Waiting for size
	iSizeWait=ETrue;

	// Size of item (mainly for CC:Mail bug workaround) */
	iSizeOfThisPart=entry.iBioType;

	// if going into richtext only then setup charset conversion
	iPreparedToConvert=EFalse;
	TInt fetchSizeBytes = static_cast<TInt>(iServiceSettings->FetchSize()); 
	// If this was a partially fetched message then there is some already fetched 
	// message content in the message store
	TInt32 fetchSize = fetchSizeBytes;

	if (iFetchIsText)
		{

		TUint charsetId = iAttachmentMimeInfo->MimeCharset();
		
		if (iStore8BitData)
			{
			iBodyText->SetDefaultCharacterSet(iCharConv->SystemDefaultCharset());
			if (charsetId == KUidMsvCharsetNone)
				iBodyText->SetCharacterSet(0);
			else
				iBodyText->SetCharacterSet(charsetId);
			}
		else
			{
			if (charsetId == KUidMsvCharsetNone)
				charsetId = iCharConv->SystemDefaultCharset();
			}
		
		iAttachmentMimeInfo->SetMimeCharset(charsetId);
		if (!iStore8BitData)
			{
			if (charsetId != KUidMsvCharsetNone)
				iPreparedToConvert = iCharConv->PrepareToConvertToFromOurCharsetL(charsetId);
			}
		}

	// ensure nothing left over
	iLeftOver.SetLength(0);

	DBG((LogText(_L8("Starting fetch for body part (MsvId=%x, iSizeOfThisPart=%d, iFetchIsText=%d, convert=%d)"),
				 iMessageId,iSizeOfThisPart,iFetchIsText, iPreparedToConvert)));

	// Issue fetch command
	NewTag();
	TPtrC8 path=iAttachmentMimeInfo->RelativePath();

	if(iFetchPartialMail)
		fetchSize = GetFetchSizeL(iSizeOfThisPart,0); // As this is the first time 0 size downloaded already

	if(fetchSize > fetchSizeBytes)
		fetchSize = fetchSizeBytes;
	
	//Some servers dont support MIME.And it contains single body type.
	//Using KImapFetchBodyPeek/KImapFetchBody to fetch the body of mails.
	if(iProgress.iPartsToDo == 1)
		{
		if (iServiceSettings->UpdatingSeenFlags())
			{
			iImapIO->SendL(iStatus,KImapFetchBodyPeek, iTag,iMessageFetching,&path,0,fetchSize);
			}
		else
			{
			iImapIO->SendL(iStatus,KImapFetchBody, iTag,iMessageFetching,&path,0,fetchSize);
			}
		}
	else
		{
		if (iServiceSettings->UpdatingSeenFlags())
			{
			iImapIO->SendL(iStatus,KImapFetchMimeBodyPeek, iTag,iMessageFetching,&path,fetchSize,&path);	
			}		
		else
			{
			iImapIO->SendL(iStatus,KImapFetchMimeBody, iTag,iMessageFetching,&path,fetchSize,&path);
			}
		}
	NewTagSent();
	CleanupStack::PopAndDestroy();
	}

void CImImap4Session::CheckForDiskSpaceL(TInt aSizeToBeDownloaded)
	{
	TInt needSpace = 0;
	TVolumeInfo volumeInfo;
	User::LeaveIfError(iFs.Volume(volumeInfo, iCurrentDrive));

	needSpace = KMinimumDiskSpaceForSync + aSizeToBeDownloaded;
	if (volumeInfo.iFree < needSpace)
		User::Leave(KErrDiskFull);
	}

// Fetch body for partial download
void CImImap4Session::FetchBody(TRequestStatus& aRequestStatus, const TMsvId aPart,
								TImImap4GetPartialMailInfo aGetPartialMailInfo)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND FetchBody(%x)"),aPart)));
	TInt err=KErrNone;

	CheckForPartialPopulate(aGetPartialMailInfo);
	if (!Connected())
		{
		Queue(aRequestStatus);
		err=KErrDisconnected;
		}
	else
		{
		TRAP(err,FetchBodyL(aRequestStatus, aPart));
		}
	if (err!=KErrNone)
                {
		DBG((LogText(_L8("-----------------------------------------------------------"))));
		DBG((LogText(_L8("CImap4Session::FetchBody(): calling Complete()"))));
		DBG((LogText(_L8("-----------------------------------------------------------"))));


		Complete(err);
                }
	}

// Checks if the partial mail download parameters are set to default 
// and the full download mail option is set, then this is a request for full download. 
void CImImap4Session::CheckForPartialPopulate(TImImap4GetPartialMailInfo aGetPartialMailInfo)
	{
	if(aGetPartialMailInfo.iPartialMailOptions == ENoSizeLimits &&
		aGetPartialMailInfo.iTotalSizeLimit == KMaxTInt &&
		aGetPartialMailInfo.iBodyTextSizeLimit == KMaxTInt && 
		aGetPartialMailInfo.iAttachmentSizeLimit == KMaxTInt && 
		(aGetPartialMailInfo.iGetMailBodyParts == EGetImap4EmailHeaders || 
		aGetPartialMailInfo.iGetMailBodyParts == EGetImap4EmailBodyText ||
		aGetPartialMailInfo.iGetMailBodyParts == EGetImap4EmailBodyTextAndAttachments ||
		aGetPartialMailInfo.iGetMailBodyParts == EGetImap4EmailAttachments ||
		aGetPartialMailInfo.iGetMailBodyParts == EGetImap4EmailBodyAlternativeText))
		{
		DBG((LogText(_L8("Populate option = %d)"),aGetPartialMailInfo.iGetMailBodyParts)));
		iFetchPartialMail = EFalse;
		iGetOptions = aGetPartialMailInfo.iGetMailBodyParts;
		}
	else
		{
		DBG((LogText(_L8("Populate option = %d)"),aGetPartialMailInfo.iPartialMailOptions)));
		iFetchPartialMail = ETrue;
		iGetPartialMailInfo = aGetPartialMailInfo;
		}
	}

void CImImap4Session::FetchBodyL(TRequestStatus& aRequestStatus, const TMsvId aPart)
	{
	__ASSERT_DEBUG(iState==EImapStateSelected,gPanic(EFetchWhenNotSelected));
	if(!(iState==EImapStateSelected))
		{
		User::LeaveIfError(KErrArgument);// Fetch when not selected .
		}
	iGetPart = aPart;
	
	Queue(aRequestStatus);

	// if we only want headers then there is nothing to do as they
	// have already been fetched on synchronisation. We complete here
	// and then the Compound object will copy the structure across
	if(!iFetchPartialMail)
		{
		if (iGetOptions == EGetImap4EmailHeaders)
			{
                        DBG((LogText(_L8("-----------------------------------------------------------"))));
			DBG((LogText(_L8("CImap4Session::FetchBodyL(): calling Complete()"))));
			DBG((LogText(_L8("-----------------------------------------------------------"))));

			Complete(KErrNone);
			return;
			}
		}
	// Get some basic info on what we're fetching
	SetEntryL(aPart);

	// First, check that we're in the right folder: this involves working up from the
	// base part looking for a folder which matches the current iMailboxId. If we get
	// to the service without finding the current folder, then we're not in the
	// right place...
  	TBool messageFound = EFalse;
  	while((iEntry->Entry().Id()!=iMailboxId) && (!messageFound))
		{
		// Reached the service?
		if (iEntry->Entry().iType==KUidMsvServiceEntry)
			{
			// Didn't find the folder - we must have the wrong one selected
			Complete(KErrImapWrongFolder);

			DBG((LogText(_L8("In the wrong folder!"))));

			return;
			}

		// Reached the message ?
		if (iEntry->Entry().iType==KUidMsvMessageEntry)
			{
			iMessageUid = ((TMsvEmailEntry&)iEntry->Entry()).UID();
 			iSyncState = EGettingStructure;
  			iState = EImapStateFetchWait;
  			if (!(iEntry->Entry().Owner()))
  				{
  				// If there are no child entries then we need to fetch the envelope and some headers (again.)
  				// We can generate the message structure and header stores from the envelope and headers after they have been fetched.
  				if (ImapIdleSupported()==EFalse)
					{
					CancelDummy();
					}
  				// We don't know the size of the body part yet as there is no structure.
  				// The envelope will be fetched shortly so clear the progress information for now.
  				// Set the iBytesToDo to 1 rather than 0 to avoid any possible division by 0 errors
  				iProgress.iBytesToDo=1;
  				iProgress.iBytesDone=0;
  				FetchLargeHeaderL(iMessageUid, EFalse);
  				}
  			else
  				{
  				// If the structure is already present then do the fetch
  				if (ImapIdleSupported()==EFalse)
					{
					CancelDummy();
					}
  				DoFetchL();
  				messageFound = ETrue;
  				}
   			}

		// Up a level
		SetEntryL(iEntry->Entry().Parent());
		}
	}

// Set/reset local subscribed flag
void CImImap4Session::LocalSubscribeL(const TMsvId aTarget, const TBool aSubscribe)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND LocalSubscribe(%d,%d)"),aTarget,aSubscribe)));

	// Set/reset subscribed flag
	SetEntryL(aTarget);
	TMsvEmailEntry message=iEntry->Entry();

	// Change only if necessary
	if (message.Subscribed()!=aSubscribe)
		{
		message.SetSubscribed(aSubscribe);
		ChangeEntryL(message);
		}
	}

// Set/reset remote subscribed flag
void CImImap4Session::RemoteSubscribeL(TRequestStatus& aRequestStatus, const TMsvId aTarget, const TBool aSubscribe)
	{
	LOG_COMMANDS((LogText(_L8("COMMAND RemoteSubscribe(%d,%d)"),aTarget,aSubscribe)));
	__ASSERT_DEBUG(iState==EImapStateNoSelect || iState==EImapStateSelected,gPanic(ESubscribeWhenNotReady));
	if(!(iState==EImapStateNoSelect || iState==EImapStateSelected))
		{
		User::LeaveIfError(KErrInUse);// Subscribe when not ready.
		}
	Queue(aRequestStatus);
	
	// Save ID for updating subscription flag when the remote one completes
	// successfully.
	iCommandIds[0]=aTarget;
	iCommandFlags[0]=aSubscribe;

	// Get path to delete
	HBufC8* path=NULL; // To stop .AER warnings...
	TRAPD(err,path=MakePathL(aTarget,ETrue));
	if (err!=KErrNone)
		{
		Complete(err);
		return;
		}
	CleanupStack::PushL(path);
	DoQuoteL(path);

	// Set state
	iSavedState=iState;
	iState=EImapStateSubscribeWait;

	// Send command
	NewTag();
	TPtrC8 pathdes(path->Des());
	if(aSubscribe)
		iImapIO->SendL(iStatus,_L8("%d %S\"%S\"\r\n"),iTag,&KIMAPC_SUBSCRIBE,&pathdes);
	else
		iImapIO->SendL(iStatus,_L8("%d %S\"%S\"\r\n"),iTag,&KIMAPC_UNSUBSCRIBE,&pathdes);
	NewTagSent();

	// Destroy buffer
	CleanupStack::PopAndDestroy();
	}

// Reset sync/fetch stats
void CImImap4Session::ResetStats()
	{
	// Reset all the stats
	iHeadersFetched=0;
	iOrphanedMessages=0;
	iRemoteMessagesDeleteTagged=0;
	iMsgsDone=0;

	iProgress.iState = TImap4GenericProgress::EIdle;
	iProgress.iImap4SubStateProgress = TImap4GenericProgress::EIdle;
	iProgress.iMsgsToDo = iProgress.iMsgsDone = 0;
	iProgress.iPartsToDo = iProgress.iPartsDone = 0;
	iProgress.iBytesToDo = iProgress.iBytesDone = 0;
	iProgress.iErrorCode = KErrNone;
	iProgress.iReturnedMsvId = 0;
	iProgress.iTotalSize = 0;
	}

// Return progress

// Msgs ToDo/Done are only setup when doing a Synchronise command, ie
// in state EImapStateSynchroniseWait.

// Parts/Bytes ToDo/Done are set up and used in a FetchBody command,
// ie in state EImapStateFetchWait.

// BytesToDo/Done are used in Append command.

void CImImap4Session::IncSyncStats(TImap4SyncProgress& aSync)
	{
	// do the synchronising stats
	aSync.iHeadersFetched += iHeadersFetched;
	aSync.iOrphanedMessages += iOrphanedMessages;
	aSync.iRemoteMessagesDeleteTagged += iRemoteMessagesDeleteTagged;

	if(iServiceSettings->SearchString().Length() != 0)
		{
		aSync.iMsgsToDo=iMailboxSize;	
		}
	else
		{
		aSync.iMsgsToDo=(iSyncLimit<=0)?iMailboxSize:Min(iMailboxSize,iSyncLimit);	
		}
	aSync.iMsgsDone = Min(iMsgsDone,aSync.iMsgsToDo);
	}

TImap4GenericProgress CImImap4Session::Progress()
	{
	// update the state with what we're doing
	if (iState==EImapStateDisconnected)
		{
		iProgress.iState=TImap4GenericProgress::EDisconnected;
		}
	else if (iState<EImapStateNoSelect)
		{
		iProgress.iState = TImap4GenericProgress::EConnecting;
		// If we're in the connecting state, get the stage and IAP values
		//  and store them in iMsgsDone and iMsgsToDo to be accessed via
		//  TImap4GenericProgress::ConnectionState() and TImap4GenericProgress::ConnectionIAP()
		iProgress.iMsgsDone = iImapIO->GetConnectionStage();
		TUint32 iap;
		TInt err = iImapIO->GetIAPValue(iap);
		if (err == KErrNone)
			{
			iProgress.iMsgsToDo = iap;
			}
		else
			{
			iProgress.iMsgsToDo = err;
			}
		}
	else if (iState==EImapStateNoSelect || iState==EImapStateSelected || iState==EImapStateIdling)
		{
		iProgress.iState=TImap4GenericProgress::EIdle;
		}
	else
		{
		switch(iState)
			{
		case EImapStateFetchWait:
			iProgress.iState=TImap4GenericProgress::EFetching;
			break;

		case EImapStateAppendSizeWait:
		case EImapStateAppendPromptWait:
		case EImapStateAppendWait:
		case EImapStateAppendResultWait:
			iProgress.iState=TImap4GenericProgress::EAppending;
			break;

		case EImapStateSynchroniseWait:
			iProgress.iState=TImap4GenericProgress::ESyncing;
			break;

		case EImapStateLogoutWait:
			iProgress.iState=TImap4GenericProgress::EDisconnecting;
			break;

		case EImapStateDeleteWait:
		case EImapStateDeleteAllWait:
		case EImapStateDeleteFolderWait:
		case EImapStateDeleteMarkWait:
		case EImapStateExpungeWait:
		case EImapStateExpungeAllWait:
			iProgress.iState = TImap4GenericProgress::EDeleting;
			break;

		case EImapStateSelectWait:
			iProgress.iState = TImap4GenericProgress::ESelecting;
			break;

		default:
			// Just 'busy' otherwise
			iProgress.iState=TImap4GenericProgress::EBusy;
			break;
			}
		}

	return iProgress;
	}

// Set entry pointer
void CImImap4Session::SetEntry(CMsvServerEntry *aEntry)
	{
	// Take note of this CMsvServerEntry
	iEntry=aEntry;

	// Park entry
	iEntry->SetEntry(NULL);
	}

// Park entries
void CImImap4Session::Park()
	{
	// Park normal entry
	iEntry->SetEntry(NULL);

	// Park moveentry if it exists
	if (iMoveEntry) iMoveEntry->SetEntry(NULL);
	}

TInt CImImap4Session::CommandFailure() const
	{
	return iCommandFailure;
	}

void CImImap4Session::FetchHeaderL(TUint aUid)
	{
	iFolderIndex.SetSizeL(iMailboxSize);

	// Create list of priority fields to request
	TBuf8<256> priorityFields;
	CDesC8ArrayFlat* array = new(ELeave) CDesC8ArrayFlat(4);
	CleanupStack::PushL(array);
	CImcvUtils::PriorityFieldsL(*array);
	for (TInt i=0; i<array->Count(); i++)
		{
		priorityFields.Append((*array)[i]);
		}
	CleanupStack::PopAndDestroy(array);
	ColonSeparatorToSpace(priorityFields);

	NewTag();
	
	iImapIO->SendL(iStatus, KImapFetchSmallHeaderToEnd,iTag,aUid, &priorityFields);	  	
  	NewTagSent();
	}

void CImImap4Session::FetchLargeHeaderL(TUint aUid, TBool aRange)
 	{
  	// First, resize folder index to hold all messages in the folder,
  	// as opposed to the old sync list. This will preserve the old
  	// contents of the index, which is what we want as it's up-to-date
  	// and correct.
	iFolderIndex.SetSizeL(iMailboxSize);

  	// build list of header fields we want note that
  	// ReceiptFieldStrings returns strings colon terminated which we
  	// convert to spaces for the fetch
  	TBuf8<256> buf;
  
  	CDesC8ArrayFlat* array = new(ELeave) CDesC8ArrayFlat(4);
  	CleanupStack::PushL(array);
  
  	CImcvUtils::ReceiptFieldsL(*array);
  	TInt i;
  	for (i=0; i<array->Count(); i++)
  		buf.Append((*array)[i]);
  
  	CImcvUtils::PriorityFieldsL(*array);
  	for (i=0; i<array->Count(); i++)
  		buf.Append((*array)[i]);
  
  	CleanupStack::PopAndDestroy(); // array
  
  	ColonSeparatorToSpace(buf);
  	
  	// Send command
  	NewTag();
  	if (!aRange)
  		{
  		// We only want one envelope
  		iImapIO->SendL(iStatus, KImapFetchLargeHeader, iTag, aUid, &buf);
  		}
  	else
  		{
		// If a UID search string has been specified, then we should create the UID FETCH
		// string from the UID integer list.
		if(iServiceSettings->SearchString().Length() !=0)
			{
			CreateUidStringL();
			TPtrC8 ptr(iUidString->Des());
 			iImapIO->SendL(iStatus, KImapFetchLargeHeaderRangeRefined,iTag,&ptr,&buf);
 			}
 		else
 			{
	  		iImapIO->SendL(iStatus, KImapFetchLargeHeaderRange,iTag,aUid, &buf);
 			}		
  		}
  	NewTagSent();
   	}
 
void CImImap4Session::DoFetchL()
 	{
	DBG((LogText(_L8("CImap4Session::DoFetchL(): running..."))));

 	User::LeaveIfError(iEntry->SetEntry(iGetPart));
 	TUid type=iEntry->Entry().iType;
 
 	// if we are not asking for a Message type then override the get
	// options to ensure that this is fetched
 	if(!iFetchPartialMail)
		{
		if (type != KUidMsvMessageEntry)
 			iGetOptions = EGetImap4EmailBodyTextAndAttachments;
		}
 
 	User::LeaveIfError(iEntry->SetEntry(iServiceId));
 				
 	// What have we been asked to fetch? Build a list of parts to fetch: if the
 	// part requested has any children, that is.
 	// Reset stats
 	iProgress.iBytesToDo=0;
 	iProgress.iBytesDone=0;
 	iFetchList->Reset();
	iHtmlEntryPart = 0;
	iBodyTextSize = 0;
	iHtmlEntrySize = 0;
	iBodyPartRemainingSize = 0;
	iFooterString = NULL;
	iSizeOfToBeFetchedAttachments=0;
	TBool hasTextParts = EFalse;
	if(iFetchPartialMail)
		{
		DBG((LogText(_L8("Using partial mail options"))));
		AddFetchItemL(iGetPart,iGetPartialMailInfo,hasTextParts);
		DBG((LogText(_L8("Found %d parts to fetch (options=%d)"),iFetchList->Count(),iGetPartialMailInfo.iPartialMailOptions)));
		}
	else
		{
 		AddFetchItemL(iGetPart,iGetOptions,hasTextParts);
		DBG((LogText(_L8("Found %d parts to fetch (options=%d)"),iFetchList->Count(),iGetOptions)));
		}
		
	if( !hasTextParts && type == KUidMsvMessageEntry )
		{
		// There are no text parts to this message - need to set body text 
		// complete flag to true otherwise UI may allow such a message to 
		// repeatedly be 'fetched' even though there is no text to fetch!
		//
		// So, set body text complete and message complete flags on the entry
		// specified by iGetPart.
		DBG((LogText(_L8("Message %d has no text parts - setting complete flag and body text complete flag to ETrue"),iGetPart)));

	 	User::LeaveIfError(iEntry->SetEntry(iGetPart));
		TMsvEmailEntry message = iEntry->Entry();

		message.SetBodyTextComplete(ETrue);
		
		ChangeEntryL(message);

		// NOTE - not sure if necessary, but changing back to service entry to 
		// ensure consistent behaviour.
	 	User::LeaveIfError(iEntry->SetEntry(iServiceId));
		}
		
	// Any parts at all?
 	if (iFetchList->Count() == 0 || iState == EImapStateFetchCancelWait)
 		{
 		// No, complete the fetch.
 		if( iState != EImapStateFetchCancelWait )
 			{
			iState=EImapStateSelectWait;
			iSyncState=ENotSyncing;
 			}
		
 		if( iCommandsOutstanding )
 			{
 			// Waiting for tagged response for fetch command that got the
 			// email structure
	 		GetReply(EFalse);
 			}
 		else
 			{
 			// Message structure already known - therefore not waiting for the
 			// tagged response, complete immediately
 			CommandCompleteL(KErrNone);
 			}
 		return;
 		}
 
 	// Part count for stats
 	if (iFetchList->Count() > 0)
 		{
 		iProgress.iPartsToDo=iFetchList->Count();
 		iProgress.iPartsDone=0;
 
 		// Do the fetch
 		iState=EImapStateFetchWait;
 		iSyncState=EFetching;
 
 		DBG((LogText(_L8("Starting body fetch of %d parts (%d bytes)"),
 					 iProgress.iPartsToDo,iProgress.iBytesToDo)));
 
 		// Make the command to send to the server
 		FetchAnItemL((*iFetchList)[0]);
 		iFetchList->Delete(0,1);
 		}

	}

TInt CImImap4Session::CalculateDownloadSizeL(const CMsvEntrySelection& aSelection)
	{
	TInt totalSize = 0;

	// Do a quick tally on the size of messages which are to be copied / moved.
	TInt count=aSelection.Count();
	while (count--)
		{
		SetEntryL(aSelection.At(count));
		// Only add the size up if the message is not complete.
		if(!iEntry->Entry().Complete())
			{
			totalSize += iEntry->Entry().iSize;
			}
		}
	return totalSize;
	}

void CImImap4Session::SetSynchronisationSelectionL(CMsvEntrySelection &aSelection)
	{
	// Used by the server mtm to prevent any messages selected for retrieval
	// from being deleted during a synchronisation with a synchronisation limit set.
	delete iSynchronisationSelection;
	iSynchronisationSelection = 0;
	iSynchronisationSelection = aSelection.CopyL();
	}

void CImImap4Session::SetInbox(TMsvId aInbox)
	{
	iInbox=aInbox;
	}

TMsvId CImImap4Session::GetInbox()
	{
	return iInbox;
	}

// Set or clear the \Seen flags on the server (aSettingsFlag = ETrue -> Sets the flag)
// Returns False if no messages need to be processed
TBool CImImap4Session::ProcessSeenFlagsL(TSeenFlagUpdateMode aUpdateMode)
	{
	CArrayFixFlat<TMsvId>* pendingList;
	TBool settingFlag = (aUpdateMode == ESetSeenFlag);

	// Point pendingList to the correct list
	pendingList = (settingFlag ? iSetSeenList: iClearSeenList);
	
	// Exit if nothing to process
	if (!pendingList->Count())
		{
		return EFalse;
		}
	
	#ifdef PRINTING
		_LIT8(KCommandProcessFlags, "COMMAND ProcessSeenFlags(%d)");
		LOG_COMMANDS((LogText(KCommandProcessFlags, aUpdateMode)));
	#endif

	_LIT8(KStoreFlagsSetCommand, "%d UID STORE %S +FLAGS (\\Seen)\r\n");
	_LIT8(KStoreFlagsClearCommand, "%d UID STORE %S -FLAGS (\\Seen)\r\n");
	const TInt KMaxUIDsToProcess = 50;
	const TInt KMaxCharsPerUID = 12;

	// Ensure that the buffer passed to CImapIO does not exceed 1024
	__ASSERT_DEBUG(KMaxUIDsToProcess * KMaxCharsPerUID < 1024 - (KStoreFlagsSetCommand().Length() + 10), gPanic(KMaxBufferLengthExceeded));

	TInt pos = 0;
	TInt stored = 0;
	TInt run = 0;
	TInt UID;
	TInt lastUID = -1;
	TInt listCount = pendingList->Count();
	TBool haveSentCommand = EFalse;
	TMsvEmailEntry message;
	HBufC8* command=HBufC8::NewLC(KMaxUIDsToProcess * KMaxCharsPerUID);
	TPtr8 commandDes = command->Des();

	// Build up a list of UID's who's \Seen flag needs changing.
	// To save bandwidth, group contiguous blocks of UID's together,
	// e.g. 1:6,8:12,14 instead of 1,2,3,4,5,6,8,9,10,12,14
	while(pos < listCount && stored < KMaxUIDsToProcess)
		{
		SetEntryL(pendingList->At(pos));
		message = iEntry->Entry();
		UID = static_cast<TInt>(message.UID());

		// Should never have the state when the list contains flags that match the servers flag
		__ASSERT_DEBUG(FIXBOOL(message.Unread()) != settingFlag, gPanic(KSettingSeenFlagToExistingState));

		// Set the Seen flag for the message
		message.SetSeenIMAP4Flag(settingFlag);
		ChangeEntryL(message);
		// If not first time through and the UID follows the previous one
		if (pos !=0 && UID == lastUID+1)
			{
			// We are in a run
			run++;
			}
		else
			{
			// If we were in a run
			if (run)
				{
				// Append a ':' followed by the UID at the end of the run
				commandDes.Append(TChar(':'));
				commandDes.AppendNum(lastUID);
				stored++;
				// Reset the run count
				run = 0;
				}
			// Append ',' only if not the first time through
			if (pos!=0)
				{
				commandDes.Append(TChar(','));
				}
			// Append the current UID
			commandDes.AppendNum(UID);
			stored++;
			}
		// Next message
		pos++;
		lastUID = UID;
		} // end while

	// If we are at the end of the list but still in a run, add the last uid to the command buffer
	if (run)
		{
		commandDes.Append(TChar(':'));
		commandDes.AppendNum(lastUID);
		}

	// Anything stored?
	if (stored)
		{
		// Get a new tag
		NewTag();

		if (settingFlag)
			iImapIO->SendL(iStatus,KStoreFlagsSetCommand, iTag, command);
		else
			iImapIO->SendL(iStatus,KStoreFlagsClearCommand, iTag, command);

		// Send the command
		NewTagSent();
		// Remove the processed flags from the list (pos = num_to_remove)
		pendingList->Delete(0, pos);
		haveSentCommand = ETrue;
		}
	else
		{
		// Should never have the state when the list contained flags but we never sent the 'STORE FLAGS' command to the server
		__ASSERT_DEBUG(FIXBOOL(message.Unread()) != settingFlag, gPanic(KErrorBuildingStoreFlagsCommand));
		}

	// Get rid of command buffer
	CleanupStack::PopAndDestroy(command);
	return haveSentCommand;
	}
// Creates an IMAP4 defined optimised string of UIDs from the integer list of UIDs
// retrieved by the UID SEARCH command.
// Example: If the input array is 123 125 126 127 300 302 303 304 308
// The output string will be "123,125:127,300,302,303:304,308
// For the time being at least, keep the string as a class instance variable for possible re-use
void CImImap4Session::CreateUidStringL()
	{
	iUidString->Des().Zero();
	TPtr8 ptr(iUidString->Des());
	TUint32* current = &iSearchList->At(0); // Loop iterator
	TUint32* start(NULL); // Keep track of the start of a UID sequence
	TInt count = iSearchList->Count(); // Quick access
	while(current < &iSearchList->At(0) + count) // Loop till the end of the integer list
		{
		// Check for room to add 2 integers plus 2 separators.
		// If not enough room then reallocate 
		if((ptr.MaxLength() - ptr.Length()) < (KMaxUint32Chars)+2)
			{
			iUidString = iUidString->ReAllocL(ptr.MaxLength()*2); // Make sure this time
			ptr.Set(iUidString->Des());
			}
		start = current; // Mark the start of a possible sequence
		if(current != &iSearchList->At(0)) // If it's the first time through the loop don't append separator
			{
			ptr.Append(',');
			}
		ptr.AppendNum(*start); // First integer always written
		// Loop until end of sequence or end of list.
		// will loop through 125 126 127
		while(current < &iSearchList->At(count-1) && *current == (*(current+1))-1)
			{
			++current;
			}
		// If there was a run then current won't equal start.
		// That being the case, append the sequence separator followed by last in sequence
		// 125:127
		// If there's no sequence, the next int is written by ptr.AppendNum(*start)
		if(current != start)
			{
			ptr.Append(':');
			ptr.AppendNum(*current);
			}
		++current;
		}	
	}

void CImImap4Session::IdleTimerExpiredL()
	{
	iIdleTimerExpired = ETrue;
	ReissueIdleL();
	}

void CImImap4Session::CancelTimerExpired()
	{
	Cancel();
	if (iState != EImapStateDisconnected)
		{
		DoDisconnect();
		}
	iObserver.NonCompletedFailure();
	}

	
CIdleTimeoutTimer::CIdleTimeoutTimer(CImImap4Session& aOperation)
: CTimer(EPriorityHigh), iOperation(aOperation)
	{}

void CIdleTimeoutTimer::RunL()
	{
	iOperation.IdleTimerExpiredL();
	}

CIdleTimeoutTimer* CIdleTimeoutTimer::NewL(CImImap4Session& aOperation)
	{
	CIdleTimeoutTimer* self = new(ELeave) CIdleTimeoutTimer(aOperation);
	CleanupStack::PushL(self);
	self->ConstructL(); // CTimer
	CActiveScheduler::Add(self);
	CleanupStack::Pop();
	return self;
	}


CImImap4SessionDummyRead* CImImap4SessionDummyRead::NewL(CImImap4Session& aSession, CImapIO& aImapIO, TInt aPriority)
	{
	CImImap4SessionDummyRead* self = new (ELeave) CImImap4SessionDummyRead(aSession, aImapIO, aPriority);
	return self;
	}

CImImap4SessionDummyRead::CImImap4SessionDummyRead(CImImap4Session& aSession, CImapIO& aImapIO, TInt aPriority)
: CActive(aPriority), iSession(aSession), iImapIO(aImapIO)
	{
	DBG(iSession.LogText(_L8("+ CImImap4SessionDummyRead()") ) );
	CActiveScheduler::Add(this);
	DBG(iSession.LogText(_L8("- CImImap4SessionDummyRead()") ) );
	}

CImImap4SessionDummyRead::~CImImap4SessionDummyRead()
	{
	DBG(iSession.LogText(_L8("+ ~CImImap4SessionDummyRead") ) );
	Cancel();
	DBG(iSession.LogText(_L8("- ~CImImap4SessionDummyRead") ) );
	}

void CImImap4SessionDummyRead::Start()
	{
	DBG( iSession.LogText( _L8("+ CImImap4SessionDummyRead::Start()") ) );
	if(IsActive())
		{ 
		// already have an outstanding dummy read, ignore this
		DBG( iSession.LogText( _L8("CImImap4SessionDummyRead::Start():Already active") ) );
		return;
		}

	DBG( iSession.LogText( _L8("+ CImImap4SessionDummyRead::Start()") ) );
	// As the spec is ambiguous about when we can get untagged responses,
	// we can get them any time, so ask for a proper reply
	iImapIO.GetReply(iStatus);
        
        DBG((iSession.LogText(_L8("*******************************************************"))));
	DBG((iSession.LogText(_L8("CImap4SessionDummyRead::Start(): waiting for iImapIO to wake me"))));
	DBG((iSession.LogText(_L8("*******************************************************"))));

 	SetActive();
	DBG( iSession.LogText( _L8("- CImImap4SessionDummyRead::Start()") ) );
	}

void CImImap4SessionDummyRead::DoCancel()
	{
	DBG( iSession.LogText( _L8("+ CImImap4SessionDummyRead::DoCancel()") ) );
	iImapIO.Cancel();
	DBG( iSession.LogText( _L8("- CImImap4SessionDummyRead::DoCancel()") ) );
	}

void CImImap4SessionDummyRead::RunL()
	{
	//Inform iOperation that the dummy read has completed.

	//Under normal conditions the dummy read should never complete.
	//However, if it does complete it is likely that iStatus != KErrNone.
	DBG( iSession.LogText( _L8("+ CImImap4SessionDummyRead::RunL()") ) );
	iSession.DummyComplete(iStatus.Int());
	if (iStatus.Int() >= KErrNone) 
		{ 
		DBG( iSession.LogText(_L8(" CImImap4SessionDummyRead::RunL() There might be more data coming, issue dummy reading again.")) ); 
		iSession.ReissueDummy(); 
		}
	DBG( iSession.LogText( _L8("- CImImap4SessionDummyRead::RunL()") ) );
	}
	

CImImap4SessionIdleRead* CImImap4SessionIdleRead::NewL(CImImap4Session& aOperation, TInt aPriority)
	{
	CImImap4SessionIdleRead* self = new (ELeave) CImImap4SessionIdleRead(aOperation, aPriority);
	return self;
	}

CImImap4SessionIdleRead::CImImap4SessionIdleRead(CImImap4Session& aOperation, TInt aPriority)
: CActive(aPriority), iOperation(aOperation)
	{
	CActiveScheduler::Add(this);
	}

CImImap4SessionIdleRead::~CImImap4SessionIdleRead()
	{
	Cancel();
	}

void CImImap4SessionIdleRead::Start(TRequestStatus& aStatus)
	{
	// Store the status for completion later
	iOperationStatus = &aStatus;
	*iOperationStatus = KRequestPending;

	// Issue the read with *our* status
	iOperation.DoIdleRead(iStatus);
        DBG((iOperation.LogText(_L8("*******************************************************"))));
	DBG((iOperation.LogText(_L8("CImap4SessionIdleRead::Start(): waiting for iOperation to wake me"))));
	DBG((iOperation.LogText(_L8("*******************************************************"))));

	SetActive();
	}

void CImImap4SessionIdleRead::DoCancel()
	{
	DBG(iOperation.LogText(_L8("CImImap4SessionIdleRead::DoCancel()")));

	// Cancel the idle read we started
	iOperation.CancelIdleRead();
        DBG((iOperation.LogText(_L8("-----------------------------------------------------------"))));
	DBG((iOperation.LogText(_L8("CImap4SessionIdleRead::DoCancel(): calling request complete"))));
	DBG((iOperation.LogText(_L8("-----------------------------------------------------------"))));

	User::RequestComplete(iOperationStatus, KErrCancel);
	}

void CImImap4SessionIdleRead::RunL()
	{
	TInt err = iStatus.Int();
	DBG(iOperation.LogText(_L8("CImImap4SessionIdleRead::RunL(iStatus=%d)"),err));

	if(err < KErrNone)
		{ // Special case for error
		// Need to do this because if read completed with an error,
		// the RequestComplete would not handle this by itself as 
		// the RunL would not be called and neither would the 
		// DoComplete (because the session's iReport is NULL...)
		iOperation.IdleReadError(err);
		}
        DBG((iOperation.LogText(_L8("-----------------------------------------------------------"))));
	DBG((iOperation.LogText(_L8("CImap4SessionIdleRead::RunL(): calling request complete"))));
	DBG((iOperation.LogText(_L8("-----------------------------------------------------------"))));

	User::RequestComplete(iOperationStatus, err);
	}
