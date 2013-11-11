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
#ifndef CXEIMAGEDATAITEMSYMBIAN_H
#define CXEIMAGEDATAITEMSYMBIAN_H

//  Include Files
#include <f32file.h> // for RFile and RFs
#include "cxeimagedataitem.h"
#include "cxestatemachine.h"


class CxeImageDataItemSymbian : public CxeImageDataItem,
                                public CxeStateMachine
{
    Q_OBJECT
public: // from CxeImageDataItemSymbian
    CxeImageDataItemSymbian(QByteArray data,
                            QString filename,
                            int id,
                            bool addLocation,
                            CxeImageDataItem::State state = CxeImageDataItem::SavePending);

    virtual ~CxeImageDataItemSymbian();

    CxeImageDataItem::State state() const;
    CxeError::Id save();
    int id() const;
    bool isLocationEnabled() const;

public: // new methods
    QString path() const;

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);
    void initializeStates();

protected:
    virtual bool checkDiskSpace(RFs *aFs, TInt aBytesToWrite, TInt aDrive);

private: // private member functions
    void trySave();
    void closeHandles();

private: // private data members

    int mIndex;
    int mId;

    // used for image saving
    QByteArray mData;
    bool mAddLocationInfo;
    QString mPath;
    RFile mFile;
    RFs mFs;
};



#endif  // __CXEIMAGEDATAITEMSYMBIAN_H__
