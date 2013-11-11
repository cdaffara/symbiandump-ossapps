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
* Description:  File Transfer Profile Plug-in header file
*
*/


#ifndef _SCONFTPPLUGIN_H
#define _SCONFTPPLUGIN_H

#include <obexserver.h>
#include <SrcsInterface.h>

#include "sconshutdownwatcher.h"

// forward declaration
class CSConFTP;
class CSConServiceTimer;
// CLASS DECLARATION

/*
* File Transfer Profile Plugin class
*
*/

class CSConFTPplugin : public CSrcsInterface, public MObexServerNotify,
                          public MShutdownObserver
    {
    public:
        static CSConFTPplugin* NewL();
        ~CSConFTPplugin();

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

        void ConstructL();
    CSConFTPplugin();

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
