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
* Description: Attachment list item widget
*
*/

#ifndef NMATTACHMENTLISTITEM_H_
#define NMATTACHMENTLISTITEM_H_


#include <hbwidget.h>
#include"nmailuiwidgetsdef.h"

class HbProgressBar;
class HbTextItem;
class HbTapGesture;
class HbFrameItem;


class NMAILUIWIDGETS_EXPORT NmAttachmentListItem : public HbWidget
{
    Q_OBJECT

public:
    explicit NmAttachmentListItem(QGraphicsItem *parent = 0);
    virtual ~NmAttachmentListItem();

public:
    void setTextColor(const QColor color);
    void setBackgroundColor(const QColor color);
    void setFileNameText(const QString &fileName);
    void setFileSizeText(const QString &fileSize);
    void setProgressBarValue(const int value);
    int progressBarValue() const;
    void hideProgressBar();
    void setPressed(bool pressed);

protected: 
   virtual void gestureEvent(QGestureEvent *event);

private:
    void init();
    void constructUi();

private slots:
    void removeProgressBar();

signals: 
    void itemActivated();
    void itemLongPressed(QPointF point);
    
private:
    Q_DISABLE_COPY(NmAttachmentListItem)
    HbTextItem *mFileNameText; // Owned
    HbTextItem *mFileSizeText; // Owned
    HbProgressBar *mProgressBar; // Owned
    HbFrameItem *mBackground; // Not owned
    QColor mTextColor;
    QColor mBackgroundColor;
};

#endif /* NMATTACHMENTLISTITEM_H_ */
