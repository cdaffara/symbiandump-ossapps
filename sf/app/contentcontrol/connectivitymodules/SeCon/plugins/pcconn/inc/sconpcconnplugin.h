/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PC Connectivity plugin header file
*
*/

#ifndef _SCONPCCONNPLUGIN_H
#define _SCONPCCONNPLUGIN_H

#include <obexserver.h>
#include <SrcsInterface.h>
#include <apmstd.h>

#include "sconpcconnclient.h"
#include "sconshutdownwatcher.h"

// forward declaration
class CSConFTP;
class CSConServiceTimer;
class CCatalogsPCConnectivityPlugin;



class CSConPCConnplugin : public CSrcsInterface, public MObexServerNotify,
                          public MShutdownObserver
    {
    public:
        static CSConPCConnplugin* NewL();
        virtual ~CSConPCConnplugin();

        /**
         * Returns active status of OBEX session
         * @return ETrue if session is active, else EFalse
         */
        TBool IsOBEXActive();
        
        /**
         * Device is shutting down, abort connection.
         */
        void NotifyShutdown();

    private:
        /**
         * Loads sconftp.dll module
         * @return none
         */
        void LoadFTPDllL();
        /**
         * Closes initialized services
         * @return none
         */
        void Disconnect();
        /**
         * Removes spaces and nulls from the end of the string
         * @param aDes String to be formatted
         * @return none
         */
        void TrimRightSpaceAndNull( TDes8& aDes ) const;
        /**
         * Current used transfer media (IR,USB,BT)
         * @param aMediaType Enumeration indicating the media type
         * @return none
         */
        void SetMediaType( TSrcsMediaType aMediaType );
        /**
         * MObexServerNotify implementation
         * @param aError Error code
         * @return none
         */
        void ErrorIndication( TInt aError );
        /**
         * MObexServerNotify implementation
         * @return none
         */
        void TransportUpIndication();
        /**
         * MObexServerNotify implementation
         * @return none
         */
        void TransportDownIndication();
        /**
         * MObexServerNotify implementation
         * @param aRemoteInfo Structure containing information used during OBEX
         * connection
         * @param aInfo Further information about the requested connection
         * @return System wide error code that indicates the success of the connection
         */
        TInt ObexConnectIndication( const TObexConnectInfo& aRemoteInfo,
                                    const TDesC8& aInfo );
        /**
         * MObexServerNotify implementation
         * @param aInfo Contains information about the disconnection
         * @return none
         */
        void ObexDisconnectIndication( const TDesC8& aInfo );
        /**
         * MObexServerNotify implementation
         * @return CObexBaseObject-derived object, which the object being put
         * will be parsed into.
         */
        CObexBufObject* PutRequestIndication();
        /**
         * MObexServerNotify implementation
         * @return System wide error code that indicates the success of the
         * PUT packet operation.
         */
        TInt PutPacketIndication();
        /**
         * MObexServerNotify implementation
         * @return System wide error code that indicates the success of the
         * PUT packet operation.
         */
        TInt PutCompleteIndication();
        /**
         * MObexServerNotify implementation
         * @param aRequiredObject Details about the object the remote client
         * has requested
         * @returnObject to return to the client
         */
        CObexBufObject* GetRequestIndication( CObexBaseObject
                                                *aRequiredObject );
        /**
         * MObexServerNotify implementation
         * @return System wide error code that indicates the success of the
         * GET packet operation.
         */
        TInt GetPacketIndication();
        /**
         * MObexServerNotify implementation
         * @return System wide error code that indicates the success of the
         * GET packet operation.
         */
        TInt GetCompleteIndication();
        /**
         * MObexServerNotify implementation
         * @param aPathInfo SETPATH command parameters
         * @param aInfo Not currently used
         * @return System wide error code indicating the success of the
         * setpath command.
         */
        TInt SetPathIndication( const CObex::TSetPathInfo& aPathInfo,
                                const TDesC8& aInfo );
        /**
         * SetObexServer
         * @param aObexServer pass obex server pointer to sevice controller
         * @return System wide error code.
         */
        TInt SetObexServer( CObexServer* aObexServer );
        /**
         * MObexServerNotify implementation
         * @return none
         */
        void AbortIndication();
        /**
         * Checks if mime type belongs to Catalogs
         * @return TBool
         */
        TBool IsCatalogsMimeType( TDataType aMime );

        void ConstructL();
        CSConPCConnplugin();
        
        /**
         * Handles PCD object. Used for advanced file handling.
         * @param aDescriptionHeader Contains command code and destination filename
         * @param aNameHeader Source filename
         * @return System wide error code.
         */
        TInt HandlePCDObjectPut( const TDesC& aDescriptionHeader, const TDesC& aNameHeader );
        
        /**
         * Get Capbility object or ConML message from PCConnServer
         * @param aNameHeader Name information.
         * @param aTypeHeader Object type header. ConML or Capability object type
         * @return none
         */
        void HandleGetPCConnObjectL( const TDesC& aNameHeader, const TDesC8& aTypeHeader );
        
        /**
         * Get Catalogs object
         * @param aTypeHeader Object type header.
         * @return none
         */
        void HandleGetCatalogsObjectL( const TDesC8& aTypeHeader );
        
        /**
         * Get file object
         * @param aNameHeader requested filename.
         * @return none
         */
        void HandleGetFileObjectL( const TDesC& aNameHeader );
        
    private:
        RLibrary                    iFTPlib;
        CObexBufObject*             iObject;
        CBufFlat*                   iBuffer;
        CSConFTP*                   iFTPHandler;
        TInt                        iPutError;
        TBool                       iSessionActive;
        CSConServiceTimer*          iServiceTimer;
        TBool                       iStartTimer;
        TSrcsMediaType              iMediaType;
        CObexFileObject*            iFileObject;
        RSConPCConnSession          iPCConnSession;
        TBool                       iPCConnSessionConnected;
        CCatalogsPCConnectivityPlugin*  iCatalogs;
        TBool                           iCatalogsConnected;
        HBufC8*                         iCatalogsMimeType;
        RPointerArray<TDataType>        iMimeArray;
        TBool                           iCatalogsMimeTypesExist;
        TBool                       iPutPacketIndicationCalled;
        CShutdownWatcher*           iShutdownWatcher;
        TBool                       iShutdownInProgress;
        CObexServer*                iObexServer;
        RSocketServ                 iSocketServer;
        RBTPhysicalLinkAdapter      iLinkAdapter;
    private:
        // Friend class is used,because existing impelentation has been working
        // properly. Avoiding re-design.
        friend class CSConServiceTimer;
    };

#endif // SCONPCCONNPLUGIN_H