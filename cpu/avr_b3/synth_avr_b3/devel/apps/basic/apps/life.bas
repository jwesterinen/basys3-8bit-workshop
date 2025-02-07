100 gr : clear
110 cleardb
130 rem gosub 1000
140 gosub 2000
145 rem gosub 3000
150 rem
160 for r=1 to 38
170 for c=1 to 78
180 if buttons() <> 0 goto 5000
190 nqty =        (getchar(r-1,c-1)=04)
200 nqty = nqty + (getchar(r-1,c  )=04)
210 nqty = nqty + (getchar(r-1,c+1)=04)
220 nqty = nqty + (getchar(r  ,c-1)=04)
230 nqty = nqty + (getchar(r  ,c+1)=04)
240 nqty = nqty + (getchar(r+1,c-1)=04)
250 nqty = nqty + (getchar(r+1,c  )=04)
260 nqty = nqty + (getchar(r+1,c+1)=04)
270 if (nqty=3) OR ((getchar(r,c)=0x04) AND (nqty=2)) then goto 290
280 putdb r,c,0x20 : goto 300
290 putdb r,c,0x04
300 next c
310 next r
320 loadfb
330 goto 150
1000 for i=0 to 1000
1100 putchar rnd(39),rnd(79),0x04
1200 next i
1300 return
2000 putchar 1,2,0x04
2100 putchar 2,3,0x04
2200 putchar 3,1,0x04 : putchar 3,2,0x04 : putchar 3,3,0x04
2300 putchar 10,21,0x04
2400 putchar 11,22,0x04
2500 putchar 12,20,0x04 : putchar 12,21,0x04 : putchar 12,22,0x04
2600 return
3000 putchar 2,1,0x04 : putchar 2,2,0x04 : putchar 2,3,0x04
3100 return
5000 text : clear
