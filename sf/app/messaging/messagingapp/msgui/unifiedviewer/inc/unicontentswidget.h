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
 * Description:  This widget is place holder for message headers and body
 *
 */

#ifndef UNI_CONTENTS_WIDGET_H
#define UNI_CONTENTS_WIDGET_H

#include <hbwidget.h>
#include <qobject.h>
#include "convergedmessage.h"

class QGraphicsLinearLayout;
class QGraphicsGridLayout;
class UniViewerFeeder;
class ConvergedMessage;
class UniViewSlideWidget;

/**
 * This widget is place holder for message headers and body
 */
class UniContentsWidget : public HbWidget
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    UniContentsWidget(UniViewerFeeder* feeder, QGraphicsItem * parent = 0);

    /**
     * Destructor
     */
    ~UniContentsWidget();

    /**
     * Clears the content on the widget
     */
    void clearContent();

    /**
     * Populates the content on the widget
     */
    void populateContent();

    /**
     * Remove item from layout
     */
    void removeItemFromLayout(UniViewSlideWidget* item);

public slots:

    /*
     * Creates and populates the next slide
     */
    void populateNextSlide();

private:

    /*
     * Adds item to layout.
     * @param item item to be added.
     */
    void addItemToLayout(UniViewSlideWidget* item);
    
    
signals:
        /**
         * this signal is emitted when sendMessage is emitted. from UniViewerTextItem.
         */
    void sendMessage(const QString& phoneNumber,const QString& alias = QString());

private:

    /**
     * QGraphicsLinearLayout object
     * Owned
     */
    QGraphicsLinearLayout* mMainLayout;

    /**
     * UniViewerFeeder object
     * Owned
     */
    UniViewerFeeder* mViewFeeder;

    /**
     * Total number of slides
     */
    int mTotalSlides;

    /**
     * Current slide number
     */
    int mCurrentSlide;

    /**
     * Slide array.
     */
    QList<UniViewSlideWidget*> mSlides;

    /**
     * Total slides that can be created
     */
    int mSlideWindow;

    /**
     * Total slides to be created in the construction
     */
    int mInitialLoadCount;

    /**
     * Total slides of the current message already loaded
     */
    int mTotalSlidesLoaded;

    /**
     * Maintains the total slides added to the layout
     */
    int mRowCount;

    /**
     * Message type of the message being viewed
     */
    ConvergedMessage::MessageType mMessageType;
};

#endif // UNI_CONTENTS_WIDGET_H
// EOF
