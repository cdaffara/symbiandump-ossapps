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
 * Description: caicondescription.h
 *
 */

#ifndef CAICONDESCRIPTION_H
#define CAICONDESCRIPTION_H

#include <QSharedDataPointer>
#include <QString>

// mocks Content Storage CaIconDescrption
class  CaIconDescription
{

public:
    QString mSetFileName;
    void setFilename(const QString &fileName)
    {
        mSetFileName = fileName;
    }
    
    // test helper, not belonging to Content Storage CaIconDescription
    void clear()
    {
        mSetFileName.clear();
    }
};

#endif // CAICONDESCRIPTION_H
