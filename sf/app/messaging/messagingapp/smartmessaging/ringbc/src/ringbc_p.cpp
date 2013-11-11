/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *
 */

// SYSTEM INCLUDES
#include <pathinfo.h>
#include <f32file.h>
#include <hbmessagebox.h>
#include <hbdevicenotificationdialog.h>

// USER INCLUDES
#include "ringbc_p.h"
#include "ringbctoneconverter.h"
#include <xqconversions.h>
#include "debugtraces.h"

// LOCALIZATION
#define LOC_RINGTONE_SAVED hbTrId("txt_messages_dpopinfo_ringing_tone_saved")

// ----------------------------------------------------------------------------
// RingBcPrivate::RingBcPrivate
// @see ringbc_p.h
// ----------------------------------------------------------------------------
RingBcPrivate::RingBcPrivate()

    {
    QDEBUG_WRITE("RingBcPrivate::RingBcPrivate : Enter")
    initL();
    QDEBUG_WRITE("RingBcPrivate::RingBcPrivate : Exit")

    }

// ----------------------------------------------------------------------------
// RingBcPrivate::~RingBcPrivate
// @see ringbc_p.h
// ----------------------------------------------------------------------------
RingBcPrivate::~RingBcPrivate()
    {
    QDEBUG_WRITE("RingBcPrivate::~RingBcPrivate : Enter")
    
    delete mConverter;
    
    QDEBUG_WRITE("RingBcPrivate::~RingBcPrivate : Exit")
    }

// ----------------------------------------------------------------------------
// RingBcPrivate::initL
// @see ringbc_p.h
// ----------------------------------------------------------------------------
void RingBcPrivate::initL()
    {
    QDEBUG_WRITE("RingBcPrivate::initL : Enter")
    
    mConverter = RingBCNSMConverter::NewL();
    
    QDEBUG_WRITE("RingBcPrivate::initL : Exit")
    }

// ----------------------------------------------------------------------------
// RingBcPrivate::saveTone
// @see ringbc_p.h
// ----------------------------------------------------------------------------
void RingBcPrivate::saveTone(const QString &path)
    {
    QDEBUG_WRITE("RingBcPrivate::saveTone : Enter")
    QString statusStr;
    TRAPD(error, saveToneL(path));
    if(error == KErrNone)
        {
        HbDeviceNotificationDialog::notification(QString(), LOC_RINGTONE_SAVED);
        }
    
    QDEBUG_WRITE("RingBcPrivate::saveTone : Exit")
    }

// ----------------------------------------------------------------------------
// RingBcPrivate::saveToneL
// @see ringbc_p.h
// ----------------------------------------------------------------------------
void RingBcPrivate::saveToneL(const QString& path)
    {
    QDEBUG_WRITE("RingBcPrivate::saveToneL : Enter")

    QStringList pathList = path.split(".");
    QString extension = QString(".") + pathList.at(pathList.count() - 1);

    RFs fsSession;
    User::LeaveIfError(fsSession.Connect());

    CleanupClosePushL(fsSession);

    HBufC* fileName = XQConversions::qStringToS60Desc(path);

    RFile file;
    User::LeaveIfError(file.Open(fsSession, fileName->Des(),
            EFileShareReadersOnly));

    CleanupClosePushL(file);
    TInt size=0;
    User::LeaveIfError(file.Size(size));
    
    HBufC8* dataBuf = HBufC8::NewLC(size);
    TPtr8 data(dataBuf->Des());
    User::LeaveIfError(file.Read(data, size));

    TBool valid = mConverter->IsRingToneMimeTypeL(data);

    if (valid)
        {
        HBufC* title = mConverter->TitleLC(data);
        TFileName path = PathInfo::PhoneMemoryRootPath();
        path.Append(PathInfo::SimpleSoundsPath());
        path.Append(*title);
        HBufC* fileExtension = XQConversions::qStringToS60Desc(extension);
        path.Append(fileExtension->Des());

        CFileMan* fileMan = CFileMan::NewL(fsSession);
        CleanupStack::PushL(fileMan);
        if(fileMan)
            {
            TInt err = fileMan->Copy(fileName->Des(), path, CFileMan::ECopy | CFileMan::EOverWrite);
            User::LeaveIfError(err);
            }
        CleanupStack::PopAndDestroy(2); // title,fileMan        
        }
    else
    	{
        User::Leave(KErrCorrupt);
    	}

    CleanupStack::PopAndDestroy(); // dataBuf
    CleanupStack::PopAndDestroy(); //file
    CleanupStack::PopAndDestroy(); //fsSession

    QDEBUG_WRITE("RingBcPrivate::saveToneL : Exit")
    }

// ----------------------------------------------------------------------------
// RingBcPrivate::toneTitle
// @see ringbc_p.h
// ----------------------------------------------------------------------------
QString RingBcPrivate::toneTitle(const QString &path)
    {
    QDEBUG_WRITE("RingBcPrivate::toneTitle : Enter")

    QString title;
    QStringList pathList = path.split(".");
    QString extension = pathList.at(pathList.count() - 1);

    HBufC* fileName = XQConversions::qStringToS60Desc(path);
    TRAPD(err, title = titleL(*fileName));
    if(err == KErrNone)
        {
        title.append(QChar('.'));
        title.append(extension);                
        }
     
    QDEBUG_WRITE("RingBcPrivate::toneTitle : Exit")
    return title;
    }

// ----------------------------------------------------------------------------
// RingBcPrivate::titleL
// @see ringbc_p.h
// ----------------------------------------------------------------------------
QString RingBcPrivate::titleL(const TDesC& aFileName)
    {
    QDEBUG_WRITE("RingBcPrivate::titleL : Enter")

    QString title; // initialize to null string
    RFs fsSession;
    TInt error = fsSession.Connect();
    if (error == KErrNone)
        {
        CleanupClosePushL(fsSession);

        RFile file;
        error = file.Open(fsSession, aFileName, EFileRead);

        if (error == KErrNone)
            {
            CleanupClosePushL(file);
            TInt size=0;
            User::LeaveIfError(file.Size(size));
            if (size)
                {
                HBufC8* dataBuf = HBufC8::NewLC(size);
                TPtr8 data(dataBuf->Des());
                User::LeaveIfError(file.Read(data, size));

                TBool valid = mConverter->IsRingToneMimeTypeL(data);
                if (valid)
                    {
                    HBufC* toneTitle = mConverter->TitleLC(data);
                    title = XQConversions::s60DescToQString(*toneTitle);
                    CleanupStack::PopAndDestroy(); //tonetitle
                    }
                else
                    {
                    User::Leave(KErrCorrupt);
                    }
                CleanupStack::PopAndDestroy(); //dataBuf
                }
            CleanupStack::PopAndDestroy(); //file
            }
        CleanupStack::PopAndDestroy(); //fsSession
        }
    QDEBUG_WRITE("RingBcPrivate::titleL : Exit")
    return title;
    }

//  End of File  
