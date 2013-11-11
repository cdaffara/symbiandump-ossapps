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

#ifndef LCFMODEL_H
#define LCFMODEL_H

#include <QAbstractListModel>

class LogsCntFinder;

/**
 *
 */
class LcfModel : public QAbstractListModel 
{
    Q_OBJECT
    
public:
    

public: 

    LcfModel( LogsCntFinder& finder );
    ~LcfModel();

public: // From QAbstractItemModel
    
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

public: // 
    
    void reset( bool enabled ) {mEnabled=enabled;}
    
private: //data 
    
    LogsCntFinder& mFinder;
    bool mEnabled;
    
};

#endif //LCFMODEL_H
