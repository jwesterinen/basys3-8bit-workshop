100 gr : clear
110 cleardb 0 : cleardb 1
120 db=0
130 rem gosub 1000
140 gosub 2000
145 rem gosub 3000
150 loadfb db
160 for r=1 to 38
170 for c=1 to 78
180 if buttons() <> 0 goto 5000
190 nqty =        (getdb(db,r-1,c-1)=04)
200 nqty = nqty + (getdb(db,r-1,c  )=04)
210 nqty = nqty + (getdb(db,r-1,c+1)=04)
220 nqty = nqty + (getdb(db,r  ,c-1)=04)
230 nqty = nqty + (getdb(db,r  ,c+1)=04)
240 nqty = nqty + (getdb(db,r+1,c-1)=04)
250 nqty = nqty + (getdb(db,r+1,c  )=04)
260 nqty = nqty + (getdb(db,r+1,c+1)=04)
270 if (nqty=3) OR (getdb(db,r,c)=0x04) AND (nqty=2) then goto 290
280 putdb (not db),r,c,0x20 : goto 300
290 putdb (not db),r,c,0x04
300 next c
310 next r
320 db=not db
330 goto 150
1000 for i=0 to 1000
1100 putdb db,rnd(39),rnd(79),0x04
1200 next i
1300 return
2000 putdb db,1,2,0x04
2100 putdb db,2,3,0x04
2200 putdb db,3,1,0x04 : putdb db,3,2,0x04 : putdb db,3,3,0x04
2300 putdb db,10,21,0x04
2400 putdb db,11,22,0x04
2500 putdb db,12,20,0x04 : putdb db,12,21,0x04 : putdb db,12,22,0x04
2600 return
3000 putdb db,2,1,0x04 : putdb db,2,2,0x04 : putdb db,2,3,0x04
3100 return
5000 text : clear
