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
 *     The header file for tone list model
 *     
 */

#ifndef TONEFETCHERMODEL_H
#define TONEFETCHERMODEL_H

#include <QStringListModel>
#include <QStringList>

// CLASS DECLARATION
/**
 *  This class is used for storing tone list items.
 *  inherited from QStringListModel so that many existing functions could be used.
 *
 */
class ToneFetcherModel : public QStringListModel
{
public:
    explicit ToneFetcherModel( QObject *parent );
    virtual ~ToneFetcherModel();
    //from QStringListModel
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;        
    QString getPath(const QModelIndex &index) const;
    //sort the list
    void sort();
    //sort method
    bool static caseSensitiveLessThan(const QString &s1, const QString &s2);
    //emit the signal of layoutToBeChanged();
    void layoutToBeChanged();
    //emit the signal of layoutChanged();
    void layoutHasChanged();
};

#endif /* TONEFETCHERMODEL_H */
