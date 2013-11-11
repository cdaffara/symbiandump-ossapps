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

#ifndef _FAVORITES_WRAPPER_H_
#define _FAVORITES_WRAPPER_H_

#include "irdbexport.h"
#include "irdbwrapper.h"

class IRDB_DLL_EXPORT favoritesWrapper: public IRDBWrapper
{
public:
    favoritesWrapper();
    ~favoritesWrapper();

public:
    /*
    * RowData: [direction: in]  table row value;
    * bNewRow: [direction: out] true  for new row added to favorites table;
    *                           false for row updated;
    * return true if operation success; 
    */     
    bool putFavorites(columnMap* const RowData,
                      bool& bNewRow);    
    
public:
    /*
    * If all parameters are default,all rows in favorites will be deleted if not violating constraint;
    * condAND: [direction: in]condition for search favorites, it is AND relationship in every pair;
    * condOR:  [direction: in]condition for search favorites, it is OR  relationship in every pair;
    * return true if operation success; 
    */      
    bool deleteFavorites(const columnMap* const condAND = NULL,
                         const columnMap* const condOR = NULL);

public:
    /*
    * If all parameters are default, all rows in favorites will return;
    * condAND: [direction: in]condition for search favorites, it is AND relationship in every pair;
    * condOR:  [direction: in]condition for search favorites, it is OR  relationship in every pair;
    * return true if operation success; 
    */                       
    QList<QVariant*>* getFavorites(const columnMap* const condAND = NULL,  
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

