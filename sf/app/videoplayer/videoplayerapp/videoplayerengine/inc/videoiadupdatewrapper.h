/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials re made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declaration of VideoIadUpdateWrapper
*
*/

// Version : %version: 

#ifndef __VIDEOIADUPDATEWRAPPER_H__
#define __VIDEOIADUPDATEWRAPPER_H__

#include <qobject.h>
#include <iaupdateobserver.h>

class CIAUpdate;
class CIAUpdateParameters;
class MMPXViewUtility;

class VideoIadUpdateWrapper : public QObject, public MIAUpdateObserver
{
    /**
     * Define to be able to use signals and slots.
     */    
    Q_OBJECT

    /**
     * Disable copy-constructor and assignment operator.
     */
    Q_DISABLE_COPY(VideoIadUpdateWrapper)     
    
public:

    /**
     * Constructor
     */    
    VideoIadUpdateWrapper();

    /**
     * Destructor
     */    
    virtual ~VideoIadUpdateWrapper();
    
    /**
     * Starts the update process.
     */
    void checkForUpdates();

private: // New methods

    /** 
     * From MIAUpdateObserver.
     * This callback function is called when the update checking operation has completed.
     *
     * @param errorCode The error code of the observed update operation.
     *                   KErrNone for successful completion, 
     *                   otherwise a system wide error code.
     * @param availableUpdates Number of the updates that were found available.
     *
     */
    void CheckUpdatesComplete(TInt errorCode, TInt availableUpdates);

    /** 
     * From MIAUpdateObserver.
     * This callback function is called when an update operation has completed.
     * Even if multiple functions re provided to start different update operations,
     * this callback function is always called after an update operation has completed.
     *
     * @param errorCode The error code of the completed update operation.
     *                   KErrNone for successful completion, 
     *                   otherwise a system wide error code.
     * @param result Details about the completed update operation.
     *                Ownership is transferred.
     *
     */
    void UpdateComplete(TInt errorCode, CIAUpdateResult* resultDetails);

    /** 
     * From MIAUpdateObserver.
     * This callback function is called when an update query operation has completed.
     *
     * @param errorCode The error code of the observed query operation.
     *                   KErrNone for successful completion, 
     *                   otherwise a system wide error code.
     * @param updateNow ETrue informs that an update operation should be started.
     *                   EFalse informs that there is no need to start an update
     *                   operation.
     *                   
     */
    void UpdateQueryComplete(TInt errorCode, TBool updateNow);
    
private:
    
    void doCheckForUpdatesL();
    
    void cleanup();
    
private:
    
    /**
     * IAD update API.
     */
    CIAUpdate* mUpdate;

    /**
     * IAD update parameters.
     */
    CIAUpdateParameters* mParameters;

};

#endif // __VIDEOIADUPDATEWRAPPER_H__

// End of file.
