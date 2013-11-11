/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Logger implementation.
*
*/



// ========
// INCLUDES
// ========

#include <e32std.h>
#include <f32file.h>
#include <sysutil.h>

#include "DebugLog.h"
#include "DebugLogConst.h"

// This takes a lot more memory to use, so it is defaulted to off.  If the code is crashing
// in DoLogLeakedObjects then uncomment this line to help find the problem.  It is most likely
// caused by a DLL that was explicitly unloaded, making the original copy of the class name
// unavailable.
// <cmail>
// #define __MAKE_COPY_OF_NAMES 
// </cmail>

// : IMPLEMENT THIS USING RDebugLog::Open/Close AND class name/object address

// This provides additional help for debugging memory leaks.  Uncomment out the following line
// in order to log all constructions/destructions from all objects from here, rather than requiring
// recompilation of all code.
//#define __LOG_CONSTRUCTION_AND_DESTRUCTION

// =========
// CONSTANTS
// =========

const TInt KDateOrTimeMaxLength=30;

_LIT( KDateTimeFormat,"%1%/1%2%/2%3 %H%:1%T%:2%S.%*C3 " );

_LIT( KUnderscore, "_" );

_LIT( KBackslash, "\\" );

_LIT8(KEndOfLineCharacters8,"\r\n");

_LIT8( KTwoColons, "::" );

_LIT8( KColonAndSpace, ": " );

const TText8 KFullStopChar8='.';

const TText8 KTabChar8='\t';

const TInt KFormattedBufferSize = 350;

const TInt KMaxTextPerLine = 150;

const TInt KMaxClassAndFuncNameLength = 35;

// ================
// INTERNAL CLASSES
// ================

// ============================================
// 8-bit formatter overflow handler
// ============================================
class TDebugLogBase8Overflow : public TDes8Overflow
{
public:
	virtual void Overflow( TDes8& aDes );
};

// ============================================
// 16-bit formatter overflow handler and buffer
// ============================================
class TDebugLogBase16Overflow : public TDes16Overflow
{
public:
	virtual void Overflow( TDes16& aDes );
};

// ============================================================
// CLogFileHandler
// This class handles a single log file for the current thread.
// Note that the filenames are the same for all log files in
// this thread, but the directory names may be different.
// ============================================================
class CLogFileHandler : public CBase
{
public:

    RPointerArray<RDebugLog> iObjects;
    
//#ifdef __MAKE_COPY_OF_NAMES    
    RPointerArray<HBufC8> iObjectNames;
//#endif   

    CLogFileHandler( RFs&         aFs,
                     TDes&        aFormatBuffer, 
                     TDes8&       aOutputBuffer,
                     const TDesC& aDirectoryName,
                     const TDesC& aFileName );

    virtual ~CLogFileHandler();
    
    // returns ETrue if this handler is the log handler for the
    // given directory
    TBool IsHandler( const TDesC& aDirectoryName );
        
    // returns ETrue if the log file is open and ready for writing        
	TBool ReadyToWrite();
	
	// tries to open the log file and sets internal flags
	// aCloseFile - set to ETrue if the file should be closed during the call
	void TryToOpenFile( TBool aCloseFile = EFalse );
	
	// closes the log file
    void CloseFile();

    // flushes the log file
    void Flush();

    void WriteFormat( const TDesC8&                aClassName,
                      const TDesC8&                aFuncName,
                      TRefByValue< const TDesC16 > aFmt,
                      VA_LIST&                     aList );
                      
    void WriteFormat( const TDesC8&               aClassName,
                      const TDesC8&               aFuncName,
                      TRefByValue< const TDesC8 > aFmt,
                      VA_LIST&                    aList );
      
	void Write( const TDesC8&  aClassName,
                const TDesC8&  aFuncName,
                const TDesC16& aDes );
	
	void Write( const TDesC8& aClassName,
                const TDesC8& aFuncName,
                const TDesC8& aDes );
	
	void HexDump( const TDesC8& aClassName,
                  const TDesC8& aFuncName,
                  const TDesC8& aBuffer );
	
private:

    RFile iFile;

    void WriteLineL( const TDesC8& aDes );

    void WriteLineL( const TDesC16& aDes );
    
    void AddTimestampToOutputBufferL();
    
    void AddClassAndFunctionToOutputBufferL( const TDesC8& aClassName,
                                             const TDesC8& aFuncName );
    
    RFs&         iFs;
    TDes&        iFormatBuffer;
    TPtr8        iFormatBuffer8;
    TDes8&       iOutputBuffer;
    const TDesC& iDirectoryName;
    const TDesC& iFileName;
    TBool        iTryToOpenFile;
    TBool        iFileIsOpen;
    TBool        iFirstTime;
    
}; // END CLogFileHandler
    
// ================================================================
// CDebugLogTlsData
// This class is the singleton object that manages all instances of
// the log file handler for the current thread.
// ================================================================
class CDebugLogTlsData : private CActive
{
public:

