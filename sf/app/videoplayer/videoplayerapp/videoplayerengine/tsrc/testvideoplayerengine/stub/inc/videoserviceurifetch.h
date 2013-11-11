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
* Description:   VideoServiceUriFetch class definition
*
*/

#ifndef __VIDEOSERVICESURIFETCH_H__
#define __VIDEOSERVICESURIFETCH_H__

//INCLUDES
#include <QStringList>

// FORWARD DECLARATIONS
class VideoServices;

class VideoServiceUriFetch : public QObject
{

    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT

public:

    /**
     * contructor
     */
    VideoServiceUriFetch( VideoServices *parent = 0 );

    /**
     * destructor
     */
    ~VideoServiceUriFetch();

public slots:  // for QTHighway to notify provider about request
    /*
     *  Client can use this method launch video URI fetching
     *
     * @param title title to be set
     *
     */
    void fetch();

private:
    VideoServices* mServiceApp;
};

#endif //__VIDEOSERVICESURIFETCH_H__
