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
* Description:   VideoServiceUriFetch STUB class definition
*
*/

// Version : %version:  1 %

#ifndef __VIDEOSERVICESURIFETCH_H__
#define __VIDEOSERVICESURIFETCH_H__

#include <qobject>

class VideoServices;

class VideoServiceUriFetch : public QObject
{
    Q_OBJECT

    public:
    
        VideoServiceUriFetch( VideoServices *parent = 0 );
        ~VideoServiceUriFetch(); 
        
};

#endif //__VIDEOSERVICESURIFETCH_H__
