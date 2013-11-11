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
* Description: LocationPickerProxyModel declaration
*
*/

#ifndef LOCATIONPICKERPROXYMODEL_H
#define LOCATIONPICKERPROXYMODEL_H


#include <QSortFilterProxyModel>


/**  Class used for sorting and filtering model items
*
*/
class LocationPickerProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    // constructor
    LocationPickerProxyModel( QObject *parent = 0 );

    // used to
    void filterParameterChanged( QString aSearchText );

protected:
    // override QSortFilterProxyModel function  to define logic for filter
    bool filterAcceptsRow( int sourceRow, const QModelIndex &sourceParent ) const;
    // override QSortFilterProxyModel function  to define logic for sort
    bool lessThan( const QModelIndex &left, const QModelIndex &right ) const;

private:
    // search string
    QString mSearchText;
};
 
 
#endif // LOCATIONPICKERPROXYMODEL_H
