rem Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem T_IMCM_PREPARE_FOR_TEST.BAT
rem 
rem

REM NOTE! Do not use tabs here!
md c:\MsgTest
md c:\MsgTest\imut
md c:\MsgTest\imut\imcv
md c:\MsgTest\imut\rfc822
REM md c:\MsgTest\imut\imcv\parse\A.Simple 
REM md c:\MsgTest\imut\imcv\parse\B.Mime
REM md c:\MsgTest\imut\imcv\parse\C.Multipart
REM md c:\MsgTest\imut\imcv\parse\D.Encoding
REM md c:\MsgTest\imut\imcv\parse\E.Charsets
REM md c:\MsgTest\imut\imcv\parse\F.Header
REM md c:\MsgTest\imut\imcv\parse\G.Mhtml
REM md c:\MsgTest\imut\imcv\parse\H.Embedded
REM md c:\MsgTest\imut\rfc822\parse\A.Simple 
REM md c:\MsgTest\imut\rfc822\parse\B.Mime
REM md c:\MsgTest\imut\rfc822\parse\C.Multipart
REM md c:\MsgTest\imut\rfc822\parse\D.Encoding
REM md c:\MsgTest\imut\rfc822\parse\E.Charsets
REM md c:\MsgTest\imut\rfc822\parse\F.Header
REM md c:\MsgTest\imut\rfc822\parse\G.Mhtml
REM md c:\MsgTest\imut\rfc822\parse\H.Embedded
md c:\MsgTest\imut\imcv\transform_receive
md c:\MsgTest\imut\imcv\transform_send
md c:\MsgTest\imut\rfc822\transform_receive
md c:\MsgTest\imut\rfc822\transform_send

REM IMSK script files
copy z:\System\MsgTest\msgtest\imut\scriptwithSSLTLS.scr c:\MsgTest\imut\scriptwithSSLTLS.scr
copy z:\System\MsgTest\msgtest\imut\scriptwithoutSSLTLS.scr c:\MsgTest\imut\scriptwithoutSSLTLS.scr
attrib c:\MsgTest\imut\*.* -r

REM  IMUT_TRAMSFORM_SEND
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_send\rfc82201.jig c:\msgtest\IMUT\rfc822\transform_send\rfc82201.jig
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_send\rfc82201.txt c:\msgtest\IMUT\rfc822\transform_send\rfc82201.txt
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_send\rfc82202.txt c:\msgtest\IMUT\rfc822\transform_send\rfc82202.txt
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_send\rfc82203.txt c:\msgtest\IMUT\rfc822\transform_send\rfc82203.txt
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_send\rfc82204.txt c:\msgtest\IMUT\rfc822\transform_send\rfc82204.txt
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_send\rfc82205.txt c:\msgtest\IMUT\rfc822\transform_send\rfc82205.txt
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_send\rfc82206.txt c:\msgtest\IMUT\rfc822\transform_send\rfc82206.txt
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_send\rfc82207_single_jp_subject.txt c:\msgtest\IMUT\rfc822\transform_send\rfc82207_single_jp_subject.txt
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_send\rfc82208_long_jp_subject.txt c:\msgtest\IMUT\rfc822\transform_send\rfc82208_long_jp_subject.txt

attrib c:\MsgTest\imut\rfc822\transform_send\*.* -r

REM IMUT_TRANSFORM_SEND
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\ASCII_old_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\ASCII_old_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\ASCII_SDC_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\ASCII_SDC_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\B64_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\B64_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\Charset_ASCII_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\Charset_ASCII_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\Charset_Big5_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\Charset_Big5_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\Charset_JP_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\Charset_JP_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\Charset_UTF7_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\Charset_UTF7_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\Charset_UTF8_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\Charset_UTF8_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\Def_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\Def_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\None_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\None_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\QP_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\QP_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\Service_B64_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\Service_B64_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\Service_Def_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\Service_Def_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\Service_QP_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\Service_QP_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\Service_UTF7_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\Service_UTF7_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\UTF7_SDC_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\UTF7_SDC_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\UTF8_old_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\UTF8_old_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\UU_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\UU_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\Wrong_SDC_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\Wrong_SDC_Info_ImcvSendOutputText.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_send\CTE_7Bit_Info_ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\transform_send\CTE_7Bit_Info_ImcvSendOutputText.txt
attrib c:\MsgTest\imut\imcv\transform_send\*.* -r

