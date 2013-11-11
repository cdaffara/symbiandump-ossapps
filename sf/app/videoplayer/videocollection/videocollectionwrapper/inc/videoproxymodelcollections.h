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
* Description: VideoProxyModelCollections class definition
* 
*/

#ifndef VIDEOPROXYMODELCOLLETIONS_H
#define VIDEOPROXYMODELCOLLETIONS_H

// INCLUDES
#include <qsortfilterproxymodel.h>
#include <mpxitemid.h>
#include "videocollectioncommon.h"
#include "videoproxymodelgeneric.h"

// CLASS DECLARATION
class VideoProxyModelCollections : public VideoProxyModelGeneric 
{
    Q_OBJECT
    
public: 
    
    /**
     * Contructor.
     *
     * @param QObject parent object.
     */
    VideoProxyModelCollections(QObject *parent = 0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoProxyModelCollections();
    
protected: // from QSortFilterProxyModel
    
    /**
     * Compares items based in the role setted before sorting.
     *  
     */
    bool lessThan(const QModelIndex & left, const QModelIndex & right) const;
    
    /**
     * Filtering functionality. Called by the fw.
     *  
     * @param source_row row to check
     * @param source_parent row's parent
     * 
     * @return bool row accepted(true) or not accepted(false)
     */
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
    
};

#endif // VIDEOPROXYMODELCOLLETIONS_H
