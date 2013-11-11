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
 * Description:This class provides duration of media file by parsing it
 *
 */
#include <MsgMediaResolver.h>
#include <xqconversions.h>
#include <HbExtendedLocale>
#include <QTime>

#include "msgmediautil.h"

// @see hbi18ndef.h
static const char TIME_FORMAT[] = r_qtn_time_durat_min_sec_with_zero;

//---------------------------------------------------------------
// MsgMediaUtil::MsgMediaUtil
// Constructor
//---------------------------------------------------------------
MsgMediaUtil::MsgMediaUtil()
    {
    
    }

//---------------------------------------------------------------
// MsgMediaUtil::MsgMediaUtil
// Desctructor
//---------------------------------------------------------------
MsgMediaUtil::~MsgMediaUtil()
    {
    
    }

//---------------------------------------------------------------
// MsgMediaUtil::mediaDuration
// @see header file
//---------------------------------------------------------------
QString MsgMediaUtil::mediaDuration(const QString& mediaFile)
    {
    QString formattedDuration;
    
    HBufC *name = XQConversions::qStringToS60Desc(mediaFile);
    TInt duration = 0;
    TRAPD(error, duration = mediaDurationL(*name));
    delete name;
    
    if ( error || duration <= 0)
        return formattedDuration;

    // Media duration in milliseconds.
    QTime mediaDuration(0, 0);
    mediaDuration = mediaDuration.addMSecs(duration);
    HbExtendedLocale locale = HbExtendedLocale::system();
    formattedDuration = locale.format(mediaDuration, TIME_FORMAT);
    return formattedDuration;
    }

//---------------------------------------------------------------
// MsgMediaUtil::mediaDurationL
// @see header file
//---------------------------------------------------------------
TInt MsgMediaUtil::mediaDurationL(const TDesC& mediaFile)
    {
    CMsgMediaResolver* resolver = CMsgMediaResolver::NewLC();
    resolver->SetCharacterSetRecognition(EFalse);
    
    RFile rFile = resolver->FileHandleL(mediaFile);
    CleanupClosePushL( rFile );

    CMsgMediaInfo* info = resolver->CreateMediaInfoL( rFile );
    resolver->ParseInfoDetailsL(info, rFile);
    CleanupStack::PushL( info );

    TInt duration = info->Duration();
    
    CleanupStack::PopAndDestroy(3, resolver); // info, rFile, resolver
    
    return duration;
    }


// EOF