    static CLogFileHandler* GetLogFileHandler( const TDesC& aDirectory, RDebugLog* aObject );
    
    static void ReleaseLogFileHandler( CLogFileHandler* aHandler, RDebugLog* aObject );
        
    static CDebugLogTlsData* Instance();
    
    static void FreeIfNecessary();
        
    static void LogLeakedObjects();
        
private:    

    CDebugLogTlsData();
    
    TBool Initialize();
        
    virtual ~CDebugLogTlsData();

    CLogFileHandler* DoGetLogFileHandler( const TDesC& aDirectory, RDebugLog* aObject );

    void DoReleaseLogFileHandler( CLogFileHandler* aHandler, RDebugLog* aObject );
    
    void DoLogLeakedObjects();

	// inherited from CActive
	void RunL();
	void DoCancel();

    RFs                            iFs;
    TFileName                      iFileName;
    RPointerArray<CLogFileHandler> iFiles;    
    TBuf<KFormattedBufferSize>     iFormatBuffer;
   	TBuf8<KMaxTextPerLine+2>       iOutputBuffer;  // two extra bytes for end of line characters
    
}; // END CDebugLogTlsData

// ======================
// METHOD IMPLEMENTATIONS
// ======================

// ==========================================================================
// METHOD:  Constructor
//
// DESIGN:  
// ==========================================================================
EXPORT_C RDebugLog::RDebugLog()
    {
    iLogFileHandler = NULL;
    } // END RDebugLog

// ==========================================================================
// METHOD:  CleanupClosePushL
//
// DESIGN:  
// ==========================================================================    
EXPORT_C void RDebugLog::CleanupClosePushL()
    {
    CleanupStack::PushL( TCleanupItem(&RDebugLog::StaticClose, this) );
    } // END CleanupClosePushL

// ==========================================================================
// METHOD:  Open
//
// DESIGN:  Initialize this object.
// ==========================================================================
EXPORT_C void RDebugLog::Open( const TDesC&  aLogDirectory,
                               const TDesC8& aClassName8,
                               TAny*         aObjectAddress )
    {
    iObjectAddress = aObjectAddress;
    iClassName8.Set( aClassName8 );
    iLogDirectory.Set( aLogDirectory );
    
#ifdef DEBUG_LOG_CLOSE_BETWEEN_WRITES_AS_DEFAULT    
    iCloseBetweenWrites = ETrue;
#else
    iCloseBetweenWrites = EFalse;
#endif    
 
    iLogFileHandler = CDebugLogTlsData::GetLogFileHandler( aLogDirectory, this );

#ifdef __LOG_CONSTRUCTION_AND_DESTRUCTION
    if( iObjectAddress != 0 )
        {        
        const TUint bufferSize = 20;
        TBuf8<bufferSize> buf;
        _LIT8( KObjAddrFmt, "0x%x" );
        _LIT8( KDestructor, "CONSTRUCTOR" );
        buf.Format( KObjAddrFmt, iObjectAddress );
        Write( KDestructor, buf );    
        }
#endif

    if( !iLogFileHandler )
        {        
        CDebugLogTlsData::FreeIfNecessary();
        } // end if
        
    } // END Open
    
// ==========================================================================
// METHOD:  Close
//
// DESIGN:  
// ==========================================================================
EXPORT_C void RDebugLog::Close()
    {
#ifdef __LOG_CONSTRUCTION_AND_DESTRUCTION
    if( iObjectAddress != 0 )
        {        
        const TUint bufferSize = 20;
        TBuf8<bufferSize> buf;
        _LIT8( KObjAddrFmt, "0x%x" );
        _LIT8( KDestructor, "DESTRUCTOR" );
        buf.Format( KObjAddrFmt(), iObjectAddress );
        Write( KDestructor(), buf );    
        }
#endif

    if( iLogFileHandler )
        {        
        CDebugLogTlsData::ReleaseLogFileHandler( iLogFileHandler, this );
        iLogFileHandler = NULL;

        CDebugLogTlsData::FreeIfNecessary();

        } // end if
        
    } // END Close

// ==========================================================================
// METHOD:  Destructor
//
// DESIGN:  
// ==========================================================================
EXPORT_C RDebugLog::~RDebugLog()
    {
    if( iLogFileHandler )
        {
        // Error!  This means that Open was called, but not Close.        
        if( PrepareForWrite() )
            {        
            _LIT8( KDummyFuncName, "***" );
            _LIT8( KErrorText, "DEBUG LOG NOT CLOSED!" );
        
        	iLogFileHandler->Write( iClassName8, KDummyFuncName, KErrorText );
        	WriteComplete();
            } // end if
            
        Close();            
        }    
    
    } // END Destructor

// ==========================================================================
// METHOD:  SetCloseBetweenWrites
//
// DESIGN:  
// ==========================================================================
EXPORT_C void RDebugLog::SetCloseBetweenWrites( TBool aValue )
    {
   iCloseBetweenWrites = aValue;
        
    } // END SetCloseBetweenWrites