REM IMUT_TRANSFORM_RECEIVE
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_receive\Rfc_01_NONMIME_UTF7.txt c:\msgtest\IMUT\rfc822\transform_receive\Rfc_01_NONMIME_UTF7.txt
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_receive\Rfc_02_NONMIME_ASCII.txt c:\msgtest\IMUT\rfc822\transform_receive\Rfc_02_NONMIME_ASCII.txt
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_receive\Rfc_03_NONMIME_ATTACH_UTF7.txt c:\msgtest\IMUT\rfc822\transform_receive\Rfc_03_NONMIME_ATTACH_UTF7.txt
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_receive\Rfc_04_MIME_UTF7.txt c:\msgtest\IMUT\rfc822\transform_receive\Rfc_04_MIME_UTF7.txt
copy z:\System\MsgTest\msgtest\imut\rfc822\transform_receive\Rfc_05_MIME_Text_PARTS.txt c:\msgtest\IMUT\rfc822\transform_receive\Rfc_05_MIME_Text_PARTS.txt
attrib c:\MsgTest\imut\rfc822\transform_receive\*.* -r

copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\ASCIIEntry_822Headers.txt c:\msgtest\IMUT\imcv\transform_receive\ASCIIEntry_822Headers.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\ASCIIEntry_Files.txt c:\msgtest\IMUT\imcv\transform_receive\ASCIIEntry_Files.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\ASCIIEntry_MimeHeaders.txt c:\msgtest\IMUT\imcv\transform_receive\ASCIIEntry_MimeHeaders.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\ASCIIEntry_RichTextBodies.txt c:\msgtest\IMUT\imcv\transform_receive\ASCIIEntry_RichTextBodies.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\ASCIIEntry_Structure.txt c:\msgtest\IMUT\imcv\transform_receive\ASCIIEntry_Structure.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\NoneEntry_822Headers.txt c:\msgtest\IMUT\imcv\transform_receive\NoneEntry_822Headers.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\NoneEntry_Files.txt c:\msgtest\IMUT\imcv\transform_receive\NoneEntry_Files.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\NoneEntry_MimeHeaders.txt c:\msgtest\IMUT\imcv\transform_receive\NoneEntry_MimeHeaders.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\NoneEntry_RichTextBodies.txt c:\msgtest\IMUT\imcv\transform_receive\NoneEntry_RichTextBodies.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\NoneEntry_Structure.txt c:\msgtest\IMUT\imcv\transform_receive\NoneEntry_Structure.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\UnknownEntry_822Headers.txt c:\msgtest\IMUT\imcv\transform_receive\UnknownEntry_822Headers.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\UnknownEntry_Files.txt c:\msgtest\IMUT\imcv\transform_receive\UnknownEntry_Files.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\UnknownEntry_MimeHeaders.txt c:\msgtest\IMUT\imcv\transform_receive\UnknownEntry_MimeHeaders.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\UnknownEntry_RichTextBodies.txt c:\msgtest\IMUT\imcv\transform_receive\UnknownEntry_RichTextBodies.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\UnknownEntry_Structure.txt c:\msgtest\IMUT\imcv\transform_receive\UnknownEntry_Structure.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\UTF7Entry_822Headers.txt c:\msgtest\IMUT\imcv\transform_receive\UTF7Entry_822Headers.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\UTF7Entry_Files.txt c:\msgtest\IMUT\imcv\transform_receive\UTF7Entry_Files.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\UTF7Entry_MimeHeaders.txt c:\msgtest\IMUT\imcv\transform_receive\UTF7Entry_MimeHeaders.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\UTF7Entry_RichTextBodies.txt c:\msgtest\IMUT\imcv\transform_receive\UTF7Entry_RichTextBodies.txt
copy z:\System\MsgTest\msgtest\imut\imcv\transform_receive\UTF7Entry_Structure.txt c:\msgtest\IMUT\imcv\transform_receive\UTF7Entry_Structure.txt
attrib c:\MsgTest\imut\imcv\transform_receive\*.* -r

