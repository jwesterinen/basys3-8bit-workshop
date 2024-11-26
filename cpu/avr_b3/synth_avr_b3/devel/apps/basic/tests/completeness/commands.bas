10 dummy = 123.456 : ?dummy
15 poke 0xf004,0x45
20 tone 440
21 delay 500
25 tone 0
30 tone 880,500
31 delay 500
35 beep
40 display 0x1234, 4
45 outchar 0x30
50 rseed 123
56 if 1 then goto 60 : ?0xbad0
60 for i = 0 to 5
65 for j = 2 to 8 step 2
70 gosub 100
75 next
80 next i
85 end
100 if i = j then goto 130
110 print "{";i;",";j;"}"
120 goto 140
130 gosub 200 : ?"return to correct point"
140 return
200 if i = j then print "i=j!"
210 if j / 2 = 1 or j / 4 = 2 then goto 230
220 goto 290
230 print "input a number:"
240 input a
250 print "the number is: ";a
260 print "input a string:"
270 input a$
280 print "the string is: ";a$
290 return