// ==========================================================================
// METHOD:  Write
//
// DESIGN:  
// ==========================================================================
EXPORT_C void RDebugLog::Write( const TDesC8& aFuncName8, 
                                const TDesC16& aText ) const
    {
	RDebugLog* self = const_cast<RDebugLog*>(this);

    if( self->PrepareForWrite() )
        {        
    	// Write the descriptor to the log file
    	iLogFileHandler->Write( iClassName8, aFuncName8, aText );
    	self->WriteComplete();
        } // end if            

    } // END Write

// ==========================================================================
// METHOD:  Write
//
// DESIGN:  
// ==========================================================================
EXPORT_C void RDebugLog::Write( const TDesC8& aFuncName8, 
                                const TDesC8& aText ) const
    {
	RDebugLog* self = const_cast<RDebugLog*>(this);

    if( self->PrepareForWrite() )
        {        
    	// Write the descriptor to the log file
    	iLogFileHandler->Write( iClassName8, aFuncName8, aText );
    	self->WriteComplete();
        } // end if

    } // END Write

// ==========================================================================
// METHOD:  WriteFormat
//
// DESIGN:  
// ==========================================================================
EXPORT_C void RDebugLog::WriteFormat( const TDesC8& aFuncName8, 
                                      TRefByValue< const TDesC16 > aFmt, 
                                      ... ) const
    {
    // coverity[UNINIT]
	VA_LIST list; 
	VA_START(list,aFmt);

	RDebugLog* self = const_cast<RDebugLog*>(this);

    if( self->PrepareForWrite() )
        {        
    	// Call WriteFormat method with argument list
    	iLogFileHandler->WriteFormat( iClassName8, aFuncName8, aFmt, list );
    	self->WriteComplete();
        } // end if

    } // END WriteFormat

// ==========================================================================
// METHOD:  WriteFormat
//
// DESIGN:  
// ==========================================================================
EXPORT_C void RDebugLog::WriteFormat( const TDesC8&                aFuncName8, 
                                      TRefByValue< const TDesC16 > aFmt,
                                      VA_LIST&                     aList ) const
    {
	RDebugLog* self = const_cast<RDebugLog*>(this);

    if( self->PrepareForWrite() )
        {        
    	// Call WriteFormat method with argument list
    	iLogFileHandler->WriteFormat( iClassName8, aFuncName8, aFmt, aList );
    	self->WriteComplete();
        } // end if

    } // END WriteFormat

// ==========================================================================
// METHOD:  WriteFormat
//
// DESIGN:  
// ==========================================================================
EXPORT_C void RDebugLog::WriteFormat( const TDesC8& aFuncName8, 
                                      TRefByValue< const TDesC8 > aFmt, 
                                      ... ) const
    {
    // coverity[UNINIT]
	VA_LIST list; 
	VA_START(list,aFmt);

	RDebugLog* self = const_cast<RDebugLog*>(this);

    if( self->PrepareForWrite() )
        {        
    	// Call WriteFormat method with argument list
    	iLogFileHandler->WriteFormat( iClassName8, aFuncName8, aFmt, list );
    	self->WriteComplete();
        } // end if

    } // END WriteFormat

// ==========================================================================
// METHOD:  WriteFormat
//
// DESIGN:  
// ==========================================================================
EXPORT_C void RDebugLog::WriteFormat( const TDesC8&               aFuncName8, 
                                      TRefByValue< const TDesC8 > aFmt,
                                      VA_LIST&                    aList ) const
    {
	RDebugLog* self = const_cast<RDebugLog*>(this);

    if( self->PrepareForWrite() )
        {        
    	// Call WriteFormat method with argument list
    	iLogFileHandler->WriteFormat( iClassName8, aFuncName8, aFmt, aList );
    	self->WriteComplete();
        } // end if

    } // END WriteFormat

// ==========================================================================
// METHOD:  HexDump
//
// DESIGN:  
// ==========================================================================
EXPORT_C void RDebugLog::HexDump( const TDesC8& aFuncName8,
                                  const TDesC8& aBuffer ) const
    {
	RDebugLog* self = const_cast<RDebugLog*>(this);

    if( self->PrepareForWrite() )
        {        
    	// Call WriteFormat method with argument list
    	iLogFileHandler->HexDump( iClassName8, aFuncName8, aBuffer );
    	self->WriteComplete();
        } // end if

    } // END HexDump
        
// ==========================================================================
// METHOD:  PrepareForWrite
//
// DESIGN:  
// ==========================================================================
TBool RDebugLog::PrepareForWrite()
    {
    if( !iLogFileHandler )
        {
        iLogFileHandler = CDebugLogTlsData::GetLogFileHandler( iLogDirectory, this );
        } // end if
       
    return (iLogFileHandler) && iLogFileHandler->ReadyToWrite();
           
    } // END PrepareForWrite
    
