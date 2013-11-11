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
* Description:  Example of home screen widget
*
*/

#include <QDebug>
#include <QGraphicsLinearLayout>
#include <QGraphicsWebView>
#include <HbPushButton>
#include <HbLineEdit>
#include <HbFrameDrawer>
#include <HbFrameItem>
#include <HbDeviceProfile>
#include "minibrowserwidget.h"

/*!
    \ingroup group_minibrowser_widget
    \class MiniBrowserWidget
    \brief Example implementation for home screen widget.

    MiniBrowserWidget derived from the HbWidget and implements 
    needed functions for the home screen widget. 
    As name suggests MiniBrowserWidget is a small web browser widget in
    the home screen. 
    
    Mini browser widget demonstrates the following home screen widget features:
    <ol>
    <li> home screen online/offline state, see \ref sssection_isonline </li>
    <li> preferences, see \ref sssection_setpreferences </li>
    </ol>
    It also shows use of HbPushButton and HbLineEdit widgets. 
    @image html hs_minibrowser.png
    
    
*/

/*!
    Constructs a widget which is a child of \a parent, with widget flags set to \a flags.
    Widget creates two buttons ('go' and 'stop'), text field to write url and browser view.
    Note that this is just an simple example so this creates child widgets directly to the layout.
    For more complicated widget layouting, see Orbit documentation and especially WidgetML section of it.
*/
MiniBrowserWidget::MiniBrowserWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags)
{
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    setLayout(layout);
    
    QGraphicsLinearLayout *buttonLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    mGoButton = new HbPushButton("Go");
    connect(mGoButton, SIGNAL(pressed()), SLOT(pressGoButton()));
    buttonLayout->addItem(mGoButton);
    mStopButton = new HbPushButton("Stop");
    connect(mStopButton, SIGNAL(pressed()), SLOT(pressStopButton()));
    buttonLayout->addItem(mStopButton);
    mStopButton->setEnabled(false);
    mEditor = new HbLineEdit();
    buttonLayout->addItem(mEditor);
    
    layout->addItem(buttonLayout);

    mWeb = new QGraphicsWebView();
    layout->addItem(mWeb);
    
    connect(mWeb, SIGNAL(loadStarted()), SLOT(loadStarted()));
    connect(mWeb, SIGNAL(loadProgress(int)), SLOT(loadProgress(int)));
    connect(mWeb, SIGNAL(loadFinished(bool)), SLOT(loadFinished(bool)));

    // maximum size of the home screen widget is (48,39) units. Convert units to pixels and 
    // resize minibrowser as maximum size
    HbFrameDrawer *drawer = new HbFrameDrawer(
        QLatin1String("qtg_fr_hsshortcut_normal"), HbFrameDrawer::NinePieces);
    setBackgroundItem(new HbFrameItem(drawer));
    
    HbDeviceProfile profile;
    qreal factor = profile.unitValue();
    setPreferredSize(48*factor, 39*factor);
    resize(preferredSize());
    
    mUrl = QString("http://www.nokia.com");
}

/*!
    Destructor
*/
MiniBrowserWidget::~MiniBrowserWidget()
{
}

/*!
    Returns widget's online state
*/
// Start of snippet 1
bool MiniBrowserWidget::isOnline()
{ 
    return mOnline;
}

/*!
    Sets online state of the widget as \a online
*/
void MiniBrowserWidget::setOnline(bool online)
{ 
    mOnline = online;
}
// End of snippet 1

/*!
    Returns url of the widget currently showing.
*/
// Start of snippet 2
QString MiniBrowserWidget::url() const
{
    return mUrl;
}

/*!
    Sets url of the widget as \a url
*/
void MiniBrowserWidget::setUrl(const QString& url)
{
    mUrl = url;
}
// End of snippet 2

/*!
    Called when widget is initialized and all to properties are set
*/
void MiniBrowserWidget::onInitialize()
{
    mGoButton->setEnabled(mOnline);
    mEditor->setText(mUrl);
}

/*!
    Called when widget is shown in the home screen
*/
// Start of snippet 3
void MiniBrowserWidget::onShow()
{
    if (mOnline) {
        mWeb->load(QUrl(mUrl));
        mStopButton->setEnabled(true);
        }
}
// End of snippet 3

/*!
    Called when widget is hidden from the home screen.
    Widget can decide what is appropriate action when 
    widget is not visible on the screen. At least it
    should not update itself as often as when on the 
    screen to save battery etc.
*/
// Start of snippet 4
void MiniBrowserWidget::onHide()
{
    mWeb->stop();
}
// End of snippet 4

/*!
    Slot connected to 'go' button
*/
// Start of snippet 5
void MiniBrowserWidget::pressGoButton()
{
    setUrl(mEditor->text());
    if (mOnline) {
        mWeb->load(QUrl(mUrl));
        mStopButton->setEnabled(true);
        }
    QStringList list;
    list.append("url");
    emit setPreferences(list);
}
// End of snippet 5

/*!
    Slot connected to 'stop' button
*/
void MiniBrowserWidget::pressStopButton()
{
    mWeb->stop();
    //mStopButton->setEnabled(false);
}

/*!
    Slot connected to webview's loadProgress signal
    TODO: check if this is needed.
*/
void MiniBrowserWidget::loadProgress(int progress)
{
    Q_UNUSED(progress)
    qDebug() << progress;
}

/*!
    Slot connected to webview's loadFinished signal
*/
void MiniBrowserWidget::loadFinished(bool ok)
{
    Q_UNUSED(ok)
    mStopButton->setEnabled(false);
}
void MiniBrowserWidget::loadStarted()
{
}

