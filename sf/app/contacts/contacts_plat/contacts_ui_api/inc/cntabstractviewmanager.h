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
#ifndef CNTABSTRACTVIEWMANAGER_H_
#define CNTABSTRACTVIEWMANAGER_H_

#include <QObject>
#include <qmobilityglobal.h>
#include <cntviewparams.h>

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntAbstractViewManager
{
public:
    virtual ~CntAbstractViewManager() {}
    
    /* New API */
    virtual void changeView( const CntViewParameters aArgs ) = 0;
    virtual void back( const CntViewParameters aArgs, bool toRoot = false ) = 0;
};
#endif /* CNTABSTRACTVIEWMANAGER_H_ */