// ==========================================================================
// METHOD:  WriteComplete
//
// DESIGN:  
// ==========================================================================
void RDebugLog::WriteComplete()
    {
    if( iCloseBetweenWrites )
        {
        iLogFileHandler->CloseFile();
        }
    else
        {
    	iLogFileHandler->Flush();	        
        } // end if
       
    } // END WriteComplete
    
// ==========================================================================
// METHOD:  StaticClose
//
// DESIGN:  
// ==========================================================================
void RDebugLog::StaticClose( TAny* ptr )
    {
    RDebugLog* debugLog = reinterpret_cast<RDebugLog*>(ptr);
    debugLog->Close();
    } // end StaticClose
    
// ==========================================================================
// METHOD:  LogLeakedObjects
//
// DESIGN:  
// ==========================================================================
EXPORT_C void RDebugLog::LogLeakedObjects()
    {
    CDebugLogTlsData::LogLeakedObjects();
    }
    
TPtrC8 RDebugLog::ClassName8()
    {
    return iClassName8;
    }

TUint32 RDebugLog::ObjectAddress()
    {
    return (TUint32)iObjectAddress;
    }

// ---------------
// CLogFileHandler
// ---------------

// ==========================================================================
// METHOD:  Constructor
//
// DESIGN:  
// ==========================================================================
CLogFileHandler::CLogFileHandler( RFs& aFs,
                                  TDes& aFormatBuffer, 
                                  TDes8& aOutputBuffer,
                                  const TDesC& aDirectoryName,
                                  const TDesC& aFileName ) : 
    iFs( aFs ),                                  
    iFormatBuffer( aFormatBuffer ),
    iFormatBuffer8( const_cast<TUint8*>(reinterpret_cast<const TUint8*>(aFormatBuffer.Ptr())), 0, aFormatBuffer.MaxLength()*2 ),
    iOutputBuffer( aOutputBuffer ),
    iDirectoryName( aDirectoryName ),
    iFileName( aFileName ),
    iTryToOpenFile( ETrue ),
    iFileIsOpen( EFalse ),
    iFirstTime( ETrue )
    {
    } // END CLogFileHandler

// ==========================================================================
// METHOD:  Destructor
//
// DESIGN:  
// ==========================================================================
CLogFileHandler::~CLogFileHandler() 
    {
//#ifdef __MAKE_COPY_OF_NAMES    
    iObjectNames.ResetAndDestroy();
//#endif   
    iObjects.Close();
    iFile.Close();
    } // END ~CLogFileHandler

// ==========================================================================
// METHOD:  IsHandler
//
// DESIGN:  
// ==========================================================================
TBool CLogFileHandler::IsHandler( const TDesC& aDirectoryName )
    {
    return (iDirectoryName.Compare( aDirectoryName ) == 0);
    } // end IsHandler        

// ==========================================================================
// METHOD:  ReadyToWrite
//
// DESIGN:  
// ==========================================================================
TBool CLogFileHandler::ReadyToWrite()
    { 
    if( iTryToOpenFile )
        {
        TryToOpenFile(); 
        } // end if    
           
    return iFileIsOpen;
    
    } // END ReadyToWrite

// ==========================================================================
// METHOD:  TryToOpenFile
//
// DESIGN:  
// ==========================================================================
void CLogFileHandler::TryToOpenFile( TBool aCloseFile )
    {
    if( !iFileIsOpen )
        {        
        // Add the directory name to the start of the file name, to avoid
        // many files with the same name in different directories.
    	TFileName logFileName;
    	logFileName.Copy( KDebugLogsBaseDirectory );
    	logFileName.Append( iDirectoryName );   	
        logFileName.Append( KBackslash );
    	logFileName.Append( iDirectoryName );  
    	logFileName.Append( KUnderscore );
        logFileName.Append( iFileName );

    	TInt result = iFile.Open( iFs, logFileName, EFileWrite | EFileShareAny );
    	
    	if( result == KErrNotFound )
    	    {
        	result = iFile.Create( iFs, logFileName, EFileWrite | EFileShareAny );
    	    } // end if

        if( iFirstTime )
            {
            iFirstTime = EFalse;
            
            // Put a marker in the file.            
            if( result == KErrNone )
                {
                // Write the marker line and software version.
                TBuf<KSysUtilVersionTextLength> text;
                SysUtil::GetSWVersion(text);

                const TUint bufSize = KSysUtilVersionTextLength+30;
                TBuf8<bufSize> text8;

                _LIT8( KMarkerLine, "------------------" );
                
                text8.Copy( text );
                text8.Insert( 0, KEndOfLineCharacters8 );
                text8.Insert( 0, KMarkerLine );
                text8.Append( KEndOfLineCharacters8 );

                iFile.Write( KMaxTInt, text8 );
                }
            }

        if( aCloseFile )
            {
            iFile.Close();
            }
        else
            {            
            // Leave the file open.
            iFileIsOpen = (result == KErrNone );                    
            } // end if
            
        // Only try next time if this open succeeded.
        iTryToOpenFile = (result == KErrNone );

        } // end if
        
    } // END TryToOpenFile

