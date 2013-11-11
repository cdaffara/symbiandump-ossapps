/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  mmsconninit  declaration
*
*/



#ifndef __MMSCONNINIT_H
#define __MMSCONNINIT_H

//  INCLUDES

#include "MmsConnInitLogging.h"

#include <e32base.h>
#include <es_sock.h>
#include <commdbconnpref.h>
#include <commdb.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class RConnection;

// CLASS DECLARATION
/**
* This class provides an interface for clients wishing to initiate an 
* Internet connection. Also function solving parameters needed in HTTP transport
* is offered.
*/
class CMmsConnectionInitiator : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMmsConnectionInitiator* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CMmsConnectionInitiator();

    public: // New functions

        /**
        * Starts the connection procedure based on the Accesspoint link/refence
        * 
        * @param RConnection conn (in)
        * @param CArrayFixFlat<TUint32>& aAccessPointArray (in)
        * @param aStatus (out)
        *   aStatus is set to an error value in case of error.
        *   In successful case aStatus is set to point to index of
        *   aAccessPointArray (actually index+1). The index specifies the AP
        *   that has been used for connection.
        *
        * Usage:
        * Client opens RConnection session and passes
        * a refernce parameter to MmsConnInit. Client
        * allocates an array containing WAP AP(s). The
        * client is responsible for allocation and
        * deallocation of memory
        */    
        IMPORT_C void ConnectL(  
            RConnection&  aConnection,
            CArrayFixFlat<TUint32>& aAccessPointArray, 
            TRequestStatus& aStatus );
        
        /**
        * Finds parameters needed for client using
        * HTTP Client API. Used also for access point validation
        * 
        * @param TUint32 aAccessPointLink (in) 
        * @param TUint32& aAccessPoint (out)
        * @param HBufC*& aServerUri (out)
        * @param TBool& aProxyUsed (out)
        * @param HBufC8*& aProxyAddress (out)
        *
        * Usage:
        * Link/reference to accesspoint is given as an input parameter.
        * The function resolves the other parameters based on the accesspoint
        * reference.
        * aUri and aProxyAddress are references to heap buffers. They should
        * not be allocated by client. Client is, however, responsible for
        * deallocating them when no longer needed.
        * Note that the buffer parameters may not be members of the client
        * because MmsConnInit puts them into CleanupStack temporarily
        * The function tries to solve the parameters even if there is something
        * wrong with the CommDb.
        * If the operation returns 0 as the value of aAccessPoint, something
        * has failed.
        */  
        IMPORT_C static void GetParametersL( 
            TUint32 aAccessPointLink, 
            TUint32& aAccessPoint,
            HBufC*& aServerUri,
            TBool& aProxyUsed,
            HBufC8*& aProxyAddress );

    public: // Functions from base classes        
        
    protected:  // New functions

    protected:  // Functions from base classes
        
        /**
        * From CActive
        * Handles the event generated when the request completes
        * @return void
        */
        void RunL();

        /**
        * From CActive
        * Cancels any outstanding request
        * @return void
        */
        void DoCancel();

        /**
        * From From CActive
        * This function is called if RunL leaves
        */  
        TInt RunError( TInt aError );

    private:

        /**
        * Constructor
        */
        void ConstructL();    

        /**
        * Checks if network is available.
        * @return TRUE, if network available, FALSE, if no network
        */
        TBool CheckNetworkL();

        
        /**
        *
        * @param TUint32 aWapAP (in)
        * @param TUint32& aIap (out)
        * @return TBool which informs if the IAP finding was successful
        */
        TBool FindIapL( TUint32 aWapAP, TUint32& aIap );

        /**
        * Checks that the WAP Start Page in CommsDB contains the scheme
        * (HTTP://). If not, it will be added.
        * @param HBufC* aUri is the WAP Start Page
        * @return void
        */
        static void CheckWapStartPageL( HBufC*& aUri );

        /**
        * Checks if there is already connections open and checks if the 
        * connection could be reused or not.
        * @param aIap, IAP to connect or not to connect 
        * @return void
        */
        void StartConnectionL( TUint32 aIap );
       
        /**
        * Searches for PDP Type and GPRS Access Point Name for an IAP given as 
        * parameter. Memory for aGprsApn is allocated inside the function and
        * deallocation should be made by caller. The pointer is not in cleanup
        * stack.
        * @return TBool. ETrue if the connection is GPRS, otherwise EFalse
        */
        TBool FindGprsParametersL( TUint32 aIap, TUint32& aPdpType, HBufC*& aGprsApn );

        /**
        * C++ default constructor.
        */
        CMmsConnectionInitiator();

    public:     // Data
    protected:  // Data

    private:    // Data
        RConnection*     iConnection;
        TRequestStatus*  iClientStatus;     // status of the client
        TCommDbConnPref  iPrefs;
        CArrayFixFlat<TUint32>* iArray; 
        TInt iIndex; // Index of the table when looping it through
        TNifProgress iProgress;  // instance of TNifProgressBuf
        // Timer to do a quick retry if case of immediate timeout
        RTimer       iTimer;
        // Tells if timer is running and must be cancelled
        TBool        iTimerRunning;
        // Time the connection is started
        TTime        iStartTime;
        TInt         iRetryCount;
    };

#endif // __MMSCONNINIT_H   
            
// End of File
