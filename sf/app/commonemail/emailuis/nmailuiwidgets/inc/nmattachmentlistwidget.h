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
* Description: Attachment list widget
*
*/
#ifndef NMATTACHMENTLISTWIDGET_H_
#define NMATTACHMENTLISTWIDGET_H_

#include <QColor>

#include <hbwidget.h>
#include "nmailuiwidgetsdef.h"

class NmAttachmentListItem;
class QGraphicsGridLayout;
class QStyleOptionGraphicsItem;
class QPainter;

class NMAILUIWIDGETS_EXPORT NmAttachmentListWidget : public HbWidget
{
    Q_OBJECT
        
public:
    NmAttachmentListWidget(QGraphicsItem *parent = 0);
    virtual ~NmAttachmentListWidget();
    void setTextColor(const QColor color);
    void setBackgroundColor(const QColor color);
    void insertAttachment(const int index, const QString &fileName, const QString &fileSize);
    void removeAttachment(const int index);
    void setAttachmentSize(int index, const QString &fileSize);
    int count() const;
    int progressValue(int index) const;
    void hideProgressBar(int index);
    void paint(QPainter *painter,const QStyleOptionGraphicsItem *option,QWidget *widget);

public slots:
    void setProgressBarValue(int index, int value); 

private slots:
    void handleLongPressed(QPointF point);
    void handleItemActivated();
    void orientationChanged(Qt::Orientation);
   
protected:
    virtual void changeEvent(QEvent *event);

signals:
    void itemActivated(int index);
    void longPressed(int index, QPointF point);

private: 
    void init( );
    void constructUi();
    int findItem(const QObject *obj);
    void insertItemToLayout(NmAttachmentListItem* item);
    void rearrangeLayout();
    QColor checkColor();
        
private:
    Q_DISABLE_COPY(NmAttachmentListWidget)
    QList<NmAttachmentListItem*> mItemList; //owned
    QGraphicsGridLayout *mLayout;
    Qt::Orientation mOrientation;
    QColor mTextColor;
    QColor mBackgroundColor;
};

#endif // NMATTACHMENTLISTWIDGET_H_
