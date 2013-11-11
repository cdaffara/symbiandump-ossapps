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
* Description: Interface to HbDocumentLoader (DocML files).
*
*/

#ifndef BUBBLEWIDGETMANAGER_H
#define BUBBLEWIDGETMANAGER_H

#include <QObject>
#include <QMap>

class QGraphicsWidget;
class BubbleDocumentLoader;
class BubbleHandler;
class BubbleImageManager;

class BubbleWidgetManager : public QObject
{
    Q_OBJECT

public:
    enum View
    {
        SingleCallView,
        TwoCallsView,
        ThreeCallsView,
        ConferenceView,
        MutedOverlay
    };

    enum Container
    {
        None,
        ExpandedBubble,
        CollapsedBubble, // top most when 2 calls
        CollapsedBubble2 // top most when 3 calls
    };

    enum Widget
    {
        Heading,
        Image,
        LeftButton,
        CenterButton,
        RightButton,
        ParticipantList,
        ParticipantListItem,
        ParticipantListButton,
        MutedIcon
    };

public:
    BubbleWidgetManager(
        BubbleImageManager& imageManager,
        QObject* parent=0);
    virtual ~BubbleWidgetManager();

    QGraphicsWidget* view(View view);

    bool isLoaded(View view);

    void releaseView(View view);

    QGraphicsWidget* container(View view,
                               Container container) const;

    QGraphicsWidget* widget(View view,
                            Container container,
                            Widget widget) const;

    QList<BubbleHandler*>* handlers(QGraphicsWidget* widget) const;

    QGraphicsWidget* createParticipantListItem();

public slots:
    void handleOrientationChange(Qt::Orientation orientation);

private:
    QGraphicsWidget* loadDocument(View view, Qt::Orientation orientation);
    void createDocumentHandlers(View view);
    void applyCustomStyles(View view);
    void setButtonStyle(Widget widget, QGraphicsWidget* button);
    void setBackground(QGraphicsWidget* widget);

private:
    BubbleImageManager& mImageManager;
    Qt::Orientation mOrientation;
    QMap<int,BubbleDocumentLoader*>  mDocumentLoaders;
    QMap<int,QObjectList>            mDocumentObjects;
    QMap<int,QList<BubbleHandler*>*> mDocumentHandlers;
    QMap<int,QGraphicsWidget*>       mDocumentContent;

    QMap<int,QString> mFileNames;
    QMap<int,QString> mContainerName;
    QMap<int,QString> mWidgetPrefix;
    QMap<int,QString> mWidgetName;
    QMap<int,QString> mOrientationName;
};

#endif // BUBBLEWIDGETMANAGER_H
