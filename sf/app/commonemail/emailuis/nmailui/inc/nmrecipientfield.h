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
* Description: Email Address Field Widget
*
*/

#ifndef NMRECIPIENTFIELD_H_
#define NMRECIPIENTFIELD_H_

#include <hbwidget.h>

class QGraphicsLinearLayout;
class HbLabel;
class NmRecipientLineEdit;
class HbPushButton;
class HbIconItem;
class HbDocumentLoader;
class XQAiwRequest;

class NmRecipientField : public QObject
{
    Q_OBJECT
public:
    NmRecipientField(QObject *parent, HbDocumentLoader &docLoader, const QString &objPrefix);
    virtual ~NmRecipientField();
    qreal height();
    void createConnections();

signals:
    void textChanged(const QString &text);
    void cursorPositionChanged(int oldPos, int newPos);
    void editingFinished();
    void selectionChanged();
    void launchContactsPickerButtonClicked();

public:
    NmRecipientLineEdit *editor() const;
    HbLabel *label() const;
    const QString text() const;

public slots:
    void setText(const QString &text);
    void launchContactsPicker();
    void requestCompleted();

private:
    HbDocumentLoader &mDocumentLoader;
    const QString mObjectPrefix;
    HbWidget *mWidget; // Not owned
    HbLabel *mLabel; // Not owned
    NmRecipientLineEdit *mRecipientsEditor; // Not owned
    HbPushButton *mLaunchContactsPickerButton; // Not owned
    XQAiwRequest *mAiwRequest;
};

#endif   // NMRECIPIENTFIELD_H_
