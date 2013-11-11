/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of VideoPlaybackDocumentLoader
*
*/

// Version : %version:  2 %



#ifndef VIDEOPLAYBACKDOCUMENTHANDLER_H_
#define VIDEOPLAYBACKDOCUMENTHANDLER_H_


#include <qobject>

class QList;
class QGraphicsWidget;
class VideoPlaybackControlsController;

class VideoPlaybackDocumentLoader : public QObject
{
    public:
        VideoPlaybackDocumentLoader();

        virtual ~VideoPlaybackDocumentLoader();

    public:
        QGraphicsWidget* findWidget( const QString &name );
        void load( const QString &fileName, const QString &section , bool *ok );

    private:
        QGraphicsWidget* createWidget( const QString &name );
        int exist( const QString &name );

    private:
        QList<QGraphicsWidget*>                 mWidgets;
};

#endif /*VIDEOPLAYBACKDOCUMENTHANDLER_H_*/

