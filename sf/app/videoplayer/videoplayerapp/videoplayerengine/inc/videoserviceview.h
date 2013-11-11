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
* Description:  Declaration of VideoServiceView 
* 
*/ 
      
// Version : %version: 3 % 
      
#ifndef __VIDEOSERVICEVIEW_H__ 
#define __VIDEOSERVICEVIEW_H__ 
   
#include <xqserviceprovider.h> 
#include <QObject> 
#include <xqsharablefile.h> 
      
// FORWARD DECLARATIONS 
class VideoServices; 
class VideoPlayerEngine; 
class QFile; 
class QLatin1String;
    
class VideoServiceView : public XQServiceProvider 
{       
    Q_OBJECT 
      
    public: 
    
        VideoServiceView( VideoServices *parent, 
                          VideoPlayerEngine* engine, 
                          QLatin1String service ); 
        
        virtual ~VideoServiceView(); 
     
        void setEngine( VideoPlayerEngine* engine ); 
      
    public slots: 
        bool view( QString file ); 
        bool view (XQSharableFile file); 
            
    private: 
        VideoPlayerEngine* mEngine; 
        VideoServices* mServiceApp; 
            
}; 
      
#endif //__VIDEOSERVICEVIEW_H__ 

