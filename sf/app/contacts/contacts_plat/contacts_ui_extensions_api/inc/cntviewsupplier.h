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

#ifndef CNTVIEWSUPPLIER_H
#define CNTVIEWSUPPLIER_H

class CntAbstractView;

/**
 * UI extension class that supplies views to phonebook UI.
 */
class CntViewSupplier
{
public:

    /**
     * Returns number of views provided by the extension.
     *
     * @return int, view count
     */
    virtual int viewCount() const = 0;

    /**
     * Returns the view ID of a specific view.
     *
     * @param int, view index 
     * @return int, viewId
     */
    virtual int viewIdAt( int aIndex ) const = 0;

    /**
     * Returns a specific view. Ownership is given.
     *
     * @param int, view index 
     * @return CntAbstractView*, view
     */
    virtual CntAbstractView* viewAt( int aIndex ) = 0;

    /**
     * Sets the view id base value for supplier. 
     * The supplier method viewIdAt should return view ids in range
     * from aViewIdBase to aViewIdBase + viewCount.
     *
     * @param int, action index 
     * @return QString, process name
     */
    virtual void setViewIdBase( int aViewIdBase ) = 0;

protected:
    // prevent deleting by client
    virtual ~CntViewSupplier() {}
};

#endif //CNTVIEWSUPPLIER_H

