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
*
*/


// Protection against nested includes
#ifndef CDIRECTPRINTSERVER_H
#define CDIRECTPRINTSERVER_H

// System include files
#include <e32base.h>
#include <fbs.h>

class CDirectPrintBody;

// ----------------------------------------------------------------------------------------
// Server's policy here
// ----------------------------------------------------------------------------------------

// Total number of ranges
const TUint KDirectPrintRangeCount = 2;

// Definition of the ranges of IPC numbers
const TInt KDirectPrintRanges[KDirectPrintRangeCount] = 
    {
    0,	// implemented 0-38
    41 	// non implemented function end of range check ; ENotSupported        
    }; 

// Policy to implement for each of the above ranges        
const TUint8 KDirectPrintElementsIndex[KDirectPrintRangeCount] = 
    {
    CPolicyServer::EAlwaysPass,		// applies to 0th range        
    CPolicyServer::ENotSupported  	// applies to 1st range (out of range IPC)
    };

// Package all the above together into a policy
const CPolicyServer::TPolicy KDirectPrintPolicy =
    {
    CPolicyServer::EAlwaysPass, 	// specifies all connect attempts should pass
    KDirectPrintRangeCount,			// number of ranges                                   
    KDirectPrintRanges,				// ranges array
    KDirectPrintElementsIndex,		// elements<->ranges index
    NULL,							// array of elements
    };
class CDirectPrintServer : public CPolicyServer
	{
	public:

		/**
		 *	2-phase constructor
		 *
		 *	@return new object. The object is left on the cleanup stack
		 */
		static CDirectPrintServer* NewLC();
		
		/**
		 *	Destructor
		 */
		~CDirectPrintServer();
		
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
		CDirectPrintBody& Engine() const;

	private:
		
		TInt RunError( TInt aError );
		CDirectPrintServer();
		void ConstructL();		
		CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;		

	private:

		TInt iConnsCount;
		
		const CSession2* iEngineHolder;
		
		CDirectPrintBody* iEngine;
				
		RFbsSession iFbs;
	};


#endif // CDIRECTPRINTSERVER_H

//  End of File
