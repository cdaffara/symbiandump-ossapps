/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*      Helper class to implement attachment handling
*
*/



// INCLUDE FILES

#include    <e32std.h>

#include    <msventry.h>
#include    <msvstd.h>
#include    <msvapi.h> //Message Server
#include    <centralrepository.h>
#include    <badesca.h>
#include    <cmsvmimeheaders.h>
#include    <mmsvattachmentmanager.h>
#include    <mmsvattachmentmanagersync.h>
#include    <utf.h>
#include    <f32file.h>
#include    <msgtextutils.h>

#include    "mmsconst.h"
#include    "mmsattachmenthandler.h"
#include    "mmsgenutils.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KMms10kilos = 10240;
const TInt KMmsMaxBytesPerCharacter = 4;    
const TInt KMmsTextBufferSize = 256;
const TInt KMmsUnicodeToUtf2MaxIncrease = 2;
const TInt KMmsLengthOfCRlf = 2;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CMmsAttachmentHandler::CMmsAttachmentHandler() 
    {
    }

// EPOC default constructor can leave.
void CMmsAttachmentHandler::ConstructL()
    {
    }

// Two-phased constructor.
EXPORT_C CMmsAttachmentHandler* CMmsAttachmentHandler::NewL() 
    {
    CMmsAttachmentHandler* self = new (ELeave) CMmsAttachmentHandler();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CMmsAttachmentHandler::~CMmsAttachmentHandler()
    {
    }

// ---------------------------------------------------------
// CMmsAttachmentHandler::AttachmentsSizeL
// ---------------------------------------------------------
//
EXPORT_C TInt CMmsAttachmentHandler::AttachmentsSizeL( CMsvStore& aStore )
    {
    // Caller controls store
    TInt size = 0;
    
    MMsvAttachmentManager& attachMan = aStore.AttachmentManagerL();
    TInt numAttachments = attachMan.AttachmentCount();
    
    TInt i;
    
    for ( i = 0; i < numAttachments; i++ )
        {
        CMsvAttachment* attachmentInfo = attachMan.GetAttachmentInfoL(i);
        CleanupStack::PushL( attachmentInfo );
        
        CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
        CleanupStack::PushL( mimeHeaders );
        
        mimeHeaders->RestoreL( *attachmentInfo );
        
        RFile attaFile = attachMan.GetAttachmentFileL( i );
        CleanupClosePushL( attaFile );
        TInt fileSize = 0;
        
        // If we cannot access the file, we are in trouble
        User::LeaveIfError( attaFile.Size( fileSize ) ); 
        
        // This adds up mime header size + actual attachment binary data
        size += mimeHeaders->Size() + fileSize;
        
        CleanupStack::PopAndDestroy( &attaFile ); // close attaFile
        CleanupStack::PopAndDestroy( mimeHeaders );
        CleanupStack::PopAndDestroy( attachmentInfo );
        }
    
    return size;
    }
    
// ---------------------------------------------------------
// CMmsAttachmentHandler::IsValidFilename
// ---------------------------------------------------------
//
EXPORT_C TBool CMmsAttachmentHandler::IsValidFilename( RFs& aFs, const TPtrC& aFileName )
    {
    TBool validName = EFalse; //pessimist.
    
    if ( aFileName.Length() == 0 )
        {
        return EFalse;
        }

    // filename should not start with dot
    // or contain any control characters
    TInt i;
    // First character may not be . or space
    if ( aFileName[0] == 0x2e || aFileName[0] == 0x20 )
        {
        return EFalse;
        }

    for ( i = 0; i < aFileName.Length(); i++ )
        {
        // check for control characters - RFs does not do it.
        if ( aFileName[i] < 0x20 )
            {
            // found a control character - not allowed.
            return EFalse;
            }
        }
    validName = aFs.IsValidName( aFileName );

    return validName;
    }
    
// ---------------------------------------------------------
// CMmsAttachmentHandler::CreateAttachmentL
// ---------------------------------------------------------
//
EXPORT_C void CMmsAttachmentHandler::CreateAttachmentL(
            CMsvStore& aStore,
            RFile& aFile,
            RFs& aFs,
            TDriveUnit aMessageDrive,
            TDesC8& aMimeType,
            CMsvMimeHeaders& aMimeHeaders,
            CMsvAttachment* aAttachmentInfo,
            TMsvAttachmentId& aAttaId)
    {
    // The ownership of aAttachmentInfo will be transferred to attachment manager
    // We must keep it safe until that time  
    CleanupStack::PushL( aAttachmentInfo );
      
    // Check that sufficient disk space available
    // for attachment binary file and index entry
    
    TInt error = KErrNone;
    TInt fileSize = 0;
    
    error = aFile.Size( fileSize );
    User::LeaveIfError( error );
    
    aAttachmentInfo->SetSize( fileSize );
    if ( aMimeHeaders.SuggestedFilename().Length() == 0 )
        {
        TFileName name;
        error = aFile.Name( name );
        if ( error == KErrNone )
            {
            aMimeHeaders.SetSuggestedFilenameL( name );
            }
        }
    
    if ( aMimeHeaders.SuggestedFilename().Length() > 0 )
        {
        aAttachmentInfo->SetAttachmentNameL( aMimeHeaders.SuggestedFilename() );
        }
    if ( aMimeType.Length() > 0 )
        {
        aAttachmentInfo->SetMimeTypeL( aMimeType );
        }
    
    // Check that sufficient disk space available
    // for attachment binary file and index entry
    
    // This does not include mime headers.
    // The mime headers are covered by KMmsIndexEntryExtra,
    // however the value may be too small, has to be checked.
    
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &aFs, 
        fileSize + KMmsIndexEntryExtra,
        aMessageDrive ) )
        {
        // we use standard error code here
        User::Leave( KErrDiskFull );
        }
        
    if ( ( aMimeHeaders.ContentType().Length() == 0 ||
        aMimeHeaders.ContentSubType().Length() == 0  ) && aMimeType.Length() > 0 )
        {
        TInt position = aMimeType.Find( KMmsSlash8 );
        if ( position > 0 )
            {
            aMimeHeaders.SetContentTypeL( aMimeType.Left( position ) );
            }
        if ( position < aMimeType.Length() - 1 )
            {
            aMimeHeaders.SetContentSubTypeL( aMimeType.Mid( position + 1 ) );
            }
        }
    
    MMsvAttachmentManagerSync& attaManSync = aStore.AttachmentManagerExtensionsL();
    
    RFile attaFile;
    
    // ownership of aAttachmentInfo is transferred to attachment manager.
    attaManSync.CreateAttachmentL( aMimeHeaders.SuggestedFilename(),
        attaFile, aAttachmentInfo );
    aAttaId = aAttachmentInfo->Id();
    CleanupStack::Pop( aAttachmentInfo ); // attachment manager now owns aAttachmentInfo
       
    // If the previous call was successful, we can now write the data
    // We need a buffer because we read from one file and write to another
    
    CleanupClosePushL( attaFile );
    
    if ( fileSize > 0 )
        {
        // Greedy, but we don't try to swallow large files all in one piece
        // Small files may be handled in one piece
        HBufC8* buffer = HBufC8::NewL( Min( fileSize, KMms10kilos ) ); // Try to get at least 10 k
        CleanupStack::PushL( buffer );
        
        TPtr8 ptr = buffer->Des();
        ptr.SetLength( 1 ); // initialized to something larger that 0, size is adjusted later
        
        while( ptr.Length() > 0 && error == KErrNone )
            {
            error = aFile.Read( ptr );
            if ( ptr.Length() > 0 && error == KErrNone)
                {
                error = attaFile.Write( ptr );
                }
            }
        if ( error == KErrNone )
            {
            error = attaFile.Flush();
            }
        
        CleanupStack::PopAndDestroy( buffer );
        buffer = NULL;
        }
        
    // we must alway close    
    CleanupStack::PopAndDestroy( &attaFile ); // close attaFile
    
    // Now actual datafile is ready.
    // We still have the atta info, and we must store the mimeheaders
    
    aMimeHeaders.StoreL( *aAttachmentInfo );
    
    // Now all should be ready. 
    // Caller must commit store (maybe headers still need to be changed,
    // or maybe several attachments are added before committing store)
    
    User::LeaveIfError( error );
    }
    
