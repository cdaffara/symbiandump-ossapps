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
* Description:   VideoOperatorService class declaration.
*
*/


#ifndef VIDEOOPERATORSERVICE_H
#define VIDEOOPERATORSERVICE_H

#include <qobject.h>

class VideoOperatorServicePrivate;

/**
 * Public functionality for loading and launching operator customisable services.
 */
class VideoOperatorService : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(VideoOperatorService)

public:

    /**
     * Contructor.
     *
     * @param parent parent of this widget
     */
    VideoOperatorService(QObject *parent = 0);

    /**
     * Destructor.
     *
     */
    ~VideoOperatorService();    
    
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
    
public slots:
    
    /**
     * Launches the service.
     */
    void launchService();
    
private:
    
    /**
     * Pointer to private implementation.
     */
    VideoOperatorServicePrivate *d_ptr;

};

#endif // VIDEOOPERATORSERVICE_H

// End of file.
