/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
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

#ifndef CPEVENT_H
#define CPEVENT_H

#include <cpcategoryglobal.h>
#include <QString>
#include <QEvent>

class CpPluginInterface;
class HbDataFormModelItem;
class CpItemDataHelper;

class CpCreatePluginItemDataEvent : public QEvent
{
public:
    enum { CreatePluginItemData = QEvent::User + 1 };
    
    CpCreatePluginItemDataEvent() : 
        QEvent ( static_cast<QEvent::Type>(CreatePluginItemData) ),
        mPluginInterface(0),
        mParentItem(0),
        mItemDataHelper(0),
        mItemPosition(-1)
    {   
    }
    
    CpPluginInterface *mPluginInterface;
    
    HbDataFormModelItem *mParentItem;
    int mItemPosition;
    
    CpItemDataHelper *mItemDataHelper;  
    
    QString mDisplayName;
    QString mDescription;
    QString mPluginFile;
};

CP_CATEGORY_EXPORT int createCpPluginItemData(CpCreatePluginItemDataEvent *event);

#endif //CPEVENT_H

//End of File

