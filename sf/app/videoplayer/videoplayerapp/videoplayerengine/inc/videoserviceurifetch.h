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
#include <xqserviceprovider.h>
#include <QStringList>

// FORWARD DECLARATIONS
class VideoServices;
class QLatin1String;

class VideoServiceUriFetch : public XQServiceProvider
{

    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT

public:

    /**
     * contructor
     */
    VideoServiceUriFetch( VideoServices *parent, QLatin1String service );

    /**
     * destructor
     */
    ~VideoServiceUriFetch();

    /**
     * Returns service active status
     *
     * @return bool true if active, false if not active
     *
     */
    bool isActive();

    /**
     * Completes the service
     *
     * @param fileList list of file URIs
     *
     */
    void complete(QStringList filesList);

    /**
     * Returns the context title set by service requestee
     *
     * @return QString the title
     *
     */
    QString contextTitle() const;

public slots:  // for QTHighway to notify provider about request

    /*
     *  Client can use this method launch video URI fetching
     *
     */
    void fetch();

public slots:  // for provider to notify client

    void fetchFailed( int errorCode );

private:

    void doComplete( QStringList filesList);

private:

    /*
     * The request index 
     */
    int mRequestIndex;

    /*
     * Pointer to owning VideoServices  
     */
    VideoServices* mServiceApp;

	/*
	 * The title requested by the service user 
	 */
	QString mTitle;
};

#endif //__VIDEOSERVICESURIFETCH_H__
