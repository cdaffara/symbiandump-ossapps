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


#ifndef CBTOBJECTSERVER_H
#define CBTOBJECTSERVER_H

#include <coecntrl.h>
#include <obexserver.h>
#include <bttypes.h>
#include <obexobjects.h>

#include "cxhtmlfilecomposer.h"
#include "btprotocolconsts.h"
#include "cbtdprserviceadvertiser.h"


/**
* MBtObjectServerObserver
*
* @brief Interface to observe bluetooth object server's state
*/
class MBtObjectServerObserver
{
    public:
	    enum TStatusMessages
	    {
	    	KObjectServerPackageSent = 10,	///@brief a requested chunk of data sent
	    	KObjectServerFileSent,			///@brief a requested file sent
	    	KObjectServerAllFilesSent, 		///@brief not used... cannot know when done!
	    	KObjectServerStopped,			///@brief server stopped
	    	KObjectServerStarted,			///@brief server started
	    	KObjectServerBTDeviceMismatch,	///@brief remote device isn't correct
			KObjectServerConnected,			///@brief remote device connected
			KObjectServerDisconnected,		///@brief remote device disconnected
			KObjectServerTransportUp,		///@brief transportation up with remote device
			KObjectServerTransportDown,		///@brief transportation down with remote device
			KObjectServerError				///@brief Error indication for any occured error
	    };

    public:

	   /**
        * OperationStatus
        *
        * A callback function to observe bluetooth object server's state
        * @param aStatus    TStatusMessages status or error code occured
        * @param aInfo  	Explaining information of the status
        */
    	virtual void HandleObjectServerEvent(TInt aStatus, TInt aInfo = 0) = 0;
};
/*!
  @class CBtObjectServer

  @discussion An instance of the Obex File Server object for the obexfileserver
  example application
  */
