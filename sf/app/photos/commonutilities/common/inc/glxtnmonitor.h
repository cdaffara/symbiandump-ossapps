/*
* ============================================================================
*  Name        : glxscreensavermonitor.h
*  Part of     : GLX / Collection Data Source Framework
*  Description : 
*  Version     :%version:  1 %
*  Copyright © 2006-2007 Nokia.  All rights reserved.
*  This material, including documentation and any related computer
*  programs, is protected by copyright controlled by Nokia.  All
*  rights are reserved.  Copying, including reproducing, storing,
*  adapting or translating, any or all of this material requires the
*  prior written consent of Nokia.  This material also contains
*  confidential information which may not be disclosed to others
*  without the prior written consent of Nokia.
* ============================================================================
*/

#ifndef PLUGINSCREENSAVERMONITORAO_H_
#define PLUGINSCREENSAVERMONITORAO_H_

#include <e32base.h>
#include <e32property.h>

class MGlxTNObserver
{
public :
    /*
     * Constructor
     */
    MGlxTNObserver() {}
    /*
     * Call back to get the number is left to create.
     */
    virtual void updateTNCount(int & count) = 0;
    /*
     * Destructor
     */
    virtual ~MGlxTNObserver() {}    
};


/**
* Class for monitoring harvester pause.
*/
class CGlxTNMonitor: public CActive
{
    public:
        /** Construction */
    	IMPORT_C static CGlxTNMonitor* NewL( MGlxTNObserver *observer );

        /**
         * Destructor - virtual and class not intended
         * for derivation, so not exported.
         */
    	virtual ~CGlxTNMonitor();

    protected:

    	/**
    	 * RunL
    	 * From CActive.
    	 */
        virtual void RunL();

    	/**
    	 * DoCancel
    	 * From CActive.
    	 */
        virtual void DoCancel();

    	/**
    	 * RunError. Error handler in case RunL() leaves.
    	 * From CActive.
    	 * @param aError  Leave code from RunL.
    	 * @return  Error code. KErrNone if error was handled.
    	 */
        virtual TInt RunError( TInt aError );
        
    private:
        /**
         * constructor
         */
        CGlxTNMonitor( MGlxTNObserver *observer );

        /**
         * 2nd phase construction.
         */    
        void ConstructL();

    private:
        RProperty iScreenSaverProperty;
//		TTime iStartTime;
//		TTime iStopTime;
		MGlxTNObserver *iObserver;
};

	
#endif