// ---------------------------------------------------------
// CMmsAttachmentHandler::CreateTextAttachmentL
// ---------------------------------------------------------
EXPORT_C void CMmsAttachmentHandler::CreateTextAttachmentL(
    CMsvStore& aStore,
    TMsvAttachmentId& aAttachmentId,
    const TDesC& aText,
    const TDesC& aFile,
    RFs& aFs,
    TDriveUnit aMessageDrive,
    TBool aConvertParagraphSeparator /*= ETrue*/ )
    {
    
    HBufC* convertedText = NULL;
    TPtrC text;
    
    if ( aConvertParagraphSeparator )
        {
        convertedText = CMsgTextUtils::ConvertParagraphSeparatorsLC( aText );
        text.Set( convertedText->Des() );
        }
    else
        {
        text.Set( aText );
        }
    
    const TInt KMmsMaxBytesPerCharacter = 4;    
	// coverity[incorrect_multiplication][buffer_alloc]
    HBufC8* buffer = HBufC8::NewL( text.Length() * KMmsMaxBytesPerCharacter ); // paranoid.
    CleanupStack::PushL( buffer );
    TPtr8 buf8 = buffer->Des();

    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL( mimeHeaders );

    // attaInfo must be on top of stack because the ownership will be transferred
    // to attacment manager.    
    CMsvAttachment* attaInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
    CleanupStack::PushL( attaInfo );
    
    TPtrC8 contentType;
    contentType.Set( KMmsTextPlain );
    
    TInt position = contentType.Find( KMmsSlash8 );
    mimeHeaders->SetContentTypeL( contentType.Left( position ) );
    mimeHeaders->SetContentSubTypeL( contentType.Mid( position + 1 ) );
    attaInfo->SetMimeTypeL( contentType );
    attaInfo->SetAttachmentNameL( aFile );
    
    mimeHeaders->SetMimeCharset( KMmsUtf8 );
    mimeHeaders->SetSuggestedFilenameL( aFile );
    
    // if conversion fails, something is really seriously wrong
    TInt error = CnvUtfConverter::ConvertFromUnicodeToUtf8( buf8, text );
  
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &aFs,
        buf8.Length() + mimeHeaders->Size() + KMmsIndexEntryExtra,
        aMessageDrive ) )
        {
        // we use standard error code here
        User::Leave( KErrDiskFull );
        }
    else
        {
        User::LeaveIfError( error );    
        }
        
    attaInfo->SetSize( buf8.Length() );
    mimeHeaders->StoreL( *attaInfo ); // Mime headers are streamed into atta info

    MMsvAttachmentManagerSync& attaManSync = aStore.AttachmentManagerExtensionsL();
    
    RFile attaFile;
    attaManSync.CreateAttachmentL( aFile, attaFile, attaInfo );
    CleanupStack::Pop( attaInfo ); // attaInfo ownership was transferred.
    aAttachmentId = attaInfo->Id();

    // Now our file handle is open for writing
    
    if ( buf8.Length() > 0 )
        {
        attaFile.Write( buf8 );
        error = attaFile.Flush();
        }
    attaFile.Close();
    
    if ( error != KErrNone )
        {
        // Something went wrong when we tried to write our data.
        // We must delete the attachment as it does not contain the
        // intended data.
        RemoveAttachmentL( aAttachmentId, aStore );
        aAttachmentId = 0;
        }

    CleanupStack::PopAndDestroy( mimeHeaders );
    CleanupStack::PopAndDestroy( buffer );
    
    if ( convertedText )
        {
        CleanupStack::PopAndDestroy( convertedText );
        convertedText = NULL;
        }
        
    User::LeaveIfError( error );    
        
    }
    
