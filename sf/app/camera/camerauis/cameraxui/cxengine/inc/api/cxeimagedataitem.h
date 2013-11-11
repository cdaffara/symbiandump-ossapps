/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CXEIMAGEDATAITEM_H
#define CXEIMAGEDATAITEM_H

#include <QObject>
#include <QString>
#include <QMetaType>

#include "cxeerror.h"

/**
 * CxeImageDataItem represents a single object that is in a queue to be saved to
 * the file system.
 */
class CxeImageDataItem : public QObject
{
    Q_OBJECT

public:
    // State Machine
    enum State {
        SavePending = 0x01,
        Saving      = 0x02,
        Saved       = 0x04,
        SaveFailed  = 0x08
        };

    /**
     * Helper method to save file to Fs
     * @return symbian error code
     */
    virtual CxeError::Id save() = 0;

    /**
     * Get the current state for this item.
     *
     * @return State
     */
    virtual State state() const = 0;

    /**
     * Get the id of this item.
     */
    virtual int id() const = 0;

    /**
     * Get the id of this item.
     */
    virtual QString path() const = 0;

signals:
    void imageSaved(CxeError::Id error, const QString &filename, int id);
    void stateChanged(State newState, CxeError::Id error);

protected:
    CxeImageDataItem() {}

private:
    Q_DISABLE_COPY(CxeImageDataItem)
};

Q_DECLARE_METATYPE(CxeImageDataItem::State)

#endif // CXEIMAGEDATAITEM_H
