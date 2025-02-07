100 gr : clear
200 cleardb
300 for i=0 to 19
400 putdb rnd(39),rnd(79),04
500
600 next i
700 loadfb
750 if buttons() <> 0 goto 1000
800 delay 500
900 goto 200
1000 text : clear