// ==========================================================================
// METHOD:  CloseFile
//
// DESIGN:  
// ==========================================================================
void CLogFileHandler::CloseFile()
    {
    iFile.Close();
    
    iFileIsOpen = EFalse;
    
    } // END CloseFile

// ==========================================================================
// METHOD:  Flush
//
// DESIGN:  
// ==========================================================================
void CLogFileHandler::Flush()
    {
    iFile.Flush();
    
    } // END Flush

// ==========================================================================
// METHOD:  WriteFormat
//
// DESIGN:  
// ==========================================================================
void CLogFileHandler::WriteFormat( const TDesC8&                aClassName,
                                   const TDesC8&                aFuncName,
                                   TRefByValue< const TDesC16 > aFmt,
                                   VA_LIST&                     aList )
    {
	TDebugLogBase16Overflow overflow16;
	
	iFormatBuffer.SetLength( 0 );
	iFormatBuffer.AppendFormatList( aFmt, aList, &overflow16 );

	Write( aClassName, aFuncName, iFormatBuffer );

    } // END WriteFormat

// ==========================================================================
// METHOD:  WriteFormat
//
// DESIGN:  
// ==========================================================================
void CLogFileHandler::WriteFormat( const TDesC8&               aClassName,
                                   const TDesC8&               aFuncName,
                                   TRefByValue< const TDesC8 > aFmt,
                                   VA_LIST&                    aList ) 
    {
	TDebugLogBase8Overflow overflow8;
	
	iFormatBuffer8.SetLength( 0 );
	iFormatBuffer8.AppendFormatList( aFmt, aList, &overflow8 );

	Write( aClassName, aFuncName, iFormatBuffer8 );

    } // END WriteFormat

// ==========================================================================
// METHOD:  Write
//
// DESIGN:  Write the given buffer.  Chunk the string if it is longer than
//          KMaxTextPerLine. 
// ==========================================================================
void CLogFileHandler::Write( const TDesC8&  aClassName,
                             const TDesC8&  aFuncName,
                             const TDesC16& aDes )
    {
    TRAP_IGNORE( AddTimestampToOutputBufferL() );
        
    TInt prefixLength = iOutputBuffer.Length();
        
    TRAP_IGNORE( AddClassAndFunctionToOutputBufferL( aClassName, aFuncName ) );
            
    TInt curPos = 0;
    
	while( curPos < aDes.Length() )
	    {
        TInt lengthToWrite = Min( KMaxTextPerLine - iOutputBuffer.Length(), aDes.Length() - curPos );
        
		TRAP_IGNORE( WriteLineL( aDes.Mid( curPos, lengthToWrite ) ) );
		    
		iOutputBuffer.SetLength( prefixLength );
				    
		curPos += lengthToWrite;

    	} // end while
    
    } // END Write

// ==========================================================================
// METHOD:  Write
//
// DESIGN:  Write the given buffer.  Chunk the string if it is longer than
//          KDebugLogBufferSize. 
// ==========================================================================
void CLogFileHandler::Write( const TDesC8& aClassName,
                             const TDesC8& aFuncName,
                             const TDesC8& aDes )
    {
    TRAP_IGNORE( AddTimestampToOutputBufferL() );
        
    TInt prefixLength = iOutputBuffer.Length();
        
    TRAP_IGNORE( AddClassAndFunctionToOutputBufferL( aClassName, aFuncName ) );
            
    TInt curPos = 0;
    
	while( curPos < aDes.Length() )
    	{
        TInt lengthToWrite = Min( KMaxTextPerLine - iOutputBuffer.Length(), aDes.Length() - curPos );        

    	TRAP_IGNORE( WriteLineL( aDes.Mid( curPos, lengthToWrite ) ) );
		    
		iOutputBuffer.SetLength( prefixLength );
				    
		curPos += lengthToWrite;

    	} // end while

    } // END Write

// ==========================================================================
// METHOD:  AddTimestampToOutputBufferL
//
// DESIGN:  
// ==========================================================================
void CLogFileHandler::AddTimestampToOutputBufferL()
    {
    // Add timestamp
    
	TBuf<KDateOrTimeMaxLength> dateTimeBuffer;
	TTime t;
	t.HomeTime();
	t.FormatL(dateTimeBuffer, KDateTimeFormat);
	
	iOutputBuffer.Copy( dateTimeBuffer );		
    
    } // END AddTimestampToOutputBufferL

// ==========================================================================
// METHOD:  AddClassAndFunctionToOutputBufferL
//
// DESIGN:  
// ==========================================================================
void CLogFileHandler::AddClassAndFunctionToOutputBufferL( const TDesC8& aClassName,
                                                          const TDesC8& aFuncName )
    {
	// Add class name & function name
	
    iOutputBuffer.Append( aClassName.Right( KMaxClassAndFuncNameLength ) );
    iOutputBuffer.Append( KTwoColons );
    iOutputBuffer.Append( aFuncName.Right( KMaxClassAndFuncNameLength ) );
    iOutputBuffer.Append( KColonAndSpace );
    
    } // END AddClassAndFunctionToOutputBufferL

