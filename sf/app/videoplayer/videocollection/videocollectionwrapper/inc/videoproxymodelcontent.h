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
* Description: VideoProxyModelContent class definition
* 
*/

#ifndef VIDEOPROXYMODELCONTENT_H
#define VIDEOPROXYMODELCONTENT_H

// INCLUDES
#include <qsortfilterproxymodel.h>
#include <mpxitemid.h>
#include "videocollectioncommon.h"
#include "videoproxymodelgeneric.h"

// CLASS DECLARATION
class VideoProxyModelContent : public VideoProxyModelGeneric 
{
    Q_OBJECT
    
public: 
    
    /**
     * Contructor.
     *
     * @param QObject parent object.
     */
    VideoProxyModelContent(QObject *parent = 0);
    
    /**
     * Destructor.
     *
     */
    virtual ~VideoProxyModelContent();
    
public:
    
	/**
	 * Gets the currently opened item.
	 * 
	 * @param None.
	 * @return TMPXItemId.
	 */
    TMPXItemId getOpenItem() const;
    
protected:
        
    /**
     * Connects all signals emitted from or throught this object.
     * 
     * @return bool
     */
    bool connectSignals();
    
    /**
     * Disconnects all signals.
     */
    void disconnectSignals();
    
protected: // from QSortFilterProxyModel
    
    /**
     * Filtering functionality. Called by the FW.
     *  
     * @param source_row row to check
     * @param source_parent row's parent
     * 
     * @return bool row accepted(true) or not accepted(false)
     */
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;

private slots:
    
    /**
     * Signaled when particular album content has changed and 
     * refiltering is required.
     */
    void albumChangedSlot();

};

#endif // VIDEOPROXYMODELCONTENT_H
