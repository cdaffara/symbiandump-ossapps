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
* Description: Orbit stubs for Live Comms UI unit tests
*
*/
#ifndef QICON_H
#define QICON_H

#include <QMetaType>
class QIcon
{
public:
    QIcon();
    QIcon(const QString &iconName);
    QIcon(const QIcon &icon);
    QIcon(const QPixmap &pixmap);
    ~QIcon();
    bool isNull() const;
    bool operator==(const QIcon &other) const;
    
    operator QVariant() const;
    
public: // Data added for the unit testing stub
    QString mName;    
};

Q_DECLARE_METATYPE(QIcon)

#endif // QICON_H