// ==========================================================================
// METHOD:  WriteLineL
//
// DESIGN:  
// ==========================================================================
void CLogFileHandler::WriteLineL( const TDesC16& aSrc )
	{
    // Converting from Unicode to Utf8 is overkill for debug logging.  Instead, just use the
    // check-and-dirty Append function, which converts multi-byte characters to char 1.
	// CnvUtfConverter::ConvertFromUnicodeToUtf8( iOutputBuffer, aSrc );
	
	iOutputBuffer.Append( aSrc );
	
	TChar ch;
	for( TInt i=0; i < iOutputBuffer.Length(); i++ )
		{
		ch=iOutputBuffer[i];
		if(!((ch.IsPrint()) || (ch==KTabChar8)))
		    {		    
			iOutputBuffer[i]=KFullStopChar8;
		    } // end if
		} // end for
		
	iOutputBuffer.Append( KEndOfLineCharacters8 );
	
	iFile.Write( KMaxTInt, iOutputBuffer );
	
	} // END WriteLineL

// ==========================================================================
// METHOD:  WriteLineL
//
// DESIGN:  Prepend date/time, truncate, and convert unprintable characters to '.'.
// ==========================================================================
void CLogFileHandler::WriteLineL( const TDesC8& aSrc )
	{	
    TInt i = iOutputBuffer.Length();
    
	iOutputBuffer.Append( aSrc );
	                  
	TChar ch;
	while( i < iOutputBuffer.Length() )
		{
		ch=iOutputBuffer[i];
		if(!((ch.IsPrint()) || (ch==KTabChar8)))
		    {		    
			iOutputBuffer[i]=KFullStopChar8;
		    } // end if
        i++;		    
		} // end for
		
	iOutputBuffer.Append( KEndOfLineCharacters8 );
	
	iFile.Write( KMaxTInt, iOutputBuffer );
	
	} // END WriteLineL

// ==========================================================================
// METHOD:  HexDump
//
// DESIGN:  
// ==========================================================================
void CLogFileHandler::HexDump( const TDesC8& /*aClassName*/,
                               const TDesC8& /*aFuncName*/,
                               const TDesC8& aBuffer )
    {
	TInt remainingLength = aBuffer.Length();
	TInt i = 0;
	
	while( remainingLength > 0 )
		{
        const TInt KHexDumpWidth=32;
		TInt n = Min( KHexDumpWidth, remainingLength );
		
		// Add the starting byte number.
        _LIT8(KFirstFormatString,"%04x : ");
		iFormatBuffer8.Format(KFirstFormatString, i);
		
		// Add hex values.
		TInt j;
		for (j=0; j<n; j++)
		    {		    
            _LIT8(KSecondFormatString,"%02x ");
			iFormatBuffer8.AppendFormat(KSecondFormatString, aBuffer[i+j]);
		    } // end for
			
        // Fill in incomplete lines.			
		while (j<KHexDumpWidth)
		    {		    
            _LIT8(KThreeSpaces,"   ");
			iFormatBuffer8.Append(KThreeSpaces);
			j++;
		    } // end while
		    
		// Add text representation.
        _LIT8(KTwoSpaces," ");
		iFormatBuffer8.Append(KTwoSpaces);
		for (j=0; j<n; j++)
		    {		    
            _LIT8(KThirdFormatString,"%c");
			iFormatBuffer8.AppendFormat( KThirdFormatString, aBuffer[i+j] );
		    } // end for
		
		iOutputBuffer.SetLength( 0 );		
		TRAP_IGNORE( WriteLineL( iFormatBuffer8 ) );
		
		remainingLength -= n;
		i += n;
		
		} // end while
		    
    } // END HexDump
	
// ----------------
// CDebugLogTlsData
// ----------------

// ==========================================================================
// METHOD: Instance 
//
// DESIGN:  
// ==========================================================================
CDebugLogTlsData* CDebugLogTlsData::Instance()
    {
    CDebugLogTlsData* tlsData = NULL;
    
	if ( !Dll::Tls() )
    	{
		tlsData = new CDebugLogTlsData;

        if( ( tlsData ) && tlsData->Initialize() )
            {
    		Dll::SetTls( tlsData );
            }
        else
            {
            delete tlsData;
            tlsData = NULL;
            } // end if
    	}
    else
        {
        tlsData = reinterpret_cast<CDebugLogTlsData*>(Dll::Tls());
        } // end if

    return tlsData;
    } // END Instance

// ==========================================================================
// METHOD:  Constructor
//
// DESIGN:  
// ==========================================================================
CDebugLogTlsData::CDebugLogTlsData() : CActive( EPriorityNormal )
    {
   	CActiveScheduler::Add(this);        
    } // END constructor
    