// ---------------------------------------------------------
// CMmsAttachmentHandler::CreateUTF8TextAttachmentFromFileL
// ---------------------------------------------------------
EXPORT_C void CMmsAttachmentHandler::CreateUTF8TextAttachmentFromFileL(
    CMsvStore& aStore,
    TMsvAttachmentId& aAttachmentId,
    RFile& aFile,
    RFs& aFs,
    TDriveUnit aMessageDrive )
    {
    
    _LIT8 ( KMmsCrLf8, "\x00D\x00A" ); // 8 bit line feed
    TInt size = 0;
    TInt error = KErrNone;
    error = aFile.Size( size );
    
    User::LeaveIfError( error ); // if can't get file size, we are in trouble

    TFileName* filename = new( ELeave ) TFileName;
    CleanupStack::PushL( filename );
    
    // 256 characters for each read
    HBufC* textBuffer = HBufC::NewL( KMmsTextBufferSize );
    CleanupStack::PushL( textBuffer );
    TPtr textPtr = textBuffer->Des();

    HBufC8* buffer = HBufC8::NewL( KMmsTextBufferSize * KMmsMaxBytesPerCharacter ); // paranoid.
    TInt fileSize = 0; // we don't know how big the file will be after conversion
    CleanupStack::PushL( buffer );
    TPtr8 buf8 = buffer->Des();

    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL( mimeHeaders );

    // attaInfo must be on top of stack because the ownership will be transferred
    // to attacment manager.    
    CMsvAttachment* attaInfo = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attaInfo );
    
    TPtrC8 contentType;
    contentType.Set( KMmsTextPlain );
    
    TInt position = contentType.Find( KMmsSlash8 );
    mimeHeaders->SetContentTypeL( contentType.Left( position ) );
    mimeHeaders->SetContentSubTypeL( contentType.Mid( position + 1 ) );
    attaInfo->SetMimeTypeL( contentType );
    
    filename->Copy( TPtrC() );
  	aFile.Name( *filename ); // if this returns error, filename should be empty - no suggestion.
    attaInfo->SetAttachmentNameL( *filename );
    mimeHeaders->SetSuggestedFilenameL( *filename );
    mimeHeaders->SetMimeCharset( KMmsUtf8 );
    
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &aFs,
        size * KMmsUnicodeToUtf2MaxIncrease + mimeHeaders->Size() + KMmsIndexEntryExtra,
        aMessageDrive ) )
        {
        // we use standard error code here
        User::Leave( KErrDiskFull );
        }
       
    mimeHeaders->StoreL( *attaInfo ); // Mime headers are streamed into atta info

    MMsvAttachmentManagerSync& attaManSync = aStore.AttachmentManagerExtensionsL();
    
    RFile attaFile;
    attaManSync.CreateAttachmentL( *filename, attaFile, attaInfo );
    CleanupStack::Pop( attaInfo ); // attaInfo ownership was transferred.
    aAttachmentId = attaInfo->Id();

    // Now our file handle is open for writing
    
    error = KErrNone;
    TMmsFileText textFile;
    textFile.Set( aFile );

    while ( error == KErrNone || error == KErrTooBig )
        {
        error = textFile.Read( textPtr );
        TBool appendCRLF = ETrue;
        if ( error == KErrTooBig )
            {
            appendCRLF = EFalse;
            error = KErrNone;
            }
        if ( error != KErrEof )
            {
            // if conversion fails, something is really seriously wrong
            error = CnvUtfConverter::ConvertFromUnicodeToUtf8( buf8, textPtr );
            }
        if ( error == KErrNone )
            {
            error = attaFile.Write( buf8 );
            if ( error == KErrNone )
                {
                fileSize += buf8.Length();
                if ( appendCRLF )
                    {
                    error = attaFile.Write( KMmsCrLf8 );
                    fileSize += KMmsLengthOfCRlf; // add length of carriage return/line feed
                    }
                }
            }
        }
        
    if ( error == KErrEof )
        {
        // end of file has been reached successfully
        error = KErrNone;
        }

    if ( error == KErrNone )
        {
        error = attaFile.Flush();
        }
    attaFile.Close();
    
    if ( error != KErrNone )
        {
        // Something went wrong when we tried to write our data.
        // We must delete the attachment as it does not contain the
        // intended data.
        RemoveAttachmentL( aAttachmentId, aStore );
        aAttachmentId = 0;
        }
    else
        {
        // If data writing was successful, the amount of data written
        // is now stored in fileSize.
        // Attachment info structure must be updated
        MMsvAttachmentManager& attaMan = aStore.AttachmentManagerL();
        attaInfo = attaMan.GetAttachmentInfoL( aAttachmentId );
        CleanupStack::PushL( attaInfo );
        attaInfo->SetSize( fileSize );
        attaManSync.ModifyAttachmentInfoL( attaInfo );
        // attachment manager now owns the attachment info
        CleanupStack::Pop( attaInfo ); // attaInfo
        }
        
    CleanupStack::PopAndDestroy( mimeHeaders );
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( textBuffer );
    CleanupStack::PopAndDestroy( filename );
    
    User::LeaveIfError( error );
    
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
void CMmsAttachmentHandler::RemoveAttachmentL( TMsvAttachmentId aAttaId, CMsvStore& aStore )
    {
    MMsvAttachmentManager& attaMan = aStore.AttachmentManagerL();
    MMsvAttachmentManagerSync& attaManSync = aStore.AttachmentManagerExtensionsL();

    // can only remove synchronously if index is known.
    TInt count = attaMan.AttachmentCount();
    
    TInt i = count - 1;
    TBool found = EFalse;
    while ( i >= 0 && !found )
        {
        CMsvAttachment* attachmentInfo = attaMan.GetAttachmentInfoL( i );
        CleanupStack::PushL( attachmentInfo );
        if ( attachmentInfo->Id() == aAttaId )
            {
            found = ETrue;
            }
        else
            {
            i--;
            }
        CleanupStack::PopAndDestroy( attachmentInfo );    
        attachmentInfo = NULL;
        }
    if ( i >= 0 && found )
        {
        attaManSync.RemoveAttachmentL( i );
        }
    }
    

