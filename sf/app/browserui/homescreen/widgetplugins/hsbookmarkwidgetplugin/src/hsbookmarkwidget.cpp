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
* Description:  Homescreen bookmark widget
*
*/
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include <HbStyleLoader>
#include <HbFrameItem>
#include <HbFrameDrawer>
#include <HbIconItem>
#include <HbTextItem>
#include <HbTouchArea>
#include <HbInstantFeedback>

#include <QDir>
#include <qdesktopservices.h>
#include <qurl.h>
#include <qdebug.h>
#include "hsbookmarkwidget.h"

/*!
    \ingroup group_hsbookmark_widget
    \class HsBookmarkWidget
    \brief Implementation for homescreen bookmark widget.

    HsBookmarkWidget derived from the HbWidget and implements 
    needed functions for the homescreen bookmark widget. 
*/

/*!
    Constructs a widget which is a child of \a parent, with widget flags set to \a flags.
*/
HsBookmarkWidget::HsBookmarkWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : HbWidget(parent, flags),
    	mBackground(0), mIcon(0), mText(0), mTouchArea(0)
{
    HbStyleLoader::registerFilePath(":/hsbookmarkwidget.widgetml");
    HbStyleLoader::registerFilePath(":/hsbookmarkwidget.css");
    	
    createPrimitives();
}

/*!
    Destructor
*/
HsBookmarkWidget::~HsBookmarkWidget()
{
    HbStyleLoader::unregisterFilePath(":/hsbookmarkwidget.widgetml");
    HbStyleLoader::unregisterFilePath(":/hsbookmarkwidget.css");
}

/*!
    Set's widget root path as \a rootPath
*/
void HsBookmarkWidget::setRootPath(const QString &rootPath)
{
    mRootPath = rootPath;
}

/*!
    Root path for widget's resources
*/
QString HsBookmarkWidget::rootPath()const
{
    return mRootPath;
}

/*!
    Set the bookmarkTitle property. This property is needed by css selector.
*/
void HsBookmarkWidget::setBookmarkTitle(const QString &bookmarkTitle) 
{ 
    mBookmarkTitle = bookmarkTitle; 
}

/*!
    Return the bookmarkTitle property. This property is needed by css selector.
*/
QString HsBookmarkWidget::bookmarkTitle() const 
{ 
    return mBookmarkTitle; 
}

/*!
    Set the bookmarkUrl property. This property is needed by QDestopService 
    to launch into browser.
*/
void HsBookmarkWidget::setBookmarkUrl(const QString &bookmarkUrl) 
{ 
    mBookmarkUrl = bookmarkUrl; 
}
    
/*!
    Return the bookmarkUrl property. This property is needed by QDestopService 
    to launch into browser.
*/
QString HsBookmarkWidget::bookmarkUrl() const 
{ 
    return mBookmarkUrl; 
}

/*!
    Set the iconFileName property. This property is to load the favison icon.
*/
void HsBookmarkWidget::setFaviconFileName(const QString &faviconFileName) 
{ 
    mFaviconFileName = faviconFileName; 
}
    
/*!
    Return the iconFileName property. This property is to load the favison icon.
*/
QString HsBookmarkWidget::faviconFileName() const 
{ 
    return mFaviconFileName; 
}

/*!
    Set the iconFileName property. This property is to load the favison icon.
*/
void HsBookmarkWidget::setFaviconPath(const QString &faviconPath) 
{ 
    mFaviconPath = faviconPath; 
}
    
/*!
    Return the iconFileName property. This property is to load the favison icon.
*/
QString HsBookmarkWidget::faviconPath() const 
{ 
    return mFaviconPath; 
}

/*!
    Filters touch area events.
*/
bool HsBookmarkWidget::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

    switch (event->type()) {
        case QEvent::GraphicsSceneMousePress:
            handleMousePressEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
            return true;
        case QEvent::GraphicsSceneMouseMove:
            handleMouseMoveEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
            return true;
        case QEvent::GraphicsSceneMouseRelease:
            handleMouseReleaseEvent(static_cast<QGraphicsSceneMouseEvent *>(event));
            return true;
        case QEvent::UngrabMouse:
            setBackgroundToNormal();
            return true;
        default:
            break;
    }

    return false;
}

