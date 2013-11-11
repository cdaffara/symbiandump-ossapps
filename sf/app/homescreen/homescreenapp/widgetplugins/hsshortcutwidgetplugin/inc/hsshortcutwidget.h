/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Home screen shortcut widget.
*
*/

#ifndef HSSHORTCUTWIDGET_H
#define HSSHORTCUTWIDGET_H

#include <HbWidget>

#include "cadefs.h"

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(TestShortcutWidget)

class HbFrameItem;    
class HbIconItem;
class HbTextItem;
class QGestureEvent;
class CaEntry;

class HsShortcutWidget : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(int caEntryId READ caEntryId WRITE setCaEntryId)
    Q_PROPERTY(QString uid READ uid WRITE setUid)
    
    Q_PROPERTY(QString text READ text WRITE setText )
    
public:
    HsShortcutWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~HsShortcutWidget();

    void setCaEntryId(int caEntryId);
    int caEntryId() const;
    void setUid(const QString &uid);
    QString uid() const;

    QString text() const;
    void setText(const QString& textItem);

signals:
    void finished();
    void setPreferences(const QStringList &names);

public slots:
    void onInitialize();
    void onShow();
    void onHide();

protected:
    void gestureEvent(QGestureEvent *event);

private:
    Q_DISABLE_COPY(HsShortcutWidget)
    
    void launch();
            
    void createPrimitives();
    void updateContent(const CaEntry &caEntry);
    void updateVisibility();

    void setBackgroundToNormal();
    void setBackgroundToPressed();

    void createCaNotifier();

private slots:
    void onEntryChanged(const CaEntry &caEntry, ChangeType changeType);

private:
    HbFrameItem *mBackground;
    HbIconItem *mIcon;
    HbTextItem *mText;
        
    int mCaEntryId;
    QString mUid;
    EntryRole mCaEntryRole;
    EntryFlags mCaEntryFlags;
    QString mCaEntryTypeName;
    HOMESCREEN_TEST_FRIEND_CLASS(TestShortcutWidget)
};

#endif // HSSHORTCUTWIDGET_H
