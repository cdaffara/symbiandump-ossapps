/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef CIMAGEPRINTSERVER_H
#define CIMAGEPRINTSERVER_H

#include <e32base.h>
#include <fbs.h>

class CImagePrintBody;

// ----------------------------------------------------------------------------------------
// Server's policy here
// ----------------------------------------------------------------------------------------

// Total number of ranges
const TUint KImagePrintRangeCount = 2;

// Definition of the ranges of IPC numbers
const TInt KImagePrintRanges[KImagePrintRangeCount] = 
    {
    0,	// implemented 0-38
    39 	// non implemented function end of range check ; ENotSupported        
    }; 

// Policy to implement for each of the above ranges        
const TUint8 KImagePrintElementsIndex[KImagePrintRangeCount] = 
    {
    CPolicyServer::EAlwaysPass,		// applies to 0th range        
    CPolicyServer::ENotSupported  	// applies to 1st range (out of range IPC)
    };

// Package all the above together into a policy
const CPolicyServer::TPolicy KImagePrintPolicy =
    {
    CPolicyServer::EAlwaysPass, 	// specifies all connect attempts should pass
    KImagePrintRangeCount,			// number of ranges                                   
    KImagePrintRanges,				// ranges array
    KImagePrintElementsIndex,		// elements<->ranges index
    NULL,							// array of elements
    };
class CImagePrintServer : public CPolicyServer
	{
	public:

		/**
		 *	2-phase constructor
		 *
		 *	@return new object. The object is left on the cleanup stack
		 */
		static CImagePrintServer* NewLC();
		
		/**
		 *	Destructor
		 */
		~CImagePrintServer();
		
		/**
		 *	Increase session counter
		 */
		void AddSession();
		
		/**
		 *	Decrease session counter. If the counter is decreased to 0, the
		 *	server is shut down
		 */
		void RemoveSession();
		
		/**
		 *	Session count getter
		 *
		 *	@return session count
		 */
		TInt SessionCount() const;
		
		/**
		 *	Reserve printing engine to the session
		 *
		 *	@param aSession session that claims the printing engine
		 *	@return error code
		 */
		TInt ReserveEngine( const CSession2* aSession );
		
		/**
		 *	Release printing engine by the session
		 *
		 *	@param aSession session that wants to release the printing engine
		 *	@return error code
		 */
		TInt ReleaseEngine( const CSession2* aSession );
		
		/**
		 *	Printing engine getter
		 *
		 *	@return printing engine
		 */
		CImagePrintBody& Engine() const;

	private:
		
		TInt RunError( TInt aError );
		CImagePrintServer();
		void ConstructL();		
		CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;		

	private:

		TInt iConnsCount;
		
		const CSession2* iEngineHolder;
		
		CImagePrintBody* iEngine;
				
		RFbsSession iFbs;
	};


#endif // CIMAGEPRINTSERVER_H

//  End of File
