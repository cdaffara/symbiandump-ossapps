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
* Description:  Container store general table.
*
*/



#ifndef __CONTAINER_STORE_GENERAL_TABLE_H__
#define __CONTAINER_STORE_GENERAL_TABLE_H__

// ========
// INCLUDES
// ========

#include <e32base.h>
#include <d32dbms.h>  // database
//<cmail>
#include "DebugLogMacros.h"
//</cmail>

#include "ContainerStoreTable.h"
#include "ContainerStore.h"
#include "ContainerStoreDefs.h"

// =======
// CLASSES
// =======

// ==================================
// CLASS: CContainerStoreGeneralTable
// ==================================
class CContainerStoreGeneralTable : public CContainerStoreTable
	{
	public:
	
	    // ==============
	    // PUBLIC METHODS
	    // ==============
	
		static CContainerStoreGeneralTable* CreateL( CContainerStoreUtils& aUtils );
		
		static CContainerStoreGeneralTable* OpenL( CContainerStoreUtils& aUtils );
		
		virtual ~CContainerStoreGeneralTable();
		
		TContainerId AssignNextIdL();
        
        TBool IsAuthenticateRequiredL();
        
        void SetAuthenticationRequiredL( TBool aFlag );
		
		//Check if the encryption flag is on
		TBool IsEncryptionOnL();
		
		//Turn encryption flag on/off
		void  SetEncryptionFlagL( TBool aFlag );
				
		TBool AuthenticationDataPresentL();
		
		void SetAuthenticationDataL( const TDesC8& aBuffer );		
		
		void GetAuthenticationDataL( RBuf8& aBuffer );
		
		//gets the encryption progress state
        TEncryptionState EncryptionStateL();
		
		//sets the encryption progress state
		void SetEncryptionStateL( TEncryptionState aEncryptionState );
        
        TUint MaxMruCountL();
        
        void SetMaxMruCountL( TUint aCount );
        
        TUint CurMruNumberL();
        
        void SetCurMruNumberL( TUint aNum );
		
	private:

        // ===============
        // PRIVATE METHODS
        // ===============
        	
		CContainerStoreGeneralTable( CContainerStoreUtils& aUtils );
		
		void OpenTableL();
		
		void CreateTableL();
				
        // ==================
        // PRIVATE ATTRIBUTES
        // ==================
	    TInt                  iNextIdColNum;
        TInt                  iAuthenticationRequiredColNum;   
	    TInt                  iAuthenticationDataColNum;
	    TInt				  iEncryptionFlagColNum;     //on/off
	    TInt                  iEncryptionStateColNum;   //TEncryptionState
        TInt                  iMaxMruCountColNum;       //Most Recently Used address count
        TInt                  iCurMruNumberColNum;      //Current MRU number
        
	    __LOG_DECLARATION
    		
	}; // end class CContainerStoreGeneralTable

#endif
