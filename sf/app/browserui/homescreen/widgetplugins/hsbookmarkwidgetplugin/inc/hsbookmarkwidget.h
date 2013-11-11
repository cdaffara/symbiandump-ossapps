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

#ifndef HSBOOKMARKWIDGET_H
#define HSBOOKMARKWIDGET_H

#include <HbWidget>

class HbFrameItem;    
class HbIconItem;
class HbTextItem;
class HbTouchArea;

class HsBookmarkWidget : public HbWidget
{
    Q_OBJECT
    
    Q_PROPERTY(QString rootPath READ rootPath WRITE setRootPath)
    
    // Persistent property of the homescreen bookmark widget. They are stored in homescreen
    // WidgetProperty of homescreen.db.
    Q_PROPERTY(QString bookmarkTitle READ bookmarkTitle WRITE setBookmarkTitle)
    Q_PROPERTY(QString bookmarkUrl READ bookmarkUrl WRITE setBookmarkUrl)
    Q_PROPERTY(QString faviconFileName READ faviconFileName WRITE setFaviconFileName)
    Q_PROPERTY(QString faviconPath READ faviconPath WRITE setFaviconPath)

public:
    HsBookmarkWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~HsBookmarkWidget();
    
    enum {
        maxDisplayLen = 6
    };
    
    enum {
        faviconPoint = 37
    };
    
    enum {
        faviconSize = 35
    };
    
    void setRootPath(const QString &rootPath);
    QString rootPath()const;
    
    void setBookmarkTitle(const QString &bookmarkTitle);
    QString bookmarkTitle() const;
    void setBookmarkUrl(const QString &bookmarkUrl);
    QString bookmarkUrl() const;
    void setFaviconFileName(const QString &faviconFileName);
    QString faviconFileName() const;
    void setFaviconPath(const QString &faviconPath);
    QString faviconPath() const;
    
    bool eventFilter(QObject *watched, QEvent *event);

public slots:

    void onInitialize();
    void onShow();
    void onHide();

private:
    
    void handleMousePressEvent(QGraphicsSceneMouseEvent *event);
    void handleMouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void handleMouseReleaseEvent(QGraphicsSceneMouseEvent *event);
            
    void createPrimitives();

    void setBackgroundToNormal();
    void setBackgroundToPressed();
    
    void updateContent(const QString title, const HbIcon& icon);
    
    QImage createImageWithOverlay(const QImage& baseImage, const QPoint& point, const QImage& overlayImage);
    HbIcon createIconWithOverlay(HbIcon& baseIcon, const QPoint& point, HbIcon& overlayIcon);
    
    void removeFavicon();
    
private:
    Q_DISABLE_COPY(HsBookmarkWidget)
	  
    HbFrameItem *mBackground;
    HbIconItem *mIcon;
    HbTextItem *mText;
    HbTouchArea *mTouchArea;
    
    QString mRootPath;
    QString mBookmarkTitle;
    QString mBookmarkUrl;
    QString mFaviconFileName;
    QString mFaviconPath;
};

#endif // HSBOOKMARKWIDGET_H
