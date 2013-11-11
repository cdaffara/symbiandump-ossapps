/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VideoOperatorServicePrivate class declaration.
*
*/

#ifndef VIDEOOPERATORSERVICEPRIVATE_H
#define VIDEOOPERATORSERVICEPRIVATE_H

// INCLUDE FILES
#include <qobject.h>
#include <qprocess.h>
#include <e32cmn.h>

class VideoOperatorService;

/**
 * Private functionality for loading and launching operator customisable services.
 */
class VideoOperatorServicePrivate : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(VideoOperatorServicePrivate)
    
public:

    /**
     * Contructor.
     *
     * @param ptr pointer to public implementation
     */
    VideoOperatorServicePrivate();

    /**
     * Destructor.
     *
     */
    ~VideoOperatorServicePrivate();
    
    /**
     * Loads the service.
     * 
     * @titleKey Central Repository key id for the service's title.
     * @iconKey Central Repository key id for the service's icon resource.
     * @uriKey Central Repository key id for the service's URI.
     * @uidKey Central Repository key id for the service's UID.
     * 
     * @return True if service was loaded succesfully.
     */
    bool load(int titleKey, int iconKey, int uriKey, int uidKey);

    /**
     * Returns name for the service.
     * 
     * @return name of the service.
     */
    const QString title() const;
    
    /**
     * Returns the icon resource for the service.
     * 
     * @return path or resource id to the icon.
     */
    const QString iconResource() const;
    
    /**
     * Launches the service.
     */
    void launchService();

private:
    
    /**
     * Gets filename including path.
     * 
     * @param TUid application UID.
     * 
     * @return string application full filename.
     */
    QString getApplicationFilenameL(const TUid uid);

    /**
     * Bring application to foreground. 
     * 
     * @param TUid application UID.
     * 
     * @return bool true if application was brought to foreground, else false.
     */
    bool bringApplicationToForeground(const TUid uid);

    /**
     * Starts the application. 
     * 
     * @param TUid application UID.
     */
    void startApplicationL(const TUid uid);
    
public slots:
    
    /**
     * Signaled when process has finished. Brings videos application back to foreground.
     */    
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    
    /**
     * Signaled if there's error with the process. 
     */
    void processError(QProcess::ProcessError error);

private:
    
    /**
     * Title for the service.
     */
    QString mTitle;
    
    /**
     * Icon for the service.
     */
    QString mIconResource;
    
    /**
     * Service URL if service should launch an URL.
     */
    QString mServiceUri;
    
    /**
     * Application UID if service should launch an external application.  
     */
    int mApplicationUid;
    
    /**
     * Pointer to started process.
     */
    QProcess *mProcess;
};

#endif // VIDEOOPERATORSERVICEPRIVATE_H

// End of file.
