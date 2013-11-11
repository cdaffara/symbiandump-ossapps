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
* Description:  
*
*/

// Orbit classes
#include <qicon.h>
#include <QVariant>
#include <QPixmap>


// -----------------------------------------------------------------------------
// QIcon::QIcon
// -----------------------------------------------------------------------------
//
QIcon::QIcon()
{   
    mName = QString();
}

// -----------------------------------------------------------------------------
// QIcon::QIcon  
// -----------------------------------------------------------------------------
//
QIcon::QIcon(const QString &iconName)
{
    mName = iconName;
}

// -----------------------------------------------------------------------------
// QIcon::QIcon  
// -----------------------------------------------------------------------------
//
QIcon::QIcon(const QPixmap &pixmap)
{
    mName = "c:\\data\\images\\bg_1.png";
}

// -----------------------------------------------------------------------------
// QIcon::QIcon  
// -----------------------------------------------------------------------------
//
QIcon::QIcon(const QIcon &icon)
{
    mName = "c:\\data\\images\\bg_1.png";
}

// -----------------------------------------------------------------------------
// QIcon::~QIcon  
// -----------------------------------------------------------------------------
//
QIcon::~QIcon()
{
}

// -----------------------------------------------------------------------------
// QIcon::isNull
// -----------------------------------------------------------------------------
//
bool QIcon::isNull() const
{
	return false;
}

// -----------------------------------------------------------------------------
// QIcon::operator==
// -----------------------------------------------------------------------------
//
bool QIcon::operator==(const QIcon   &other) const
{
    return (other.mName == mName);
}

// -----------------------------------------------------------------------------
// QIcon::operator QVariant
// Returns the icon as a QVariant.
// -----------------------------------------------------------------------------
//
QIcon::operator QVariant() const
{
    return QVariant::fromValue(*this);
}


