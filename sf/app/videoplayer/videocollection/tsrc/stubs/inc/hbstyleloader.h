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
* Description:  stub hbaction 
* 
*/

#ifndef HBSTYLELOADER_H
#define HBSTYLELOADER_H

#include <qglobal.h>
#include <QObject>
#include <QString>

class  HbStyleLoader : QObject
{
    Q_OBJECT
public:
    /**
     * Contructor.
     */       
    HbStyleLoader( const QString txt )
    {
        Q_UNUSED(txt);
        initializeCount++;
    };
    
    /**
     * destructor
     */
   virtual ~HbStyleLoader(){initializeCount--;}
   
   static bool registerFilePath(const QString &filePath) 
   { 
       Q_UNUSED(filePath);
       return mRegisterFilePathReturnValue; 
   }
   
   static bool unregisterFilePath(const QString &filePath) 
   { 
       Q_UNUSED(filePath);
       return true;
   }
    
    /**
     * counter to make sure alloc dealloc match
     */
    static int initializeCount;
    
    /**
     * returned from registerFilePath
     */
    static bool mRegisterFilePathReturnValue;
    
signals:
            
};

#endif
