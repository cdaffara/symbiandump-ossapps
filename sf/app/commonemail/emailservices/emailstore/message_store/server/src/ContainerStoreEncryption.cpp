/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Container store encryption implementation.
*
*/



// ========
// INCLUDES
// ========

#include <bautils.h>
#include <utf.h>

#include "ContainerStoreEncryption.h"
#include "ContainerStoreUtils.h"

class CCryptoBase: public CBase {};

// =========
// CONSTANTS
// =========

// This length is more than sufficient.
const TInt KMaxAuthenticationBufferLength = 100;

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// FUNCTION: NewL
// ==========================================================================
CContainerStoreEncryption* CContainerStoreEncryption::NewL()
	{
	CContainerStoreEncryption* self = new(ELeave) CContainerStoreEncryption;
	return self;
	} // end NewL
	
// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CContainerStoreEncryption::CContainerStoreEncryption() 
: iAuthenticationRequired( EFalse ), iEncryptionFlag (EFalse)
	{
	__LOG_CONSTRUCT( "msg", "CContainerStoreEncryption" )
	__LOG_CLOSE_BETWEEN_WRITES
	} // end constructor
	
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CContainerStoreEncryption::~CContainerStoreEncryption()
	{
    ClearAuthentication();
	__LOG_DESTRUCT
	} // end destructor
	
// ==========================================================================
// FUNCTION: Authenticated
// ==========================================================================
TBool CContainerStoreEncryption::Authenticated()
    {
    //always return true if authentication is not required
    return ( iCryptoLibrary || !iAuthenticationRequired ); 
    } // end Authenticated

// ==========================================================================
// FUNCTION: ClearAuthentication
// ==========================================================================
void CContainerStoreEncryption::ClearAuthentication()
    {
    delete iCryptoLibrary;
    iCryptoLibrary = NULL;
    } // end ClearAuthentication  

// ==========================================================================
// FUNCTION: CreateAuthenticationBufferL
// ==========================================================================
void CContainerStoreEncryption::CreateAuthenticationBufferL( const TDesC& aPassword, 
                                                             RBuf8&       aAuthenticationBuffer )
    {
    RBuf8 unencryptedAuthenticationBuffer;
    unencryptedAuthenticationBuffer.CreateL( KMaxAuthenticationBufferLength );
    CleanupClosePushL( unencryptedAuthenticationBuffer );
    
    if( !iCryptoLibrary )
        {        
        iCryptoLibrary = new(ELeave) CCryptoBase;
        } // end if
    
    const TUint size = 5; 
    HBufC8* key = HBufC8::NewL(size);
    _LIT8( K12345, "12345" );
    key->Des().Copy( K12345 );
    
    // Add the encryption key to the authentication buffer.    
    unencryptedAuthenticationBuffer.Append( *key );
    delete key;

    RBuf8 sha1Checksum;
    CleanupClosePushL( sha1Checksum );

    // Use the password length as the "SHA-1 Checksum"
    sha1Checksum.CreateL( 1 );
    sha1Checksum.Append( aPassword.Length() );   
    
    // Add the SHA-1 checksum of the key to the authentication buffer.
    unencryptedAuthenticationBuffer.Append( sha1Checksum );

    // Add the length of the SHA-1 checksum to the authentication buffer.
    unencryptedAuthenticationBuffer.Append( TUint8(sha1Checksum.Length()) );
    
    CleanupStack::PopAndDestroy( &sha1Checksum );   
    
    // Add PKCS5 padding to the buffer.
    AddPaddingL( unencryptedAuthenticationBuffer, BlockSizeL() );
                
    // Encrypt the buffer using the given password.
    aAuthenticationBuffer.ReAllocL( unencryptedAuthenticationBuffer.Length() );                
    aAuthenticationBuffer.Copy( unencryptedAuthenticationBuffer );

    CleanupStack::PopAndDestroy( &unencryptedAuthenticationBuffer );
    
    } // end DoGenerateAuthenticationBufferL

// ==========================================================================
// FUNCTION: AuthenticateL
// ==========================================================================
TBool CContainerStoreEncryption::AuthenticateL( const TDesC&  aPassword, 
                                                const TDesC8& aAuthenticationBuffer )
    {
    TBool returnValue = EFalse;
    
    RBuf8 decryptedBuffer;
    CleanupClosePushL( decryptedBuffer );

    decryptedBuffer.CreateL( aAuthenticationBuffer.Length() );
    decryptedBuffer.Copy( aAuthenticationBuffer );
    TInt result = KErrNone;

    if( result == KErrNone )
        {            
        // Verify that the PKCS5 padding is still intact.
        TUint8 padLength = decryptedBuffer[decryptedBuffer.Length()-1];
    
        TBool paddingOk = (padLength >= 1 && padLength < aAuthenticationBuffer.Length() );
    
        for( TInt i = 0; paddingOk && i < padLength; i++ )
            {
            paddingOk = decryptedBuffer[decryptedBuffer.Length()-i-1] == padLength;
            } // end for  
            
        // Verify the SHA-1 checksum of the encryption key.            
        if( paddingOk && (padLength < decryptedBuffer.Length()) )
            {
            // Remove the pad characters.
            decryptedBuffer.SetLength( decryptedBuffer.Length() - padLength );

            // Get the length of the checksum.
            TUint8 checksumLength = decryptedBuffer[decryptedBuffer.Length()-1];
            decryptedBuffer.SetLength( decryptedBuffer.Length() - 1 ); 
            
            if( checksumLength < decryptedBuffer.Length() )
                {
                // Create a pointer to the encryption key.
                TPtrC8 encryptionKey( decryptedBuffer.Left( decryptedBuffer.Length() - checksumLength ) );
            
                // Compute the expected SHA-1 checksum.
                RBuf8 expectedChecksum;
                CleanupClosePushL( expectedChecksum );
                expectedChecksum.CreateL(1);
                expectedChecksum.Append( aPassword.Length() );                                    

                // Verify the SHA-1 checksum.
                if( expectedChecksum.Compare(decryptedBuffer.Right(checksumLength)) == 0 )
                    {
                    // Password is OK.
                    returnValue = ETrue;
                                    
                    // Save the encryption key for later use.
                    if( !iCryptoLibrary )
                        {
                        iCryptoLibrary = new(ELeave) CCryptoBase;
                        } // end if

                    } // end if

                CleanupStack::PopAndDestroy( &expectedChecksum );
                
                } // end if

            } // end if
        
        } // end if
            
    CleanupStack::PopAndDestroy( &decryptedBuffer );

    return returnValue;
    
    } // end AuthenticateL

