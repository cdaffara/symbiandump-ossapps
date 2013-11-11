// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// $Workfile: ObexParsers.h $
// $Author: Simonk $
// $Revision: 4 $
// $Date: 22/01/02 11:31 $
// 
//


#ifndef __OBEXPARSERS_H__
#define __OBEXPARSERS_H__


// System includes
// User includes
#include "parsers.h"        // CMainSectionParser


// Forward declarations
class CMainTestHarness;
class CObexClientTest;
class CTestScript;


//
// CObexMainSectionParser
//
class CObexMainSectionParser: public CMainSectionParser
/**
 * This class parses the [main] section of OBEX test scripts. There
 * can only be one main section in a script.
 */
    {
    public:
        /**
         * Factory function which creates a new main section parser.
         * @param aMainTest A handle to the MMS test harness.
         * @param aScript A handle to the MMS test script.
         * @return The new main section parser object.
         */
        static CObexMainSectionParser* NewL(CMainTestHarness& aMainTest, CTestScript& aScript);

    protected:
        void ConstructL();
        CObexMainSectionParser(CMainTestHarness& aMainTest, CTestScript& aScript);
    //  void LogCommentL(const TDesC& aComment);

    private:
        CMainTestHarness& iMainTest;
    };



//
// CNewObexTestCommandParser
//
class CNewObexTestCommandParser : public CBaseCommandParser
/**
 * This class implements the standard command parser interface.
 * The only command it parses is the "obex_client_test" command in the
 * [main] section.
 */
    {
    public:
        /**
         * Function which implements the standard command parser interface
         * to parse supported commands.
         */
        void ProcessL();
        /**
         * Factory function which creates a new command parser.
         * @param aScript A handle to the MMS test script.
         * @param aMainTest A handle to the MMS test harness.
         * @return The new command parser object.
         */
        static CNewObexTestCommandParser* NewL(CTestScript& aScript, CMainTestHarness& aMainTest);
    private:
        /**
         * Second stage constructor. Sets up the list of commands this
         * parser can parse.
         */
        void ConstructL();
        /** C++ constructor.
         * @param aScript A handle to the MMS test script.
         * @param aMainTest A handle to the MMS test harness.
         */
        CNewObexTestCommandParser(CTestScript& aScript, CMainTestHarness& aMainTest);
        CMainTestHarness& iMainTest;
        CTestScript& iScript;
    };



//
// CObexClientSectionParser
//
class CObexClientSectionParser : public CBaseSectionParser
/**
 * This class parses the OBEX client test section of a test script. It
 * creates an OBEX client test command parser to parse the commands.
 */
    {
    public:
        /**
         * Factory function which creates a new OBEX client test section parser.
         * @param aMainTest A handle to the OBEX test harness.
         * @param aScript A handle to the OBEX test script.
         */
        static CObexClientSectionParser* NewL(CObexClientTest& aObexClientTest, CTestScript& aScript, const TDesC& aNewSectionName);
        
    protected:
        void LogCommentL(const TDesC& aComment);
        void ConstructL(const TDesC& aNewSectionName);
        CObexClientSectionParser(CObexClientTest& aObexTest, CTestScript& aScript);

    private:
        CObexClientTest& iObexTest;
        CTestScript& iScript;
    };

class CObexClientTestCommandParser : public CBaseCommandParser
/**
 * This class parses the commands that are currently in the "obex_client_test" section of a script
 * file.
 */
    {
    public:
        /**
         * Function which implements the standard command parser interface
         * to parse supported commands.
         */
        void ProcessL();
        /**
         * Factory function which creates a new command parser.
         * @param aScript A handle to the OBEX test script.
         * @param aObexTest A handle to an OBEX test harness to which this parser adds commands.
         * @return The new command parser object.
         */
        static CObexClientTestCommandParser* NewL(CTestScript& aScript, CObexClientTest& aObexTest);
    private:
        /**
         * Second stage constructor. Sets up the list of commands this
         * parser can parse.
         */
        void ConstructL();
        /** C++ constructor.
         * @param aScript A handle to the OBEX test script.
         * @param aMainTest A handle to an OBEX test harness to which this parser adds commands.
         */
        CObexClientTestCommandParser(CTestScript& aScript, CObexClientTest& aObexTest);
        
    private:
        CTestScript& iScript;
        CObexClientTest& iObexTest;
    };


#endif // __OBEXPARSERS_H__
