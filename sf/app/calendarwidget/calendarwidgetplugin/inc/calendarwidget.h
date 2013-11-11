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
* Description: Home screen calendar widget
*
*/

#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

// System includes
#include <hbwidget.h>

// Forward declarations
class HbLabel;
class QGraphicsLinearLayout;
class HbFrameDrawer;
class HbFrameItem;
class ContentLayoutHandler;
class DateIconLayoutHandler;
class MainLayoutHandler;
/*!
    \class HbWidget
    \brief The super class for homescreen widgets.
*/
class HbWidget;
class XQServiceRequest;
class HbDocumentLoader;
class QTranslator;

/*!
    \class CalendarWidget
    \brief The CalendarWidget class is the main class for the calendar widget project
*/
// Class declaration
class CalendarWidget : public HbWidget
{
    Q_OBJECT

public:
    /*! 
        \fn CalendarWidget::CalendarWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
        
        Constructor of the CalendarWidget class.
     */
    CalendarWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    /*!
        \fn CalendarWidget::~CalendarWidget()

        Destructor of the CalendarWidget class.
    */
    ~CalendarWidget();
    /*!
        \fn QRectF CalendarWidget::boundingRect() const

        Derived from HbWidget. Returns the bounding rectangle of the widget.
    */
    QRectF boundingRect() const;
    /*!
        \fn QPainterPath CalendarWidget::shape() const

        Derived from HbWidget. Returns the shape of the widget.
    */
    QPainterPath shape() const;

signals:
    /*!
        \fn void CalendarWidget::mousePressed(QPointF& point)

        Emitted by the widget when mouse press event is received
        @param point The position in which the event took place.
    */
    void mousePressed(QPointF& point);
    /*!
        \fn void CalendarWidget::mouseReleased()

        Emitted by the widget when mouse release event is received.
    */
    void mouseReleased();
    /*!
        \fn void CalendarWidget::themeChanged()

        Emitted by the framework when a theme change event is received by the widget.
    */
    void themeChanged();
    /*!
        \fn void CalendarWidget::finished()

        Emitted by the widget if the initialization fails.
    */
    void finished();
        /*!
        \fn void CalendarWidget::tapGesture(QPointF& point)

        Emitted by the widget when the widget receives a tap gesture.
        @param point Position of the tap gesture
    */
    void tapGesture(QPointF& point);

protected:
    /*!
        \fn bool CalendarWidget::event(QEvent *  event)

        Derived from HbWidget. Catches all the events that are directed to the widget.
        @param event The received event.
    */
    bool event(QEvent *  event);
    /*!
        \fn bool CalendarWidget::eventFilter(QObject * obj, QEvent * event)

        Reimplemented from QObject. Filters events when CalendarWidget has 
        been installed as an event filter for a watched object. Used for detecting 
        mouse press events before they arrive to HomeScreen application.
        @param obj The watched object.
        @param event The caught event.
    */
    bool eventFilter(QObject *obj, QEvent *event);
    /*!
        \fn void CalendarWidget::gestureEvent(QGestureEvent *event)

        Reimplemented from HbWidgetBase. Called by the framework when 
        a gesture event has been received.
        @param event The received event
    */
    void gestureEvent(QGestureEvent *event);

public slots:
    /*!
        \fn void CalendarWidget::onShow()

        Called by the framework each time the widget is shown on the screen.
    */
    void onShow();
    /*!
        \fn void CalendarWidget::onHide()

        Called by the framework each time the widget is hidden from the screen.
    */
    void onHide();
    /*!
        \fn void CalendarWidget::onInitialize()

        Called by the framework each time the widget is initialized.
    */
    void onInitialize();
    /*!
        \fn void CalendarWidget::onUninitialize()

        Called by the framework each time the widget is deleted.
    */
    void onUninitialize();

private:
    /*!
        \fn bool CalendarWidget::loadWidget()

        Used by CalendarWidget::onInitialize to load the DOCML and create the layout handlers. 
        Upon success it returns true. If initialization fails it returns false. 
    */
    bool loadWidget();
    /*!
        \fn void CalendarWidget::mousePressEvent(QGraphicsSceneMouseEvent *aEvent)

        Called by the framework when the mouse is pressed on the top of the widget.
        Does not currently do anything. Mouse press events are caught using an event filter.
        @param aEvent The received mouse event
    */
    void mousePressEvent(QGraphicsSceneMouseEvent *aEvent);
    /*!
        \fn void CalendarWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *aEvent)

        Called by the framework when the mouse is released on the top of the widget.
        @param aEvent The received mouse event
    */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *aEvent);
    /*!
        \fn void CalendarWidget::loadTranslator()

        Loads and installs the localization.
    */
    void loadTranslator();
    /*!
        \fn void CalendarWidget::connectLayoutHandler(HbWidget *widget, QObject *layoutHandler, HbDocumentLoader& loader)()

        Used by CalendarWidget::loadWidget() to initialize the layouts and to connect the widget's signals with 
        layout handler's slots.
        @param widget Main widget
        @param layoutHandler Layout handler to connect
        @param loader Document loader needed for getting the relevant objects
    */
    void connectLayoutHandler(HbWidget *widget, QObject *layoutHandler, HbDocumentLoader& loader);
    /*!
        \fn bool CalendarWidget::parseFile(QString& fileName, HbDocumentLoader& loader)

        Parses an XML file that describes the functionality of the widget.
        @param fileName Name of the XML file
        @param loader Document loader that contains the relevant objects
    */
    bool parseFile(QString& fileName, HbDocumentLoader& loader);

private: // data
    Q_DISABLE_COPY(CalendarWidget)
    QTranslator* mCommonTranslator;
    bool mWidgetLoaded;
    bool mIsTapAndHold;
    
    //For documentation purpose
    ContentLayoutHandler* mContent;
    //to get the widget shape
    HbLabel* mWidgetBackground;
    HbLabel* mIconLabel;
};

#endif // CALENDARWIDGET_H
