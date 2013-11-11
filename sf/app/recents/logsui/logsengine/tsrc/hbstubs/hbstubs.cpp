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
#include <hbnotificationdialog.h>
#include <hbicon.h>
#include <hblineedit.h>
#include <QVariant>
#include <hbstringutil.h>
#include "hbstubs_helper.h"



QString mUTClipboard;
static QIcon* logsTestIcon = 0;

QIcon* HbIcon::defaultIcon()
{
    return logsTestIcon;
}

#define ENSURE_DEFAULT_ICON \
if ( !logsTestIcon ){ \
    logsTestIcon = new QIcon(); \
}

// -----------------------------------------------------------------------------
// HbStubsHelper
// -----------------------------------------------------------------------------
//

QString testNotifDialogText;
bool testConversionEnabled = false;

void HbStubHelper::reset()
{
    testNotifDialogText.clear();
    testConversionEnabled = false;
}

QString HbStubHelper::notificationDialogTxt()
{
    return testNotifDialogText;
}

void HbStubHelper::stringUtilDigitConversion(bool enabled)
{
    testConversionEnabled = enabled;
}


// -----------------------------------------------------------------------------
// HbIcon::HbIcon
// -----------------------------------------------------------------------------
//
HbIcon::HbIcon()
{   
    mName = QString();
    ENSURE_DEFAULT_ICON
}

// -----------------------------------------------------------------------------
// HbIcon::HbIcon
// -----------------------------------------------------------------------------
//
HbIcon::HbIcon(const QString &iconName)
{
    mName = iconName;
    ENSURE_DEFAULT_ICON
}

// -----------------------------------------------------------------------------
// HbIcon::HbIcon
// -----------------------------------------------------------------------------
//
HbIcon::HbIcon(const QIcon &icon)
{
    mName = "c:\\data\\images\\designer.png";
    ENSURE_DEFAULT_ICON
}

// -----------------------------------------------------------------------------
// HbIcon::HbIcon
// -----------------------------------------------------------------------------
//
HbIcon::HbIcon(const HbIcon &icon)
{
    mName = icon.mName;
    ENSURE_DEFAULT_ICON
}

// -----------------------------------------------------------------------------
// HbIcon::~HbIcon
// -----------------------------------------------------------------------------
//
HbIcon::~HbIcon()
{
}

// -----------------------------------------------------------------------------
// HbIcon::isNull
// -----------------------------------------------------------------------------
//
bool HbIcon::isNull() const
{
	return false;
}

// -----------------------------------------------------------------------------
// HbIcon::qicon
// -----------------------------------------------------------------------------
//
QIcon &HbIcon::qicon() const
{
    return *logsTestIcon;
}

// -----------------------------------------------------------------------------
// HbIcon::operator==
// -----------------------------------------------------------------------------
//
bool HbIcon::operator==(const HbIcon &other) const
{
    return (other.mName == mName);
}

// -----------------------------------------------------------------------------
// HbIcon::operator QVariant
// Returns the icon as a QVariant.
// -----------------------------------------------------------------------------
//
HbIcon::operator QVariant() const
{
    return QVariant::fromValue(*this);
}

// -----------------------------------------------------------------------------
// HbLineEdit::HbLineEdit
// -----------------------------------------------------------------------------
//
HbLineEdit::HbLineEdit()
{   
    //mName = QString();
}

// -----------------------------------------------------------------------------
// HbLineEdit::HbLineEdit
// -----------------------------------------------------------------------------
//
HbLineEdit::HbLineEdit(const QString &lineEditText)
{
    //mName = iconName;
}

// -----------------------------------------------------------------------------
// HbLineEdit::~HbLineEdit
// -----------------------------------------------------------------------------
//
HbLineEdit::~HbLineEdit()
{
}

// -----------------------------------------------------------------------------
// HbLineEdit::setText
// -----------------------------------------------------------------------------
//
void HbLineEdit::setText(const QString &text)
{
	mText = text;
}
// -----------------------------------------------------------------------------
// HbLineEdit::selectAll
// -----------------------------------------------------------------------------
//
void HbLineEdit::selectAll()
{
}
// -----------------------------------------------------------------------------
// HbLineEdit::selectAll
// -----------------------------------------------------------------------------
//
void HbLineEdit::setSelection(int start, int length)
{
    Q_UNUSED(start);
    Q_UNUSED(length);
}
// -----------------------------------------------------------------------------
// HbLineEdit::copy
// -----------------------------------------------------------------------------
//
void HbLineEdit::copy()
{
	 mUTClipboard = mText;
}
// -----------------------------------------------------------------------------
// HbLineEdit::paste
// -----------------------------------------------------------------------------
//
void HbLineEdit::paste()
{
	mText = mUTClipboard;
}
// -----------------------------------------------------------------------------
// HbLineEdit::text
// -----------------------------------------------------------------------------
//
QString HbLineEdit::text() const
{
	return mText;
}

// -----------------------------------------------------------------------------
// HbNotificationDialog::launchDialog
// -----------------------------------------------------------------------------
//
void HbNotificationDialog::launchDialog(const QString &title, QGraphicsScene *scene)
{
    Q_UNUSED(scene);
    testNotifDialogText = title;
}

// -----------------------------------------------------------------------------
// HbStringUtil
// -----------------------------------------------------------------------------
//

QString HbStringUtil::convertDigits( const QString str )
{
    // Normally this method converts to current locale digits but for testing purpose
    // is enough to see just that some conversion occured.
    return convertDigitsTo(str, WesternDigit);
}

QString HbStringUtil::convertDigitsTo( const QString str, const DigitType digitType )
{
    Q_UNUSED(digitType);
    if ( testConversionEnabled && !str.isEmpty() ){
        return ( QString("conv") + str );
    }
    return str;
}
