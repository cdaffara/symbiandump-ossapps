T_SmsSendRecv uses a script file called sendrecv.script.
That file describes all the messages that needs to be sent in order to complete the
test harness. But there is no phone that is able to handle all of those messages 
therefor additional files have been added, Nokia8210.txt and EricssonT39.txt.
To run all test cases a Nokia 8210 over IR and a Ericsson T300 with serial cable should
 be used.
First rename Nokia8210.txt in <drive>:\msgtest\sms to sendrecv.script.
Run the test harness. If successful move to next step.
Rename EricssonT300.txt in <drive>:\msgtest\sms to sendrecv.script.
Run the test harness. If successful the complete test is completed.