// ==========================================================================
// FUNCTION: CheckForAuthenticationL
// ==========================================================================
void CContainerStoreEncryption::CheckForAuthenticationL()
    {    
    __LOG_ENTER_SUPPRESS( "CheckForAuthenticationL" )
    
    //leave if authentication is required is ON and library is NULL
	if( !iCryptoLibrary && iAuthenticationRequired )
	    {
	    __LOG_WRITE_ERROR( "not authenticated" )
	    User::Leave( KErrNotReady );
	    } // end if
	    
    } // end CheckForAuthenticationL

// ==========================================================================
// FUNCTION: DecryptL
// ==========================================================================
void CContainerStoreEncryption::DecryptL( const TDesC8& aEncryptedBuffer,
                                          TDes8&        aUnencryptedBuffer )
    {
    __LOG_ENTER_SUPPRESS( "DecryptL" )
    
    CheckForAuthenticationL();

    __LOG_HEX_DUMP_DEBUG1( "before", aEncryptedBuffer )
            
    EncryptL( aEncryptedBuffer, aUnencryptedBuffer );
    
    __LOG_HEX_DUMP_DEBUG1( "after", aUnencryptedBuffer )
            
    } // end DecryptL

// ==========================================================================
// FUNCTION: EncryptL
// ==========================================================================
void CContainerStoreEncryption::EncryptL( const TDesC8& aUnencryptedBuffer,
                                          TDes8&        aEncryptedBuffer )
    {
    __LOG_ENTER_SUPPRESS( "EncryptL" )
    
    CheckForAuthenticationL();

    __LOG_HEX_DUMP_DEBUG1( "before", aUnencryptedBuffer )
            
    // Use a dummy encryption method (i.e. just XOR a fixed value to each byte).
    aEncryptedBuffer.SetLength( 0 );    
    for( TInt i = 0 ; i < aUnencryptedBuffer.Length(); i++ )
        {
        aEncryptedBuffer.Append( aUnencryptedBuffer[i] ^ 0xA7 );        
        }
        
    __LOG_HEX_DUMP_DEBUG1( "after", aEncryptedBuffer )
            
    } // end EncryptL

// ==========================================================================
// FUNCTION: RemovePaddingL
// ==========================================================================
void CContainerStoreEncryption::RemovePaddingL( TDes8& aBuffer )
    {
    TUint8 padChar = aBuffer[aBuffer.Length()-1];
    
    TInt newLength = aBuffer.Length() - padChar;        
    
    if( newLength < 0 || aBuffer[newLength] != padChar )
        {
        User::Leave( KErrCorrupt );
        }
    else
        {            
        aBuffer.SetLength( newLength );        
        } // end if
    
    } // end RemovePaddingL
        
// ==========================================================================
// FUNCTION: AddPaddingL
// ==========================================================================
void CContainerStoreEncryption::AddPaddingL( TDes8& aBuffer,
                                             TInt   aBlockLength )
    {
    TUint8 padChar = aBlockLength - (aBuffer.Length() % aBlockLength);        
    
    aBuffer.SetLength( aBuffer.Length() + padChar );
    aBuffer.RightTPtr(padChar).Fill( padChar );
        
    } // end AddPaddingL

// ==========================================================================
// FUNCTION: BlockSizeL
// ==========================================================================
TInt CContainerStoreEncryption::BlockSizeL()
    {
    CheckForAuthenticationL();
    
    const TUint KBlockSize = 16;
    return KBlockSize;    
    } // end BlockSize

// ==========================================================================
// FUNCTION: PasswordToEncryptionKeyL
// ==========================================================================
void CContainerStoreEncryption::PasswordToEncryptionKeyL( const TDesC& aPassword, RBuf8& aEncryptionKey )
    {
    const TInt   KMinimumEncryptionKeyLength = 16;
    const TUint8 KPadChar                    = 'x';

    // Allocate the encryption key buffer.  (Not that aPassword.Length * 2 is used to account for Unicode
    // to UTF conversions.  In some worst case situations, the password could be truncated if certain
    // characters are present, but that is OK for this authentication logic).
    aEncryptionKey.CreateL( Max( aPassword.Length() * 2, KMinimumEncryptionKeyLength ) );

    // Convert the password from Unicode to UTF-8.
   	CnvUtfConverter::ConvertFromUnicodeToUtf8( aEncryptionKey, aPassword );
	
	// Pad the password to the minimum length, if needed.
    if( aEncryptionKey.Length() < KMinimumEncryptionKeyLength )
        {        
        TInt padLength = KMinimumEncryptionKeyLength - aEncryptionKey.Length();
        
        aEncryptionKey.SetLength( KMinimumEncryptionKeyLength );
        aEncryptionKey.RightTPtr(padLength).Fill( KPadChar );
        } // end if

    } // end PasswordToEncryptionKeyL