// Helper class that is used instead of TFileText
// because the TFileText does not behave like we want it to behave    
    
// ---------------------------------------------------------
// Default constructor.
// ---------------------------------------------------------
//
TMmsFileText::TMmsFileText()
	{}

// ---------------------------------------------------------
// Sets the file to be read from
// ---------------------------------------------------------
//
void TMmsFileText::Set( RFile& aFile )
	{
    iFile = aFile;
    iReadBuf.Zero();
    iNext = ( TText* )iReadBuf.Ptr();
    iEnd = iNext;
    TInt pos = 0;
    iFile.Seek( ESeekStart, pos );
    iState = EStartOfFile;
	}

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt TMmsFileText::Read( TDes& aDes )
/**
Reads single line text record into the specified descriptor.

The read operation begins at the current file position, and ends when
a line delimiter character is read or the caller's buffer is full or
the file ends;

If the line is longer than fits into user's buffer, of if the file does
not end with a terminator, KErrTooBig is returned.
The purpose is to inform the caller that a terminator should not be added
to the line when it is written elsewhere.

Next time the reading continues from the current position so that a long
line may be read in chunks and terminator added when the end of the line
has been reached.

If Read() is called when the current position is the end of the file (that 
is, after the last line delimiter in the file), KErrEof is returned, and the 
length of the buffer is set to zero.

@param aDes On return, contains the single record read from the file. Any 
            previous contents are overwritten.

@return KErrNone if successful, otherwise one of the other system-wide error 
        codes. KErrTooBig indicates that the line does not end with a
        terminator. Buffer is too short to hold the whole line or the line
        is the last line in the file and the file does not end with a 
        terminator character.
*/
	{
	TText* pD = ( TText* )aDes.Ptr();
	TInt len = aDes.MaxLength();
	TInt newLen = 0;
	TInt r = KErrNone;
    TBool terminate = EFalse;
	while ( newLen < len )
		{
		if ( iNext >= iEnd )
			{
			r = FillBuffer();
			if ( r != KErrNone && r != KErrEof )
			    {
				return r;
			    }
			if ( r == KErrEof )
				{
				aDes.SetLength( newLen );
				return ( newLen ? KErrTooBig : KErrEof );
				}
			continue;
			}
		terminate = newLen;
		r = CheckForTerminator( terminate );
		if ( r != KErrNone || terminate)
			{
			aDes.SetLength( newLen );
			return r;
			}
		*pD++ = ( *iNext++ );
		newLen++;
		}
	aDes.SetLength( newLen );
	terminate = newLen;
	r=CheckForTerminator( terminate );
	if ( r != KErrNone || terminate )
	    {
		return r;
	    }
// don't skip the rest of the line - return the rest the next time.
	return KErrTooBig;
	}

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
static void SwapWords( TText* aStart, TInt aCount )
 	{
 	TUint8* p = ( TUint8* )aStart;
 	while ( aCount-- > 0 )
 		{
 		TUint8 temp = *p;
 		*p = p[1];
 		p[1] = temp;
 		p += 2;
   		}
   	}

