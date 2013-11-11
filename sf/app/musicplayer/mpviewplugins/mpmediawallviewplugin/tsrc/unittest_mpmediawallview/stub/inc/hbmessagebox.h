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
* Description: hbmessagebox stub for testing mpmediawallview
*
*/

#ifndef HBMESSAGEBOX_H
#define HBMESSAGEBOX_H

#include <QObject>
#include <qglobal.h>
#include <QMetaObject>
#include <hbicon.h>

//  FORWARD DECLARATIONS
class QGraphicsItem;
class HbIcon;

/*!
     Stub HbMessageBox class.
 */
class HbMessageBox : public QObject
{
    Q_OBJECT

public: // Data types

    enum MessageBoxType {
        MessageTypeNone,
        MessageTypeInformation,
        MessageTypeQuestion,
        MessageTypeWarning
    };

public:

    HbMessageBox(const QString &text, MessageBoxType type = MessageTypeNone, QGraphicsItem *parent = 0)
    {
        Q_UNUSED(text);
        Q_UNUSED(type);
        Q_UNUSED(parent);
        mMessageDisplayed = false;
    }

    ~HbMessageBox()
    {
    }

    void setAttribute(Qt::WidgetAttribute att, bool on = true)
    {
        Q_UNUSED(att);
        Q_UNUSED(on);
    }

    void setIcon(const HbIcon &icon)
    {
        Q_UNUSED(icon);
    }

    void show()
    {
        mMessageDisplayed = true;
    }

public:
    bool    mMessageDisplayed;
};

#endif // HBMESSAGEBOX_H

