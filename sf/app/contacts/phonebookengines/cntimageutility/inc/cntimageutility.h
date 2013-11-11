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
#ifndef CNTIMAGEUTILITY_H
#define CNTIMAGEUTILITY_H

#include <qglobal.h>
#include <QObject>

#include <f32file.h>

#include "cntimageutilityglobal.h"

class QPixmap;

class CNTIMAGEUTILITYLIB_EXPORT CntImageUtility : public QObject
{
    Q_OBJECT
    
public:
    enum Error {
            NoError = 0,
            NotSupportedError,
            UnspecifiedError,
        };
    
	CntImageUtility(QObject *parent = 0);
	~CntImageUtility();
	
	bool createImage(const QString& filePath, QString& imagePath);
	bool createImage(const QPixmap& pixmap, QString& imagePath);
	bool removeImage(const QString& imagePath);
	bool isImageRemovable(const QString& filePath);
	bool isMassStorageAvailable();
	CntImageUtility::Error error();
	
private:
	int selectDrive(int &driveIndex);
	bool initPath(QString &path, int drive);

private:
	RFs m_Fs;
	CntImageUtility::Error m_error;
};

#endif