// ---------------------------------------------------------
// Read the new data from the file
// ---------------------------------------------------------
//
TInt TMmsFileText::FillBuffer()
	{
	TInt r = iFile.Read( iReadBuf );
	if ( r !=KErrNone )
	    {
		return r;
	    }
	if ( iReadBuf.Length() == 0 )
	    {
		return KErrEof;
	    }
	iNext = ( const TText* )iReadBuf.Ptr();
	iEnd = iNext + iReadBuf.Length() / sizeof( TText );
	 
 	// Use any leading byte order marker to determine endianness.
 	if ( iState == EStartOfFile )
 		{
 		iState = ENormal;

 		// Ignore an ordinary byte order marker.
 		if ( *iNext == 0xFEFF )
 		    {
 			iNext++;
 		    }

 		// Set the endianness state to 'reverse' if a reversed byte order marker is found.
 		else if ( *iNext == 0xFFFE )
 			{
 			iNext++;
 			iState = EReverse;
 			}
 
 		if ( iNext == iEnd )
 		    {
 			return KErrEof;
 		    }
 		}
 
 	if ( iState == EReverse )
 	    {
		SwapWords( ( TText* )iNext, ( iEnd - iNext ) );
 	    }

	return KErrNone;
	}

// ---------------------------------------------------------
// Return ETrue if the next char is a record terminator: PARAGRAPH SEPARATOR (U+2029), LINE SEPARATOR (U+2028),
// CR-LF (U+000D, U+000A), or LF (U+000A)
// If the file ends without terminator, return KErrTooBig
// KErrTooBig actually only means that the line does not end with a terminator
// ---------------------------------------------------------
//
TInt TMmsFileText::CheckForTerminator( TBool& anAnswer )
	{
	TInt r = KErrNone;
	if ( iNext >= iEnd )
		{
		r = FillBuffer();
		if ( r != KErrNone )
			{
			if ( r == KErrEof && anAnswer )
			    {
				return KErrTooBig; // no terminator
			    }
			return r;
			}
		}

	anAnswer = EFalse;
	const TText* oldNext = iNext;
	TInt oldBufferLength = iReadBuf.Length();
	TText c = ( *iNext );
	TBool peek = EFalse;

	// Check for unambiguous paragraph or line separator.
 	if ( c == 0x2029 || c == 0x2028 )
 		{
 		iNext++;
 		anAnswer = ETrue;
		return KErrNone;
 		}
 
 	// Check for CR-LF or LF.
 	if ( c == 0x000D )
		{
		iNext++;
		if ( iNext < iEnd )
		    {
			c = ( *iNext );
		    }
		else
			{
			peek = ETrue;
			r = FillBuffer();
			if ( r != KErrNone && r != KErrEof )
			    {
				return r;
			    }
			if ( r == KErrNone )
			    {
				c = ( *iNext );
			    }
			}
		}

	if ( c == 0x000A )
		{
		iNext++;
		anAnswer = ETrue;
		return KErrNone;
		}

	iNext = oldNext;
	if ( !peek )
	    {
		return KErrNone;
	    }

	TInt pos = ( -1 ) * ( oldBufferLength + iReadBuf.Length() );
	r = iFile.Seek( ESeekCurrent, pos );
	if ( r == KErrNone )
	    {
		r = FillBuffer();
	    }
	if ( r != KErrNone )
	    {
		return r;
	    }
	iNext = oldNext;
	return KErrNone;
	}
    
// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  
