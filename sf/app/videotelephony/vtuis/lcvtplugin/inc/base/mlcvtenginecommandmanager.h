/*
 * mlcvtenginecommandmanager.h
 *
 *  Created on: Jun 29, 2009
 *      Author: z51wang
 */

#ifndef M_LCVTENGINECOMMANDMANAGER_H
#define M_LCVTENGINECOMMANDMANAGER_H

//  INCLUDES
#include    <e32base.h>

// FORWARD DECLARATIONS
class MVtEngCommandObserver;

// CLASS DECLARATION

/**
*  Command manager class for Video Telephone application.
*
*  @since Series 60 2.6
*/
class MLcVtEngineCommandManager
    {
    public: // New functions

        /**
        * Adds observer.
        * @param aObserver observer to be added.
        */
        virtual void AddObserverL( MVtEngCommandObserver& aObserver ) = 0;

        /**
        * Removes observer.
        * @param aObserver observer to be removed.
        */
        virtual void RemoveObserver( MVtEngCommandObserver& aObserver ) = 0;

    };


#endif /* M_LCVTENGINECOMMANDMANAGER_H */