// ==========================================================================
// METHOD:  Destructor
//
// DESIGN:  
// ==========================================================================
CDebugLogTlsData::~CDebugLogTlsData()
    {
    Cancel();    
    iFs.Close();
    iFiles.Close();  
    } // END ~CDebugLogTlsData

// ==========================================================================
// METHOD:  Initialize
//
// DESIGN:  
// ==========================================================================
TBool CDebugLogTlsData::Initialize()
    {
    TInt returnValue = ( iFs.Connect() == KErrNone );
    
    if( returnValue )
        {
        // Request notification of directory adds/deletes in the logs directory.
        iFs.NotifyChange( ENotifyDir, iStatus, KDebugLogsBaseDirectory );        
        SetActive();
        
    	// Dynamically create the name of the log files based on the application name
    	// and thread ID.  This will eliminate multiple processes/thread usage of
    	// this debug logging infrastructure from scribbling each others traces.
	    RProcess thisProcess;
   	    RThread  thisThread;

	    // The file name is the process name followed by the thread ID.
    	TParsePtrC fileNameParser( thisProcess.FileName() );
    	iFileName.Copy( fileNameParser.Name() );
    	iFileName.Append( KUnderscore );
    	iFileName.Append( thisThread.Name() );    	    	
    	iFileName.Append( KDebugLogFileExt );    	       
        
        } // end if
        
    return returnValue;        

    } // END Initialize
        
// ==========================================================================
// METHOD:  FreeIfNecessary
//
// DESIGN:  
// ==========================================================================
void CDebugLogTlsData::FreeIfNecessary()
    {
    CDebugLogTlsData* tlsData;

    tlsData = reinterpret_cast<CDebugLogTlsData*>(Dll::Tls());
    
    if( tlsData->iFiles.Count() == 0 )
        {
        delete tlsData;
        Dll::SetTls( NULL );       
        }
           
    } // END FreeIfNecessary

// ==========================================================================
// METHOD:  RunL
//
// DESIGN:  
// ==========================================================================
void CDebugLogTlsData::RunL()
    {
    if( iStatus == KErrNone )
        {
        // Try to open each file, just in case the modified directory was
        // for one of the log files.
        for( TInt i = 0; i < iFiles.Count(); i++ )
            {
    	    iFiles[i]->TryToOpenFile( ETrue );
            } // end for
	        
        iFs.NotifyChange( ENotifyDir, iStatus, KDebugLogsBaseDirectory );
        SetActive();
        
        } // end if
        
    } // END RunL
        
// ==========================================================================
// METHOD:  DoCancel
//
// DESIGN:  
// ==========================================================================
void CDebugLogTlsData::DoCancel()
    {
    iFs.NotifyChangeCancel();
    
    } // END DoCancel
        
// ==========================================================================
// METHOD:  GetLogFileHandler
//
// DESIGN:  
// ==========================================================================
CLogFileHandler* CDebugLogTlsData::GetLogFileHandler( const TDesC& aDirectory, RDebugLog* aObject )
    {
    CLogFileHandler*  returnValue = NULL;
    
    CDebugLogTlsData* tlsData = CDebugLogTlsData::Instance();

    if( tlsData )
        {            
        returnValue = tlsData->DoGetLogFileHandler( aDirectory, aObject );
        } // end if
        
    return returnValue;
    } // END GetLogFileHandler
    
// ==========================================================================
// METHOD:  ReleaseLogFileHandler
//
// DESIGN:  
// ==========================================================================
void CDebugLogTlsData::ReleaseLogFileHandler( CLogFileHandler* aHandler, RDebugLog* aObject )
    {
	if ( Dll::Tls() )
	    {
        CDebugLogTlsData* tlsData;

        tlsData = reinterpret_cast<CDebugLogTlsData*>(Dll::Tls());

        tlsData->DoReleaseLogFileHandler( aHandler, aObject );        
	    } // end if
	    
    } // END ReleaseLogFileHandler
        
// ==========================================================================
// METHOD:  DoGetLogFileHandler
//
// DESIGN:  
// ==========================================================================
CLogFileHandler* CDebugLogTlsData::DoGetLogFileHandler( const TDesC& aDirectory, RDebugLog* aObject )
    {        
    CLogFileHandler* handler = NULL;
    
    // Look for the log file handler for the given directory.
    TInt index = 0;
    while( index < iFiles.Count() && ( !handler ) )
        {
        CLogFileHandler* currentHandler = iFiles[index];
        if( currentHandler->IsHandler( aDirectory ) )
            {
            handler = currentHandler;
            }
        else
            {                
            index++;
            } // end if
        } // end while
        
    if( !handler )
        {        
        handler = new CLogFileHandler( iFs, iFormatBuffer, iOutputBuffer, aDirectory, iFileName );

        if( handler )
            {            
            iFiles.AppendL( handler );
            } // end if
        }
    
    if( handler )
        {        
        // Handler already exists.  Add object to list of objects.
//#ifdef __MAKE_COPY_OF_NAMES 
        HBufC8* copyOfName = HBufC8::New( aObject->ClassName8().Length() ); 
		if ( copyOfName )
		    {       
            copyOfName->Des().Copy( aObject->ClassName8() );
            handler->iObjectNames.AppendL( copyOfName );
			}
//#endif        

        handler->iObjects.AppendL( aObject );
	    } // end if
	    
	return handler;       
    
    } // END DoGetLogFileHandler
	    
