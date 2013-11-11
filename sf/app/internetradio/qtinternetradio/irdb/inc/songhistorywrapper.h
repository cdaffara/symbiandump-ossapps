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
* Description: 
*
*/

#ifndef _SONG_HISTORY_WRAPPER_H_
#define _SONG_HISTORY_WRAPPER_H_

#include "irdbexport.h"
#include "irdbwrapper.h"

class IRDB_DLL_EXPORT songHistoryWrapper: public IRDBWrapper
{
public:
    songHistoryWrapper();
    ~songHistoryWrapper();

public:
    /*
    * RowData: [direction: in]table row value;
    * condAND: [direction: in]condition for search songHistory, it is AND relationship in every pair;
    * condOR:  [direction: in]condition for search songHistory, it is OR  relationship in every pair;
    */      
    bool putSongHistory(const columnMap* const RowData,
                        const columnMap* const condAND = NULL,
                        const columnMap* const condOR = NULL);                         

    /*
    * If all parameters are default,all rows in songHistory will be deleted if not violating constraint;
    * condAND: [direction: in]condition for search songHistory, it is AND relationship in every pair;
    * condOR:  [direction: in]condition for search songHistory, it is OR  relationship in every pair;
    */      
    bool deleteSongHistory(const columnMap* const condAND = NULL,
                           const columnMap* const condOR = NULL);

    /*
    * If all parameters are default, all rows in songHistory will return;
    * cond1: condition for search songHistory, it is AND relationship in every pair;
    * cond2: condition for search songHistory, it is OR  relationship in every pair;
    */                       
    QList<QVariant*>* getSongHistory(const columnMap* const condAND = NULL,  
                                     const columnMap* const condOR = NULL);

private:
    /*
    * It is called by IRDB instance;
    * IRDB instance delivers rows set to IRDBWrapper via this function;
    * the rows set is storaged in m_IRDataSet;
    */
    bool getIRTableCB(QSqlQuery& aIRDataSet,
                      QList<QVariant*>* pDataSet);

};

#endif