REM IMCV PARSE
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\A.Simple\Entry_822Headers.txt c:\msgtest\IMUT\imcv\parse\A.Simple\Entry_822Headers.txt          
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\A.Simple\Entry_Files.txt c:\msgtest\IMUT\imcv\parse\A.Simple\Entry_Files.txt               
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\A.Simple\Entry_MimeHeaders.txt c:\msgtest\IMUT\imcv\parse\A.Simple\Entry_MimeHeaders.txt         
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\A.Simple\Entry_RichTextBodies.txt c:\msgtest\IMUT\imcv\parse\A.Simple\Entry_RichTextBodies.txt      
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\A.Simple\Entry_Structure.txt c:\msgtest\IMUT\imcv\parse\A.Simple\Entry_Structure.txt           
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\A.Simple\ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\parse\A.Simple\ImcvSendOutputText.txt        
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\A.Simple\ImcvSendOutputText_MIME.txt c:\msgtest\IMUT\imcv\parse\A.Simple\ImcvSendOutputText_MIME.txt   
REM attrib c:\MsgTest\imut\imcv\parse\A.Simple\*.*	-r

REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\B.Mime\Entry_822Headers.txt c:\msgtest\IMUT\imcv\parse\B.Mime\Entry_822Headers.txt       
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\B.Mime\Entry_Files.txt c:\msgtest\IMUT\imcv\parse\B.Mime\Entry_Files.txt            
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\B.Mime\Entry_MimeHeaders.txt c:\msgtest\IMUT\imcv\parse\B.Mime\Entry_MimeHeaders.txt      
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\B.Mime\Entry_RichTextBodies.txt c:\msgtest\IMUT\imcv\parse\B.Mime\Entry_RichTextBodies.txt   
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\B.Mime\Entry_Structure.txt c:\msgtest\IMUT\imcv\parse\B.Mime\Entry_Structure.txt        
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\B.Mime\ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\parse\B.Mime\ImcvSendOutputText.txt     
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\B.Mime\ImcvSendOutputText_MIME.txt c:\msgtest\IMUT\imcv\parse\B.Mime\ImcvSendOutputText_MIME.txt
REM attrib c:\MsgTest\imut\imcv\parse\B.Mime\*.* -r

REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\C.Multipart\Entry_822Headers.txt c:\msgtest\IMUT\imcv\parse\C.Multipart\Entry_822Headers.txt       
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\C.Multipart\Entry_Files.txt c:\msgtest\IMUT\imcv\parse\C.Multipart\Entry_Files.txt            
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\C.Multipart\Entry_MimeHeaders.txt c:\msgtest\IMUT\imcv\parse\C.Multipart\Entry_MimeHeaders.txt      
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\C.Multipart\Entry_RichTextBodies.txt c:\msgtest\IMUT\imcv\parse\C.Multipart\Entry_RichTextBodies.txt   
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\C.Multipart\Entry_Structure.txt c:\msgtest\IMUT\imcv\parse\C.Multipart\Entry_Structure.txt        
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\C.Multipart\ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\parse\C.Multipart\ImcvSendOutputText.txt     
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\C.Multipart\ImcvSendOutputText_MIME.txt c:\msgtest\IMUT\imcv\parse\C.Multipart\ImcvSendOutputText_MIME.txt
REM attrib c:\MsgTest\imut\imcv\parse\C.Multipart\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\D.Encoding\Entry_822Headers.txt c:\msgtest\IMUT\imcv\parse\D.Encoding\Entry_822Headers.txt       
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\D.Encoding\Entry_Files.txt c:\msgtest\IMUT\imcv\parse\D.Encoding\Entry_Files.txt            
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\D.Encoding\Entry_MimeHeaders.txt c:\msgtest\IMUT\imcv\parse\D.Encoding\Entry_MimeHeaders.txt      
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\D.Encoding\Entry_RichTextBodies.txt c:\msgtest\IMUT\imcv\parse\D.Encoding\Entry_RichTextBodies.txt   
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\D.Encoding\Entry_Structure.txt c:\msgtest\IMUT\imcv\parse\D.Encoding\Entry_Structure.txt        
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\D.Encoding\ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\parse\D.Encoding\ImcvSendOutputText.txt     
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\D.Encoding\ImcvSendOutputText_MIME.txt c:\msgtest\IMUT\imcv\parse\D.Encoding\ImcvSendOutputText_MIME.txt
REM attrib c:\MsgTest\imut\imcv\parse\D.Encoding\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\E.Charsets\Entry_822Headers.txt c:\msgtest\IMUT\imcv\parse\E.Charsets\Entry_822Headers.txt        
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\E.Charsets\Entry_Files.txt c:\msgtest\IMUT\imcv\parse\E.Charsets\Entry_Files.txt             
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\E.Charsets\Entry_MimeHeaders.txt c:\msgtest\IMUT\imcv\parse\E.Charsets\Entry_MimeHeaders.txt       
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\E.Charsets\Entry_RichTextBodies.txt c:\msgtest\IMUT\imcv\parse\E.Charsets\Entry_RichTextBodies.txt    
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\E.Charsets\Entry_Structure.txt c:\msgtest\IMUT\imcv\parse\E.Charsets\Entry_Structure.txt         
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\E.Charsets\ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\parse\E.Charsets\ImcvSendOutputText.txt      
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\E.Charsets\ImcvSendOutputText_MIME.txt c:\msgtest\IMUT\imcv\parse\E.Charsets\ImcvSendOutputText_MIME.txt 
REM attrib c:\MsgTest\imut\imcv\parse\E.Charsets\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\F.Header\Entry_822Headers.txt c:\msgtest\IMUT\imcv\parse\F.Header\Entry_822Headers.txt        
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\F.Header\Entry_Files.txt c:\msgtest\IMUT\imcv\parse\F.Header\Entry_Files.txt             
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\F.Header\Entry_MimeHeaders.txt c:\msgtest\IMUT\imcv\parse\F.Header\Entry_MimeHeaders.txt       
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\F.Header\Entry_RichTextBodies.txt c:\msgtest\IMUT\imcv\parse\F.Header\Entry_RichTextBodies.txt    
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\F.Header\Entry_Structure.txt c:\msgtest\IMUT\imcv\parse\F.Header\Entry_Structure.txt         
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\F.Header\ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\parse\F.Header\ImcvSendOutputText.txt      
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\F.Header\ImcvSendOutputText_MIME.txt c:\msgtest\IMUT\imcv\parse\F.Header\ImcvSendOutputText_MIME.txt 
REM attrib c:\MsgTest\imut\imcv\parse\F.Header\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\G.Mhtml\Entry_822Headers.txt c:\msgtest\IMUT\imcv\parse\G.Mhtml\Entry_822Headers.txt        
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\G.Mhtml\Entry_Files.txt c:\msgtest\IMUT\imcv\parse\G.Mhtml\Entry_Files.txt             
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\G.Mhtml\Entry_MimeHeaders.txt c:\msgtest\IMUT\imcv\parse\G.Mhtml\Entry_MimeHeaders.txt       
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\G.Mhtml\Entry_RichTextBodies.txt c:\msgtest\IMUT\imcv\parse\G.Mhtml\Entry_RichTextBodies.txt    
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\G.Mhtml\Entry_Structure.txt c:\msgtest\IMUT\imcv\parse\G.Mhtml\Entry_Structure.txt         
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\G.Mhtml\ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\parse\G.Mhtml\ImcvSendOutputText.txt      
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\G.Mhtml\ImcvSendOutputText_MIME.txt c:\msgtest\IMUT\imcv\parse\G.Mhtml\ImcvSendOutputText_MIME.txt 
REM attrib c:\MsgTest\imut\imcv\parse\G.Mhtml\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\H.Embedded\Entry_822Headers.txt c:\msgtest\IMUT\imcv\parse\H.Embedded\Entry_822Headers.txt          
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\H.Embedded\Entry_Files.txt c:\msgtest\IMUT\imcv\parse\H.Embedded\Entry_Files.txt               
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\H.Embedded\Entry_MimeHeaders.txt c:\msgtest\IMUT\imcv\parse\H.Embedded\Entry_MimeHeaders.txt         
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\H.Embedded\Entry_RichTextBodies.txt c:\msgtest\IMUT\imcv\parse\H.Embedded\Entry_RichTextBodies.txt      
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\H.Embedded\Entry_Structure.txt c:\msgtest\IMUT\imcv\parse\H.Embedded\Entry_Structure.txt           
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\H.Embedded\ImcvSendOutputText.txt c:\msgtest\IMUT\imcv\parse\H.Embedded\ImcvSendOutputText.txt        
REM copy z:\System\MsgTest\msgtest\imut\imcv\parse\H.Embedded\ImcvSendOutputText_MIME.txt c:\msgtest\IMUT\imcv\parse\H.Embedded\ImcvSendOutputText_MIME.txt   
REM attrib c:\MsgTest\imut\imcv\parse\H.Embedded\*.* -r
 
