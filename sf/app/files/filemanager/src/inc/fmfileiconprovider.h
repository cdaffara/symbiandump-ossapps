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
*     The header file of file icon provider of filemanager
*/

#ifndef FMFILEICONPROVIDER_H
#define FMFILEICONPROVIDER_H

#include <QFileIconProvider>

class FmFileTypeRecognizer;

class FmFileIconProvider : public QFileIconProvider
{
public:
    FmFileIconProvider();
    virtual ~FmFileIconProvider();

    // from QFileIconProvider
    virtual QIcon icon(const QFileInfo &info) const;
    
private:

    FmFileTypeRecognizer *mFileTypeRecognizer;

};

#endif
