/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Logging utility application for MC Photos
*
*/



#ifndef __GLXLOGGING_APPUI_H__
#define __GLXLOGGING_APPUI_H__

#include <aknappui.h>
#include <glxlogchunk.h>

/** 
  @class CGlxLoggingAppUi
  
  @discussion An instance of class CGlxLoggingAppUi is the UserInterface part of the AVKON
  application framework for the GlxLogging example application
  */
NONSHARABLE_CLASS( CGlxLoggingAppUi ) : public CAknAppUi
    {
    public:

        /**
         * @function CGlxLoggingAppUi
         * 
         * @discussion Perform the first phase of two phase construction.
         * This needs to be public due to the way the framework constructs the AppUi 
         */
        CGlxLoggingAppUi();

        /**
         * @function ~CGlxLoggingAppUi
         * 
         * @discussion Destroy the object and release all memory objects
         */
        ~CGlxLoggingAppUi();

        /**
         * @function ConstructL
         * 
         * @discussion Perform the second phase construction of a CGlxLoggingAppUi object
         * this needs to be public due to the way the framework constructs the AppUi 
         */
        void ConstructL();

    public: // from CAknAppUi

        /**
           * @function HandleCommandL
           *
           * @discussion Handle user menu selections
           * @param aCommand the enumerated code for the option selected
           */
        void HandleCommandL(TInt aCommand);

    public: // log writing

        static TInt TimerCallback( TAny* aParam );
        TInt DoLog();

    private: // Data

        /// Own: log manager
        RGlxLogManager iLogManager;
        /// Own: timer for log write
        CPeriodic* iTimer;

    };


#endif // __GLXLOGGING_APPUI_H__