REM REM RFC822 PARSE
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\A.Simple\rfc_0002.txt c:\msgtest\IMUT\rfc822\parse\A.Simple\rfc_0002.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\A.Simple\rfc_0006.txt c:\msgtest\IMUT\rfc822\parse\A.Simple\rfc_0006.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\A.Simple\rfc_0007.txt c:\msgtest\IMUT\rfc822\parse\A.Simple\rfc_0007.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\A.Simple\rfc_0008.txt c:\msgtest\IMUT\rfc822\parse\A.Simple\rfc_0008.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\A.Simple\rfc_0009.txt c:\msgtest\IMUT\rfc822\parse\A.Simple\rfc_0009.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\A.Simple\rfc_0010.txt c:\msgtest\IMUT\rfc822\parse\A.Simple\rfc_0010.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\A.Simple\rfc_0011.txt c:\msgtest\IMUT\rfc822\parse\A.Simple\rfc_0011.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\A.Simple\rfc_0061.txt c:\msgtest\IMUT\rfc822\parse\A.Simple\rfc_0061.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\A.Simple\rfc_0070.txt c:\msgtest\IMUT\rfc822\parse\A.Simple\rfc_0070.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\A.Simple\rfc_0072.txt c:\msgtest\IMUT\rfc822\parse\A.Simple\rfc_0072.txt
REM attrib c:\MsgTest\imut\rfc822\parse\A.Simple\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0001.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0001.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0003.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0003.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0004.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0004.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0005.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0005.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0012.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0012.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0013.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0013.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0014.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0014.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0015.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0015.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0016.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0016.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0017.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0017.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0018.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0018.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0019.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0019.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0020.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0020.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0021.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0021.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0022.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0022.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\B.Mime\rfc_0088.txt c:\msgtest\IMUT\rfc822\parse\B.Mime\rfc_0088.txt
REM attrib c:\MsgTest\imut\rfc822\parse\B.Mime\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0023.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0023.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0024.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0024.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0025.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0025.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0026.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0026.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0027.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0027.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0028.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0028.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0029.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0029.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0037.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0037.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0038.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0038.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0039.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0039.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0040.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0040.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0041.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0041.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0042.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0042.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0043.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0043.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0052.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0052.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0053.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0053.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0054.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0054.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0055.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0055.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0056.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0056.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0057.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0057.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0058.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0058.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0059.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0059.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0060.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0060.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0062.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0062.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0063.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0063.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0064.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0064.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0065.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0065.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0066.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0066.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0067.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0067.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0068.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0068.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\C.Multipart\rfc_0071.txt c:\msgtest\IMUT\rfc822\parse\C.Multipart\rfc_0071.txt 
REM attrib c:\MsgTest\imut\rfc822\parse\C.Multipart\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0030.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0030.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0031.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0031.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0032.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0032.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0033.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0033.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0034.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0034.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0035.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0035.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0036.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0036.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0044.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0044.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0045.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0045.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0046.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0046.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0047.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0047.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0048.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0048.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0049.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0049.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0050.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0050.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0051.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0051.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0077.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0077.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0078.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0078.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0085.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0085.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_0101.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_0101.txt
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\D.Encoding\rfc_inline_Base64.txt c:\msgtest\IMUT\rfc822\parse\D.Encoding\rfc_inline_Base64.txt
REM attrib c:\MsgTest\imut\rfc822\parse\D.Encoding\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\E.Charsets\rfc_0098.txt c:\msgtest\IMUT\rfc822\parse\E.Charsets\rfc_0098.txt        
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\E.Charsets\rfc_0099.txt c:\msgtest\IMUT\rfc822\parse\E.Charsets\rfc_0099.txt        
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\E.Charsets\rfc_2022_ex1.txt c:\msgtest\IMUT\rfc822\parse\E.Charsets\rfc_2022_ex1.txt    
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\E.Charsets\rfc_amazon_co_jp.txt c:\msgtest\IMUT\rfc822\parse\E.Charsets\rfc_amazon_co_jp.txt
REM attrib c:\MsgTest\imut\rfc822\parse\E.Charsets\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0073.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0073.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0074.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0074.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0075.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0075.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0076.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0076.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0079.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0079.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0080.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0080.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0081.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0081.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0083.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0083.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0084.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0084.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0092.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0092.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0093.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0093.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0094.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0094.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0095.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0095.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0096.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0096.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0097.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0097.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\F.Header\rfc_0100.txt c:\msgtest\IMUT\rfc822\parse\F.Header\rfc_0100.txt 
REM attrib c:\MsgTest\imut\rfc822\parse\F.Header\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0069.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0069.txt                 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0082.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0082.txt                 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0086.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0086.txt                 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0087.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0087.txt                 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0089.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0089.txt                 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0090.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0090.txt                 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0091.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0091.txt                 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0102_digest.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0102_digest.txt          
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0103_reverse.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0103_reverse.txt         
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0104_rfc822.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0104_rfc822.txt          
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\RFC_0105_encoded_filename.TXT c:\msgtest\IMUT\rfc822\parse\G.Mhtml\RFC_0105_encoded_filename.TXT
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0105_japanese.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0105_japanese.txt        
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0106_japanese.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0106_japanese.txt        
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\G.Mhtml\rfc_0500.txt c:\msgtest\IMUT\rfc822\parse\G.Mhtml\rfc_0500.txt                 
REM attrib c:\MsgTest\imut\rfc822\parse\G.Mhtml\*.* -r
 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\H.Embedded\RFC_0105_encoded_filename.TXT c:\msgtest\IMUT\rfc822\parse\H.Embedded\RFC_0105_encoded_filename.TXT                              
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\H.Embedded\rfc_106_mx5-d_Digest_V99.txt c:\msgtest\IMUT\rfc822\parse\H.Embedded\rfc_106_mx5-d_Digest_V99.txt                               
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\H.Embedded\rfc_107_Really_mad_nested_message.txt c:\msgtest\IMUT\rfc822\parse\H. Embedded\rfc_107_Really_mad_nested_message.txt                      
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\H.Embedded\rfc_108_Chicken_Oriental_pathologically_nested_message.txt c:\msgtest\IMUT\rfc822\parse\H.Embedded\rfc_108_Chicken_Oriental_pathologically_nested_message.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\H.Embedded\rfc_109_Chicken_Oriental_pathologically_nested_message.txt c:\msgtest\IMUT\rfc822\parse\H.Embedded\rfc_109_Chicken_Oriental_pathologically_nested_message.txt 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\H.Embedded\rfc_110_inline_plus_attached_gifs.txt c:\msgtest\IMUT\rfc822\parse\H.Embedded\rfc_110_inline_plus_attached_gifs.txt                      
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\H.Embedded\rfc_111_empty_embedded.txt c:\msgtest\IMUT\rfc822\parse\H.Embedded\rfc_111_empty_embedded.txt                                 
REM copy z:\System\MsgTest\msgtest\imut\rfc822\parse\H.Embedded\rfc_112_multipart_report.txt c:\msgtest\IMUT\rfc822\parse\H.Embedded\rfc_112_multipart_report.txt                               
REM attrib c:\MsgTest\imut\rfc822\parse\H.Embedded\*.* -r
