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
 * Description: 
 *
 */

#ifndef NMHSWIDGETTITLEROW_H_
#define NMHSWIDGETTITLEROW_H_

#include <hbwidget.h>

//FORWARD DECLARATIONS:
class HbLabel;
class HbPushButton;
class HbFrameItem;
class HbDocumentLoader;

class NmHsWidgetTitleRow : public HbWidget
{
    Q_OBJECT
  
public:
    NmHsWidgetTitleRow(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~NmHsWidgetTitleRow();
    QPainterPath shape() const;
    qreal containerHeight();
    bool setupUI(HbDocumentLoader &loader); 
    void setAccountIcon(const QString& accountIconName );
    void setExpandCollapseIcon(const bool& expand);
    void gestureEvent(QGestureEvent *event);
    
private:
    bool loadDocML(HbDocumentLoader &loader);
    bool setupGraphics();
    void updateData();
    void setHighlighedFontsColor( bool pressed );
    void showHighlight( bool pressed );
    
public slots: 
    void updateAccountName(const QString& accountName );
    void updateUnreadCount(const int& unreadCount );

signals:
    void expandCollapseButtonPressed();
    void mailboxLaunchTriggered();
    
protected:
    bool event( QEvent *event );
    
private:
    HbWidget *mContainer;
    HbLabel *mMailboxIcon;
    HbLabel *mMailboxInfo;
    HbLabel *mUnreadCountLabel;
    HbPushButton *mCollapseExpIconLabel;
    QString mAccountName;
    int mUnreadCount;
    HbFrameItem* mBackgroundLayoutItem;
};

#endif // NMHSWIDGETTITLEROW_H_