void HsBookmarkWidget::onInitialize()
{
    if (mFaviconFileName.isNull())
    {	  
        // get default icon, rootPath is set thus we can use it
        QString iconPath = QDir::toNativeSeparators(mRootPath + "/nokiabrowser-bm-icon_70x70_no_ico.png");
        HbIcon icon = HbIcon(iconPath);
        updateContent(mBookmarkTitle, icon);
    } 
    else
    {
        // get favicon first
        QString faviconPath = QDir::toNativeSeparators(mFaviconPath + mFaviconFileName);
	  	
        HbIcon favicon = HbIcon(faviconPath);
        favicon.setSize(QSize(HsBookmarkWidget::faviconSize, HsBookmarkWidget::faviconSize));
         	
        QString iconPath = QDir::toNativeSeparators(mRootPath + "/nokiabrowser-bm-icon_70x70_ico.png");
        HbIcon icon = HbIcon(iconPath);
        
        updateContent(mBookmarkTitle, 
                      createIconWithOverlay(icon, 
                                            QPoint(HsBookmarkWidget::faviconPoint, HsBookmarkWidget::faviconPoint), 
                                            favicon));
    }
}

/*!
    Called when widget is shown in the home screen
*/
void HsBookmarkWidget::onShow()
{
    setVisible(true);
}

/*!
    Called when widget is hidden from the home screen
*/

void HsBookmarkWidget::onHide()
{
}

/*!
    \internal
*/
void HsBookmarkWidget::handleMousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    setBackgroundToPressed();
}

/*!
    \internal
*/
void HsBookmarkWidget::handleMouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (contains(event->pos())) {
        setBackgroundToPressed();
    } else {
        setBackgroundToNormal();
    }
}

/*!
    \internal
*/
void HsBookmarkWidget::handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setBackgroundToNormal();

    if (!contains(event->pos())) {
        return;
    }

    //HbInstantFeedback::play(HSCONFIGURATION_GET(bookmarkWidgetTapFeedbackEffect));
    
    QUrl url = QUrl(mBookmarkUrl);
    qDebug() << "hsBookmarkWidget - opening url - " << url << ", " << stat;
    bool stat = QDesktopServices::openUrl(url);
    qDebug() << "hsBookmarkWidget - opened url - " << url << ", " << stat;
}

/*!
    \internal
*/
void HsBookmarkWidget::createPrimitives()
{
    // Background
    if (!mBackground) {
        HbFrameDrawer *drawer = new HbFrameDrawer(
            QLatin1String("qtg_fr_hsshortcut_normal"), HbFrameDrawer::NinePieces);
        mBackground = new HbFrameItem(drawer, this);
        HbStyle::setItemName(mBackground, QLatin1String("background"));
    }

    // Icon
    if (!mIcon) {
        mIcon = new HbIconItem(this);
        HbStyle::setItemName(mIcon, QLatin1String("icon"));
    }
    
    // Text
    if (!mText) {
        mText = new HbTextItem(this);
        HbStyle::setItemName(mText, QLatin1String("text"));
    }
               
    // Touch AreaOK
    if (!mTouchArea) {
        mTouchArea = new HbTouchArea(this);
        mTouchArea->installEventFilter(this);
        HbStyle::setItemName(mTouchArea, QLatin1String("toucharea"));
    }
}

/*!
    \internal
*/
void HsBookmarkWidget::setBackgroundToNormal()
{
    mBackground->frameDrawer().
        setFrameGraphicsName(QLatin1String("qtg_fr_hsshortcut_normal"));
}

/*!
    \internal
*/
void HsBookmarkWidget::setBackgroundToPressed()
{
    mBackground->frameDrawer().
        setFrameGraphicsName(QLatin1String("qtg_fr_hsitems_pressed"));
}

/*!
    \internal
*/
void HsBookmarkWidget::updateContent(const QString title, const HbIcon& icon)
{
    mIcon->setIcon(icon);

    if (title != "") {
        mText->setText(title.left(HsBookmarkWidget::maxDisplayLen));     
    }
}

QImage HsBookmarkWidget::createImageWithOverlay(const QImage& baseImage, const QPoint& point, const QImage& overlayImage)
{
    QImage imageWithOverlay = QImage(baseImage.size(), QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&imageWithOverlay);

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(imageWithOverlay.rect(), Qt::transparent);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, baseImage);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(point, overlayImage);

    painter.end();

    return imageWithOverlay;
}

HbIcon HsBookmarkWidget::createIconWithOverlay(HbIcon& baseIcon, const QPoint& point, HbIcon& overlayIcon)
{

    // Create overlay image with favicon on top
    QPixmap basePixmap = baseIcon.pixmap();
    QPixmap overlayImage = overlayIcon.pixmap();
    
    QImage imageWithOverlay = createImageWithOverlay(basePixmap.toImage(), point, overlayImage.toImage());
    QPixmap pixmapWithOverlay = QPixmap::fromImage(imageWithOverlay);
   
    return HbIcon(QIcon(pixmapWithOverlay));
}

void HsBookmarkWidget::removeFavicon()
{
    QDir faviconDir(mFaviconPath);
	
    faviconDir.remove(mFaviconFileName);
}	
