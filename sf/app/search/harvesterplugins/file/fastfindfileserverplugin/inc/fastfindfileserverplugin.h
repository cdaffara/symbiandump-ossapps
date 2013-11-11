/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitors file creations, modifications and deletions.
*
*/

#ifndef FASTFINDFILESERVERPLUGIN_H
#define FASTFINDFILESERVERPLUGIN_H

#include <f32plugin.h>
#include "fastfindfileserverpluginclient.h"

/**
* File server plug-in.
*/
class CFastFindFileServerPlugin : public CFsPlugin
	{
    public:

        /**
         * Standard NewL()
         * From CActive.
         */
        static CFastFindFileServerPlugin* NewL();

        /**
         * Virtual destructor.
         */
        virtual ~CFastFindFileServerPlugin();

        /**
         * Initialize plugin.
         */
        virtual void InitialiseL();

        /**
         * 
         */
        virtual TInt DoRequestL( TFsPluginRequest& aRequest );

        /**
         * 
         */
        virtual CFsPluginConn* NewPluginConnL();

        /**
         * 
         */
        TInt RegisterNotification( CFsPluginConnRequest& aRequest );

        /**
         * Add a new notification path.
         */
        TInt AddNotificationPath( CFsPluginConnRequest& aRequest );

        /**
         * Remove notification path.
         */
        TInt RemoveNotificationPath( CFsPluginConnRequest& aRequest );

        /**
         * Add a new path to ignore.
         */
        TInt AddIgnorePath( CFsPluginConnRequest& aRequest );

        /**
         * Remove a path from the ignore list.
         */
        TInt RemoveIgnorePath( CFsPluginConnRequest& aRequest );

        /**
         * 
         */
        void EnableL();

        /**
         * 
         */
        void DisableL();

        /**
         * Add CFastFindFileServerPluginConn connection to this file server plugin
         */
        void AddConnection();

        /**
         * Remove CFastFindFileServerPluginConn connection from this file server
         * plugin
         */
        void RemoveConnection();

    private:

        /**
         * Private constructor.
         */
        CFastFindFileServerPlugin();

        /**
         * Check if a path is listed.
         * @param aFileName  Path to check.
         * @return EFalse, if path is ignored.
         *         ETrue, if the path is on the notification path list or
         *         if there is no notification paths set.
         *         Else EFalse.
         */
        TBool CheckPath( TFileName& aFileName );

        /**
         * Check if path/file hass hidden or system attribute.
         * @param aFileName  Filename to check.
         * @return ETrue, if path or file has a hidden or system attribute set.
         */
        TBool CheckAttribs( TFileName& aFileName );

#ifdef _DEBUG_EVENTS
        void RegisterDebugEventsL();
        void UnregisterDebugEventsL();
        void PrintDebugEvents( TInt aFunction );
#endif
    	
    private:

        /**
         * 
         */
        CFsPluginConnRequest* iNotification;

        /**
         * An array of notification paths.
         */
        RPointerArray<TFileName> iPaths;

        /**
         * A queue of file server events.
         */
        RPointerArray<TFastFindFSPStatus> iQueue;

        /**
         * 
         */
        RPointerArray<TFileName> iIgnorePaths;

        /**
         * File system client session.
         */
        RFs iFsSession;

        /**
         * 
         */
        RPointerArray<TFileName> iCreatedFiles;
        TInt iFormatDriveNumber;

        TInt iConnectionCount;
    };

#endif // FASTFINDILESERVERPLUGIN_H
