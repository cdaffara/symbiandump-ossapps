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
* Description:  Container store encryption.
*
*/



#ifndef __CONTAINER_STORE_ENCRYPTION_H__
#define __CONTAINER_STORE_ENCRYPTION_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
#include <s32strm.h>
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

#include "ContainerStoreGeneralTable.h"

class CCryptoBase;

// =======
// CLASSES
// =======

// ================================ 
// CLASS: CContainerStoreEncryption
// ================================ 
class CContainerStoreEncryption : public CBase
	{
	public:

        // ==============
        // PUBLIC METHODS
        // ==============
        		
		static CContainerStoreEncryption* NewL();
		
		virtual ~CContainerStoreEncryption();
		
		TBool Authenticated();
		
		void ClearAuthentication();

		void CreateAuthenticationBufferL( const TDesC& aPassword, RBuf8& aAuthenticationBuffer );
		
		TBool AuthenticateL( const TDesC& aPassword, const TDesC8& aAuthenticationBuffer );

        // Leaves if the store isn't authenticated
        void CheckForAuthenticationL();
        
        //This variable is kept in this class for performance
        TBool IsAuthenticationRequired() { return iAuthenticationRequired; };
        
        void SetAuthenticationRequired( TBool aFlag ) { iAuthenticationRequired = aFlag; };
					
        TBool IsEncryptionOn(){ return iEncryptionFlag; } ;
        
        void SetEncryptionFlag( TBool aFlag ) { iEncryptionFlag = aFlag; };
        
        void DecryptL( const TDesC8& aEncryptedBuffer,
                    TDes8&        aUnencryptedBuffer );
        
        void EncryptL( const TDesC8& aUnencryptedBuffer,
                    TDes8&        aEncryptedBuffer );
        
        void AddPaddingL( TDes8& aBuffer,
                       TInt   aBlockLength );
                       
        void RemovePaddingL( TDes8& aBuffer );
        
        // All input buffers must be a multiple of this block size.
        TInt BlockSizeL();
        
	private:
	
        // ===============
        // PRIVATE METHODS
        // ===============
        		
		CContainerStoreEncryption();	
		
		void PasswordToEncryptionKeyL( const TDesC& aPassword, RBuf8& aEncryptionKey );
		
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
        		
		CCryptoBase* iCryptoLibrary;
		
        TBool        iAuthenticationRequired;
		TBool        iEncryptionFlag;  //encryption on/off
		
		__LOG_DECLARATION
		
	}; // end class CContainerStoreEncryption

#endif
