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
* Description: VideoProxyModelAllVideos class definition
* 
*/

#ifndef VIDEOPROXYMODELALLVIDEOS_H
#define VIDEOPROXYMODELALLVIDEOS_H

// INCLUDES
#include <qsortfilterproxymodel.h>
#include <mpxitemid.h>
#include "videocollectioncommon.h"
#include "videoproxymodelgeneric.h"

// CLASS DECLARATION
class VideoProxyModelAllVideos : public VideoProxyModelGeneric 
{
    Q_OBJECT
    
public: 
    
    /**
     * Contructor.
     *
     * @param QObject parent object.
     */
    VideoProxyModelAllVideos(QObject *parent = 0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoProxyModelAllVideos();
    
public:
    
	/**
	 * Gets the currently opened item.
	 * 
	 * @param None.
	 * @return TMPXItemId.
	 */
    TMPXItemId getOpenItem() const;
	
protected: // from QSortFilterProxyModel
    
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

#endif // VIDEOPROXYMODELALLVIDEOS_H
