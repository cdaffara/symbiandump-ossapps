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

#include "cntcontactcarddataitem.h"

CntContactCardDataItem::CntContactCardDataItem(const QString& aTitle, int aPosition, bool aIsFocusable) :
    mIcon(HbIcon()),
    mSecondaryIcon(HbIcon()),
    mTitle(aTitle),
    mValueText(QString("")),
    mAction(QString("")),
    mActionDescriptor(QContactActionDescriptor()),
    mValueTextElideMode(Qt::ElideNone),
    mIsFocusable(aIsFocusable),
    mExtension(NULL),
    mExtensionActionIndex(0),
    mPosition(aPosition)
{
    
}

CntContactCardDataItem::~CntContactCardDataItem()
{

}

void CntContactCardDataItem::setTitleText(const QString& aTitle)
{
    mTitle = aTitle;
}

void CntContactCardDataItem::setValueText(
    const QString& aValueText,
    Qt::TextElideMode aValueTextElideMode)
{
    mValueText = aValueText;
    mValueTextElideMode = aValueTextElideMode;
}

void CntContactCardDataItem::setIcon(HbIcon aIcon)
{
    mIcon = aIcon;
}

void CntContactCardDataItem::setSecondaryIcon(HbIcon aSecondaryIcon)
{
    mSecondaryIcon = aSecondaryIcon;
}

void CntContactCardDataItem::setAction(const QString& aAction)
{
    mAction = aAction;
}

void CntContactCardDataItem::setContactDetail(QContactDetail aDetail)
{
    mDetail = aDetail;
}

void CntContactCardDataItem::setActionDescriptor(const QContactActionDescriptor& aActionDescriptor)
{
    mActionDescriptor = aActionDescriptor;
}

void CntContactCardDataItem::setLongPressText(const QString& aLongPressText)
{
    mLongPressText = aLongPressText;
}

void CntContactCardDataItem::setContactCardExtensionAndActionIndex( CntUiContactCardExtension& aExtension, int aIndex )
{
    mExtension = &aExtension;
    mExtensionActionIndex = aIndex;
}

