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




#ifndef GLXTNOBSERVER_H
#define GLXTNOBSERVER_H

#include <QObject>
#include <glxtnmonitor.h>

class CGlxTNMonitor;

class GlxTNObserver : public QObject, public MGlxTNObserver
{
    Q_OBJECT
    
public:
    
    /*
     * constructor
     */
    GlxTNObserver();
    
    /*
     * Call back to get the number of TN is left to create.
     */
    void updateTNCount(int &count); 
    
    /*
     * It will return the number of TN is left to create.
     */
    int getTNLeftCount() ;
    
    /*
     * It will start the tumbnail generation montior process.
     */    
    void startTNObserving(); 
    
    /*
     * To start the thumbnail generation in back ground
     */
    void startTNMDaemon() ;

    /*
     * To stop the thumbnail generation in back ground
     */
    void stopTNMDaemon() ;
    
    /*
     * Destructor
     */
    ~GlxTNObserver();
    
signals : 
    /*
     * broad cast the number of thumbnail left to generate
     */
    void leftTNCount( int count);
    
private :
    CGlxTNMonitor* mTNMonitor;
};

#endif /* GLXTNOBSERVER_H */
