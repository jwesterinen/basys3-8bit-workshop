10 gr : clear
20 RSIZE=20 : CSIZE=30
100 DIM CB(RSIZE,CSIZE) : DIM NB(RSIZE,CSIZE)
101 rem seed the buffer
102 rem CB(2,1)=1 : CB(2,2)=1 : CB(2,3)=1
103 gosub 500
106 rem exit if a button is pressed
110 if buttons() <> 0 goto 1000
115 FOR R=1 TO RSIZE-2
120 FOR C=1 TO CSIZE-2
130 TT=CB(R-1,C-1)+CB(R-1,C)+CB(R-1,C+1)+CB(R,C-1)
140 TT=TT+CB(R,C+1)+CB(R+1,C-1)+CB(R+1,C)+CB(R+1,C+1)
150 IF TT=3 OR CB(R,C)=1 AND TT=2 THEN GOTO 170
155 if buttons() <> 0 goto 1000
160 NB(R,C)=0 : putchar R,C,0x20 : GOTO 180
170 NB(R,C)=1 : putchar R,C,0x04
180 NEXT C
190 NEXT R
200 FOR RR=1 TO RSIZE-2
210 FOR CC=1 TO CSIZE-2
220 CB(RR,CC) = NB(RR,CC)
230 NEXT CC
240 NEXT RR
250 GOTO 110
500 for i=0 to (RSIZE*CSIZE)/10
510 CB(rnd(RSIZE-2),rnd(CSIZE-2))=1
520 next i
530 return
1000 text : clear
