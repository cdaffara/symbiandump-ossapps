/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Declaration for MMMCScBkupEngineObserver
*     
*
*/

#ifndef __MMMCSCBKUPENGINEOBSERVER_H__
#define __MMMCSCBKUPENGINEOBSERVER_H__

/**
*
*
* @since 3.0
*/
class MMMCScBkupEngineObserver
    {
    public:

        /**
        *
        */
        enum TEvent
            {
            // COMMON
            ECommonEventFirst = 0,
            ECommonOperationStarting = ECommonEventFirst,
            ECommonSizeOfTaskUnderstood,
            ECommonOperationPrepareEnded,
            ECommonOperationEnded,
            ECommonOperationError,
            ECommonProgress,
            ECommonEventLast,

            // BACKUP
            EBackupEventFirst = 100,
            EBackupAnalysingData,
            EBackupEventLast,

            // RESTORE
            ERestoreEventFirst = 200,
            ERestoreEventLast
            };

    public: // From MMMCScBkupEngineObserver

        /**
        *
        */
        virtual TInt HandleBkupEngineEventL(TEvent aEvent, TInt aAssociatedData) = 0;

    };


#endif // __MMMCSCBKUPENGINEOBSERVER_H__

//End of File
