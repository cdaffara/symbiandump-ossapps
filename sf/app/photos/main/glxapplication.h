/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   ?Description
*
*/

#ifndef GLXAPPLICATION_H_
#define GLXAPPLICATION_H_

#include <hbApplication.h>

class QSymbianEvent;

/**
 * Class Description
 * Custom Glx appliaction class to monitor the low memory situation 
 */

class GlxApplication: public HbApplication
{
    Q_OBJECT
public:

    /**
     * Constructor
     */
    GlxApplication( int &argc, char *argv[],
                        Hb::ApplicationFlags flags = Hb::DefaultApplicationFlags );
    
    /**
     * Constructor
     */
    GlxApplication( QApplication::QS60MainApplicationFactory factory,
                        int &argc, 
                        char *argv[],
                        Hb::ApplicationFlags flags = Hb::DefaultApplicationFlags );
    
    /**
     * symbianEventFilter()
     * /// From HbApplication
     */
    bool symbianEventFilter( const QSymbianEvent *aQSEvent );
    
    /**
     * initMattiAutomation() - To monitor the application ready event for matti tool.
     */
    void initMattiAutomation();
    
signals :
    /**
     * applicationReady() - To emit the application ready signal for matti tool.
     */
    void applicationReady();
    
public slots:
    /**
     * handleAppReady() - Call back function to get the notification of application ready.
     * It is also used for matti tool.
     */
    void handleAppReady();
    
private :
    /**
     * cleanUpCache() - To clear the cache in the low memory situation.
     */
    void cleanUpCache();

};
    
#endif // GLXAPPLICATION_H_