NONSHARABLE_CLASS(CBtObjectServer) : public CBase, public MObexServerNotify
{
    public:

	   /**
        * NewL()
        * Construct a CBtObjectServer
        * @param aDevice    Device (printer) which should be contacted over Bluetooth
        * @param aImgArray  Images that will be sent over OBEX (if printer asks them)
        * @return a pointer to the created instance of CBtObjectServer
        */
		static CBtObjectServer* NewL(const TBTDevAddr& aDeviceAddress, RArray<CImageInfo> &aImgArray, MBtObjectServerObserver& aObs);

	   /**
        * NewLC()
        * Construct a CBtObjectServer
        * @param aDevice    Device which should be contacted over Bluetooth
        * @param aImgArray  Images that will be sent over OBEX (if printer asks them)
        * @return a pointer to the created instance of CBtObjectServer
        */
		static CBtObjectServer* NewLC(const TBTDevAddr& aDeviceAddress, RArray<CImageInfo> &aImgArray, MBtObjectServerObserver& aObs);

       /**
        * ~CBtObjectServer()
        * Destructor for the class
        */
        ~CBtObjectServer();

	   /**
		* CreateObexObject()
		* For creating an OBEX object that will be send through bluetooth
		*/
		CObexBufObject* CreateObexObjectL(const TDesC& aUri);

	   /**
        * StartL()
        * Starts the server
        */
        void StartL();

       /**
        * Stop()
        * Stops the server
        */
        void Stop();

	   /**
		* Cancel()
		* For stopping server action nicely - will continue to send the current file to
		* the BT device
		*/
        void Cancel();


       /**
        * IsConnected()
        * @return ETrue if the server is connected.
        */
        TBool IsConnected();

       /**
        * IsTransporting()
        * @return ETrue if the transport connection is up.
        */
        TBool IsTransporting();

       /**
        * IsSendingData()
        * @return ETrue if the server is sending data to a Bluetooth device
        */
        TBool IsSendingData();

       /**
        * IsFinished()
        * @return ETrue if the server is finished and can be deleted
        */
        TBool IsFinished();

	protected:

        /**
	    * ConstructL()
	    * Method that constucts the classes possibly leaving parts
	    */
        void ConstructL( RArray<CImageInfo> &aImgArray );


	private:

	   /**
		* CBtObjectServer()
		* @param aDevice    Device (printer) which should be contacted over Bluetooth
        * @param aImgArray  Images that will be sent over OBEX (if printer asks them)
		* Private constructor
		*/
		CBtObjectServer( const TBTDevAddr& aDeviceAddress, MBtObjectServerObserver& aObs );

       /**
        * InitialiseServerL
        * Initialises the server
        */
        void InitialiseServerL();

       /**
        * InitTransferData
        * Initialises the data used to transfer and process a file
        */
        void InitTransferData();

       /**
        * InitHeaderVariables
        * Initialises the requested header data members
        */
		void InitHeaderVariables();
		
        TBool AllowConnection();

       /**
        * HandleGetRequestL
        * Method to go through to trap leaves for GetRequestIndication
        */
        CObexBufObject* HandleGetRequestL( CObexBaseObject* aRequestedObject );

		void GetAppHeaderL(CObexBaseObject* aRequestedObject);
		void GetDataSequenceL(const TFileName& aFileName, TInt& aFileSize);
		void SetCompleted();

       /**
		* SetSecurityWithChannelL
		* Sets the security on the channel port
		* And returns the channel available.
		* @param aAuthentication is authentication used
		* @param aEncryption is encryption used
		* @param aAuthorisation is authorisation used
		* @param aDenied is denied used
		* @return an available channel
		*/
		TInt SetSecurityWithChannelL ( TBool aAuthentication,
									   TBool aEncryption,
		                               TBool aAuthorisation,
                                       TBool aDenied );
	private:

        /**
         * Following private methods implement the MObexServerNotify class
         * - these methods (together with CObexServer) implement the observer
         *   for the OBEX data handling
         *  - Methods are called every time server receives something over the bluetooth
         */

       /**
        * ErrorIndication
        * Receive error indication
        * @param aError the error code
        */
        void ErrorIndication( TInt aError );

       /**
        * TransportUpIndication
        * Called when the underlying socket transport connection is made from
        * a remote client to the server
        */
        void TransportUpIndication();

       /**
        * TransportDownIndication
        * Transport connection is dropped
        */
        void TransportDownIndication();

       /**
        * ObexConnectIndication
        * Invoked when an OBEX connection is made from a remote client
        * @param aRemoteInfo connection information supplied by
        * the remote machine
        * @param aInfo holds further information about the
        * requested connection
        * @return system wide error code
        */
        TInt ObexConnectIndication( const TObexConnectInfo& aRemoteInfo,
                                    const TDesC8& aInfo );

       /**
        * ObexDisconnectIndication
        * OBEX server has been disconnected
        * @param aInfo contains information about the disconnection
        */
        void ObexDisconnectIndication( const TDesC8& aInfo );

       /**
        * PutRequestIndication
        * @return the CObexBufObject in which to store
        * the transferred object
        */
        CObexBufObject* PutRequestIndication();

       /**
        * PutPacketIndication
        * @return system wide error code
        */
        TInt PutPacketIndication();

       /**
        * PutCompleteIndication
        * @return system wide error code
        */
        TInt PutCompleteIndication();

       /**
        * GetRequestIndication
        * Called when a full get request has been received from the client
        * @param aRequestedObject holds details about the requested object
        * @return the CObexBufObject in which return to the client
        */
        CObexBufObject* GetRequestIndication( CObexBaseObject*
                                              aRequestedObject );

       /**
        * GetPacketIndication
        * @return system wide error code
        */
        TInt GetPacketIndication();

       /**
        * GetCompleteIndication
        * @return system wide error code
        */
        TInt GetCompleteIndication();

       /**
        * SetPathIndication
        * @param aPathInfo the information received in a SETPATH command
        * @return system wide error code
        */
        TInt SetPathIndication( const CObex::TSetPathInfo& aPathInfo,
                                const TDesC8& aInfo );

       /**
        * AbortIndication
        * Server has been aborted
        */
        void AbortIndication();

        /**
         * End of the Obex Observer methods
         *
         */

	private:
		
	    /**
	     * State of the server
	     * Owned by CBtObjectServer
         */
		enum TServerState
		{
			EStateDisconnected = 0,
			EStateConnected,
			EStateStop
		};
	
	    /**
	     * Current operation of the server
	     * Owned by CBtObjectServer
         */
		enum TServerOperation
		{
			ETerminating = 0,
			ESending,
			EIdle
		};
	
	    /**
	     * The bluetooth device where the images will be sent
	     * Owned by CBtObjectServer
         */
		const TBTDevAddr& iAllowedAddress;

	    /**
	     * The bluetooth device requested the data
	     * Owned by CBtObjectServer
         */
		TBTDevAddr iRemoteAddress;

	    /**
	     * The images that will be printed if printer asks for them
	     * Owned by CBtObjectServer
         */
	    RArray<CImageInfo> iImgArray;

	    MBtObjectServerObserver& iObs;

		/**
         * iObexServer manages the OBEX client connection
         * Owned by CBtObjectServer
         */
        CObexServer* iObexServer;

        /**
        * iObexBufData the raw data that has been transferred
        * Owned by CBtObjectServer
        */
        CBufFlat* iObexBody;

       /**
        * iCurrObject the OBEX object that has been transferred
        * Owned by CBtObjectServer
        */
        CObexBufObject* iObexBufObject;

       /**
        * iAdvertiser used to advertise this service
        * Owned by CBtObjectServer
        */
        CBtDprServiceAdvertiser* iAdvertiser;

        /**
         * Current operation
         */
        TServerOperation iOperation;

        RFs iFs;

		TBool iStarted;
		TBool iConnected;
		TBool iTransportUp;

		TUint iOffset;
		TInt iCount;
		TInt iSize;
		TInt iProgress; // proggress in percents of currently printed file

};

#endif // CBTOBJECTSERVER_H

//  End of File
