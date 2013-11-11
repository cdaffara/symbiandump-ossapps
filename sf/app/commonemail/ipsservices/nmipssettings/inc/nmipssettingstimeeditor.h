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

#ifndef NMIPSSETTINGSTIMEEDITOR_H
#define NMIPSSETTINGSTIMEEDITOR_H

#include <QDateTime>
#include <HbWidget>

class QGraphicsItem;
class HbPushButton;
class HbDialog;
class HbAction;

class NmIpsSettingsTimeEditor : public HbWidget
{
    Q_OBJECT

    Q_PROPERTY(QTime time READ time WRITE setTime)
    Q_PROPERTY(QString heading READ heading WRITE setHeading)

public:

    NmIpsSettingsTimeEditor(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    virtual ~NmIpsSettingsTimeEditor();

    QTime time() const;
    void setTime(QTime time);

    QString heading() const;
    void setHeading(QString label);

signals:

    void timeChanged(QTime time);

private slots:

    void launchTimePicker();
    void handleTimeAction(HbAction *action);

private:

    QString timeFormat();

private: // data

    // Now owned.
    HbPushButton *mButton;

    // Owned.
    HbDialog *mTimePickerDialog;

    QTime mTime;
    QString mLabel;

    // Not owned.
    HbAction *mPrimaryAction;
};

#endif // NMIPSSETTINGSTIMEEDITOR_H
