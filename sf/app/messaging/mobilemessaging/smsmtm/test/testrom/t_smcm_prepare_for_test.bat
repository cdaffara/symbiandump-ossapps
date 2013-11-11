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
rem

REM NOTE! Do not use tabs here!
md c:\sms
md c:\MsgTest
md c:\MsgTest\sms

copy z:\System\MsgTest\sms\SimParameter.script c:\sms\SimParameter.script
copy z:\System\MsgTest\sms\smssc_config.txt c:\sms\smssc_config.txt
attrib c:\sms\*.* -r

copy z:\System\MsgTest\msgtest\sms\sms.script c:\msgtest\sms\sms.script
copy z:\System\MsgTest\msgtest\sms\smsenum.script c:\msgtest\sms\smsenum.script
copy z:\System\MsgTest\msgtest\sms\\wap.script c:\msgtest\sms\wap.script
copy z:\System\MsgTest\msgtest\sms\\nbs.script c:\msgtest\sms\nbs.script
copy z:\System\MsgTest\msgtest\sms\\biofication.script c:\msgtest\sms\biofication.script
copy z:\System\MsgTest\msgtest\sms\\smcmsimtsytest_config.txt c:\msgtest\sms\smcmsimtsytest_config.txt
copy z:\System\MsgTest\msgtest\sms\\sendrecv.script c:\msgtest\sms\sendrecv.script
copy z:\System\MsgTest\msgtest\sms\\Nokia8210.txt c:\msgtest\sms\Nokia8210.txt
copy z:\System\MsgTest\msgtest\sms\\EricssonT300.txt c:\msgtest\sms\EricssonT300.txt
copy z:\System\MsgTest\msgtest\sms\\logo1.olp c:\msgtest\sms\logo1.olp
copy z:\System\MsgTest\msgtest\sms\\tone1.rtp c:\msgtest\sms\tone1.rtp
copy z:\System\MsgTest\msgtest\sms\\tone2.rtp c:\msgtest\sms\tone2.rtp
copy z:\System\MsgTest\msgtest\sms\\tone3.rtp c:\msgtest\sms\tone3.rtp
copy z:\System\MsgTest\msgtest\sms\\t_smssendrecv.html c:\msgtest\sms\t_smssendrecv.html
attrib c:\msgtest\sms\*.* -r




