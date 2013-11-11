// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __OBEXHARNESS_H__
#define __OBEXHARNESS_H__

#include <sendas.h>     // MSendAsObserver
#include <obexmtmutil.h>
#include "harness.h"    // CMsvClientTest
#include "obextestheaderlist.h"

#include "cobextestfilenameandheaders.h"

// Forward declarations
class CObexTestUtils;
class RTest;

class CObexClientTest : public CMsvClientTest, public MSendAsObserver
/**
 * The CObexClientTest class is the main repository for information
 * shared between test states. It runs these states using functions
 * inherited from CMsvClientTest.
 */
    {
    public:
        /**
         * Factory function which initialises the class. This version is
         * passed in an existing MMS Test Utils object, of which it does not
         * take ownership.
         * @param aTestUtils A reference to an ObexTestUtils object
         * @param aTest The test object which provides a console, etc.
         */
        static CObexClientTest* NewL(CObexTestUtils& aTestUtils, RTest& aTest);
        /**
         * Factory function which initialises the class. This version is
         * passed in an existing MMS Test Utils object, of which it does
         * take ownership.
         * @param aTestUtils A reference to an ObexTestUtils object
         * @param aTest The test object which provides a console, etc.
         */
        static CObexClientTest* NewL(CObexTestUtils* aTestUtils, RTest& aTest);
        /**
         * C++ destructor.
         */
        virtual ~CObexClientTest();
    
        /**
         * Writes a comment to the error log.
         * @param aError The comment to write to the log.
         */
        void LogCommentL(const TDesC& aError);
    
        /**
         * Deletes messages and the test utils object.
         */
        void Reset();
    
        /**
         * Returns a reference to the MMS test utilities object.
         * @return A reference to the test utilities.
         */
        inline CObexTestUtils& ObexTestUtils();
    
        /**
         * Creates the CSendAs object and its associated message.
         */
        void CreateSendAsL();
        /**
         * Returns a pointer to the SendAs object.
         * @return A pointer to the CSendAs object.
         */
        inline CSendAs* SendAs();
    
        /**
         * Sets the frequency with which progress of operations is checked.
         * @param aGranularity The granularity of the progress timer, in ms.
         */
        inline void SetProgressGranularity(TInt aGranularity);
        /**
         * Gets the frequency with which progress of operations is checked.
         * @return aGranularity The granularity of the progress timer, in ms.
         */
        inline TInt ProgressGranularity();
    
        /**
         * Sets the time after which operations will be cancelled.
         * @param aGranularity The granularity of the cancel timer, in ms.
         */
        inline void SetCancelGranularity(TInt aGranularity);
        /**
         * Sets the time after which operations will be cancelled.
         * @return The granularity of the cancel timer, in ms.
         */
        inline TInt CancelGranularity();

        /**
         * This class inherits from MSendAsObserver so that the SendAs class can
         * be instantiated, and hence needs to provide this function. It always returns true.
         * @return ETrue.
         */
        inline TBool CapabilityOK(TUid aCapability, TInt aResponse);

        /**
         * Returns a flag to indicate whether or not the failure of a send operation results in the termination of the test harness
         * @return The value of the flag for ignoring send errors
         */
        inline TBool IgnoreSendErrors();

        /**
         * Sets a flag to indicate whether or not the failure of a send operation results in the termination of the test harness
         * @param aFlag The new value of the flag for ignoring send errors
         */
        inline void SetIgnoreSendErrors(TBool aFlag);

		/**
		 * Over ridden for test all passed logging.
		 */
		void RunL();
 
		/**
         * Returns a pointer to the current obex headers list object.
         * @return A pointer to the CObexHeaderList object.
         */
        inline CObexHeaderList* CurrentHeaders();

		/**
         * Returns a pointer to the verify obex headers list object.
         * @return A pointer to the CObexHeader object.
         */
        inline CObexTestHeaderList* VerifyHeaders();

		/**
         * Clear the headers added to the current headers list object.
         */
		void ClearCurrentHeadersL();

		/**
         * .
         */
		void ReleaseCurrentHeaders();
		 

		/**
         * Clear the headers added to the verify headers list object.
         */
		void ClearVerifyHeadersL();

		inline RPointerArray<CObexTestFilenameAndHeaders>& Attachments();


		void ClearAttachmentsL();

    protected:
        /**
         * Second-stage constructor. Simply calls the base class' ConstructL().
         */
        virtual void ConstructL();
        /**
         * C++ constructor. This version is passed in an existing MMS Test
         * Utils object, of which it does not take ownership.
         * @param aTestUtils A reference to an ObexTestUtils object
         * @param aTest The test object which provides a console, etc.
         */
        CObexClientTest(CObexTestUtils& aTestUtils, RTest& aTest);
        /**
         * C++ constructor. This version is passed in an existing MMS Test
         * Utils object, of which it does not take ownership.
         * @param aTestUtils A reference to an ObexTestUtils object
         * @param aTest The test object which provides a console, etc.
         */
        CObexClientTest(CObexTestUtils* aTestUtils, RTest& aTest);


    protected:
        // A handle to the MMS test utils object.
        CObexTestUtils& iObexTestUtils;

    private:
        CObexTestUtils* iOwnedObexTestUtils;  //< A pointer to the MMS test utils object, if the class owns it.
        TBool iSessionReset;  //< Whether the session has been reset and the message objects deleted.
        CSendAs* iSendAs;  //< A pointer to the SendAs object owned by the test harness.
        TInt iProgressGranularity;  //< The granularity of the progress timer - how frequently to request progress, in ms
        TInt iCancelGranularity;  //< The granularity of the cancellation timer - how quickly to cancel operations, in ms
        TBool iIgnoreSendErrors;    //< A flag to indicate whether or not the failure of a send operation results in the termination of the test harness
		TBool iAllTestPassed;		//<A flag to indicate if all the test states passed
		CObexHeaderList* iCurrentHeaders;
		CObexTestHeaderList* iVerifyHeaders;
		RPointerArray<CObexTestFilenameAndHeaders> iAttachmentsList;
    };


#include "ObexHarness.inl"


#endif // __OBEXHARNESS_H__
