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
 * Description: This widget holds the message details and silde contents.
 *
 */

#ifndef UNIVIEWSLIDE_WIDGET_H
#define UNIVIEWSLIDE_WIDGET_H

#include <hbwidget.h>
#include <qobject.h>

class QGraphicsLinearLayout;
class UniViewerBodyWidget;
class UniViewerFeeder;
class UniViewerHeaderContainer;

/**
 * This widget is place holder for message headers and body
 */
class UniViewSlideWidget: public HbWidget
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    UniViewSlideWidget(UniViewerFeeder* feeder, int slideNumber, QGraphicsItem * parent = 0);

    /**
     * Destructor
     */
    ~UniViewSlideWidget();

    /**
     * Returns the value of mInsideLayout
     * @return value of mInsideLayout
     */
    bool insideLayout();

    /**
     * Sets the mInsideLayout
     * @param inside which is set to mInsideLayout
     */
    void setInsideLayout(bool inside);

public slots:

    /**
     * Clears the content on the widget
     */
    void clearContent();

    /**
     * Populates the content on the widget
     */
    void populateContent();

    /**
     * Populates the content first slide
     * @param messageDetails message details
     */
    void populateFirstSlide();

signals:

    /**
     * this signal is emitted when sendMessage is emitted. from UniViewerTextItem.
     */
    void sendMessage(const QString& phoneNumber,const QString& alias = QString());

private:

    /**
     * This is used only inside the contents widget
     * to find out if the slide widget is inside the
     * layout of contents widget
     */
    bool mInsideLayout;

    /**
     * slide number
     */
    int mSlideNumber;

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
     * UniViewerBodyWidget object
     * Owned
     */
    UniViewerBodyWidget* mBody;

    /**
     * UniViewerHeaderContainer object
     * Owned
     */
    UniViewerHeaderContainer* mHeaderContainer;

};

#endif // UNIVIEWSLIDE_WIDGET_H
// EOF
