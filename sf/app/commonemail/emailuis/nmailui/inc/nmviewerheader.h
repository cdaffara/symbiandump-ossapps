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
* Description: NMail viewer view header widget definition
*
*/


#ifndef NMVIEWERHEADER_H_
#define NMVIEWERHEADER_H_

#include <hbwidget.h>

class NmMessage;
class NmAddress;
class NmViewerView;
class HbTextItem;
class HbGroupBox;
class HbTextEdit;
class HbIconItem;

class NmViewerHeader : public HbWidget
{
Q_OBJECT

public:
    NmViewerHeader(QGraphicsItem *parent=0);
    virtual ~NmViewerHeader();
    void setMessage(NmMessage* message);
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void rescaleHeader(const QSizeF layoutReso);
    void setView(NmViewerView* view);
    void updateMessageData(NmMessage* message);

private slots:
    void anchorTapped(const QString &anchor);

private:
    void loadWidgets();
    void setHeaderData();
    void createExpandableHeader();
    QString formatRecipientList(const QString &sender,
                                const QList<NmAddress> &to,
                                const QList<NmAddress> &cc);
    QString addressToDisplay(const NmAddress &addr);
    
private:
    NmMessage *mMessage;                 // Not owned
    QString mSenderName;                // Not owned
    HbTextItem *mSubject;                   // Not owned
    HbTextItem *mSent;                     // Not owned
    HbIconItem *mPrioIcon;              // Not owned
    HbGroupBox *mHeaderBox;             // not owned
    HbTextEdit *mRecipientsBox;         // Not owned
    NmViewerView *mViewerView;          // Not owned

};

 #endif /* NMVIEWERHEADER_H_ */
