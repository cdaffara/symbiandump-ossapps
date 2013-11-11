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
 * Description: caentry.h
 *
 */

#ifndef CAENTRY_H
#define CAENTRY_H

#include <HbIcon>
#include <QMap>
#include <QSharedDataPointer>
#include <QSize>
#include <cadefs.h>

#include "caicondescription.h"
class HbIcon;

class CaEntry
{

public:
    mutable EntryRole mCaEntryRole;
    mutable int mIdResult;
    mutable QString mTextResult;
    mutable CaIconDescription mSetCaIconDescription;
    mutable EntryFlags mFlagsResult;
    mutable QString mEntryTypeNameResult;
    mutable QMap<QString, QString> mAttributes;
    mutable QSizeF mMakeIconSize;
    mutable HbIcon mMakeIconResult;
    explicit CaEntry(EntryRole entryRole = ItemEntryRole):
        mCaEntryRole(entryRole),
        mIdResult(0)
    {
        mCaEntryRole = entryRole;
    }
    
    int id() const
    {
        return mIdResult;
    }

    QString text() const
    {
        return mTextResult;
    }
    
    void setText(const QString &text)
    {
        mTextResult = text;
    }

    void setIconDescription(const CaIconDescription &iconDescription)
    {
        mSetCaIconDescription = iconDescription;
    }
    
    EntryFlags flags() const
    {
        return mFlagsResult;
    }
    
    void setFlags(EntryFlags flags)
    {
        mFlagsResult = flags;
    }

    QString entryTypeName() const
    {
        return mEntryTypeNameResult;
    }
        
    void setEntryTypeName(const QString &entryTypeName)
    {
        mEntryTypeNameResult = entryTypeName;
    }
    
    void setAttribute(const QString &name, const QString &value)
    {
        mAttributes.insert(name, value);
    }
    
        
    void removeAttribute(const QString &name)
    {
        mAttributes.remove(name);
    }
    
    QString attribute(const QString &name) const
    {
        return mAttributes[name];
    }
    
    HbIcon makeIcon(const QSizeF &size = QSize(70, 70)) const
    {
        mMakeIconSize = size;
        return mMakeIconResult;
    }
    
    EntryRole role() const
    {
        return mCaEntryRole;
    }
    
    // test helper, not belonging to ContentStorage CaEntry
    void clear()
    {
        mCaEntryRole = ItemEntryRole;
        mIdResult = 0;
        mTextResult.clear();
        mSetCaIconDescription.clear();
        mFlagsResult = 0;
        mEntryTypeNameResult.clear();  
        mAttributes.clear();
        mMakeIconSize.setHeight(-1);
        mMakeIconSize.setWidth(-1);
    }
};

#endif // CAENTRY_H