// ==========================================================================
// METHOD:  DoReleaseLogFileHandler
//
// DESIGN:  
// ==========================================================================
void CDebugLogTlsData::DoReleaseLogFileHandler( CLogFileHandler* aHandler, RDebugLog* aObject )
    {
    TInt index = aHandler->iObjects.FindL( aObject );
    
    if( index >= 0 )
        {
//#ifdef __MAKE_COPY_OF_NAMES
        delete aHandler->iObjectNames[index];
        aHandler->iObjectNames.Remove(index);
//#endif        
        aHandler->iObjects.Remove( index );
        
        if( aHandler->iObjects.Count() == 0 )
            {
            iFiles.Remove( iFiles.Find(aHandler) );
            delete aHandler;
            } // end if
        }
    else
        {        
        // Something is wrong.  The most likely case is incorrect usage of the debug log macros.
        _LIT8( KDummyFuncName, "***" );
        _LIT8( KErrorText, "INCORRECT DEBUG LOG USAGE!" );
        
        aObject->Write( KDummyFuncName, KErrorText );        
        }
        
    } // END DoReleaseLogFileHandler

// ==========================================================================
// METHOD:  LogLeakedObjects
//
// DESIGN:  
// ==========================================================================
void CDebugLogTlsData::LogLeakedObjects()
    {    
    if( Dll::Tls() )
        {
        // If the TLS data wasn't cleaned up then there was a leak, either due
        // to incorrect usage of the logger or due to the leak of an object that
        // is using the logger.
        Instance()->DoLogLeakedObjects();
        }
    }

// ==========================================================================
// METHOD:  DoLogLeakedObjects
//
// DESIGN:  
// ==========================================================================
void CDebugLogTlsData::DoLogLeakedObjects()
    {   
    for( TInt i = 0; i < iFiles.Count(); i++ )
        {
        // There are still objects hanging around.  Something leaked, or the log macros
        // were not properly used. 

        _LIT8( KDebugLogClassName, "DebugLogger" );
        _LIT8( KDebugLogFunctionNameName, "DoLogLeakedObjects" );
        _LIT8( KDebugLogErrorText, "LEAKS DETECTED!" );
        const TUint bufSize = 60;
        
        TBuf8<bufSize> tempBuffer;
        
        if( iFiles[i]->ReadyToWrite() )
            {                
            iFiles[i]->Write( KDebugLogClassName, KDebugLogFunctionNameName, KDebugLogErrorText );        

            for( TInt j = 0; j < iFiles[i]->iObjects.Count(); j++ )
                {
                _LIT8( KLeakFmt, "Leak #%i" );
                tempBuffer.Format( KLeakFmt, j+1 );
                iFiles[i]->Write( KDebugLogClassName, KDebugLogFunctionNameName, tempBuffer );
                _LIT8( KAdressFmt, "address=0x%x" );
                tempBuffer.Format( KAdressFmt(), iFiles[i]->iObjects[j]->ObjectAddress() );
                iFiles[i]->Write( KDebugLogClassName, KDebugLogFunctionNameName, tempBuffer );
                
//#ifdef __MAKE_COPY_OF_NAMES                
                TPtrC8 className( iFiles[i]->iObjectNames[j]->Des() );             
//#else
//                TPtrC8 className( iFiles[i]->iObjects[j]->ClassName8() );             
//#endif                
                _LIT8( KClassFmt, "class=%S" );
                tempBuffer.Format(KClassFmt() , &className );
                iFiles[i]->Write( KDebugLogClassName, KDebugLogFunctionNameName, tempBuffer );
                }
            iFiles[i]->CloseFile();            
            }
        }
    }

// ----------------------
// TDebugLogBase8Overflow
// ----------------------

// ==========================================================================
// METHOD:  Overflow
//
// DESIGN:  Handle 8-bit descriptor overflow from the 8-bit formatter.
// ==========================================================================
void TDebugLogBase8Overflow::Overflow( TDes8& /* aDes */ )
    {
	// NOOP; nothing we can do.  The descriptor parameter is the
	// string that has already been printed, not the remainder that
	// hasn't been printed yet.

    } // END Overflow

// -----------------------
// TDebugLogBase16Overflow
// -----------------------

// ==========================================================================
// METHOD:  Overflow
//
// DESIGN:  Handle 16-bit descriptor overflow from the 16-bit formatter.
// ==========================================================================
void TDebugLogBase16Overflow::Overflow( TDes16& /* aDes */ )
    {
	// NOOP; nothing we can do.  The descriptor parameter is the
	// string that has already been printed, not the remainder that
	// hasn't been printed yet.

    } // END Overflow

// END FILE DebugLog.cpp

