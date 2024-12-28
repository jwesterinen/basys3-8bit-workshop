100 gr : clear
110 dim head(2)
120 dim tail(4,2)
130 dim nextpos(2)
131 rem
200 if buttons() <> 0 goto 1000
210 rem
300 head(0) = 0 : head(1) = 0
310 for i=0 to 3
320 tail(i,0) = 0 : tail(i,1) = 0
330 next i
331 rem
400 putchar head(0),head(1),0x04
410 for j=0 to 3
420 putchar tail(j,0),tail(j,1),0x04
430 next j
431 rem
500 putchar head(0),head(1),0x20
510 for k=0 to 3
520 putchar tail(k,0),tail(k,1),0x20
530 next k
531 rem
600 nextpos(0)=head(0) : nextpos(1)=head(1)
610 dir = rnd(4)
620 if dir = 0 goto 700   rem south
630 if dir = 1 goto 720   rem east
640 if dir = 2 goto 740   rem north
650 if dir = 3 goto 760   rem west
651 rem
700 if head(0) >= 39 goto 200
710 nextpos(0) = nextpos(0) + 1 : goto 800
720 if head() >= 79 goto 200
730 nextpos(1) = nextpos(1) + 1 : goto 800
740 if head(0) <= 0 goto 200
750 nextpos(0) = nextpos(0) - 1 : goto 800
760 if head() <= 0 goto 200
770 nextpos(1) = nextpos(1) - 1 : goto 800
771 rem
800 tail(3,0) = tail(2,0) : tail(3,1) = tail(2,1)
810 tail(2,0) = tail(1,0) : tail(2,1) = tail(1,1)
820 tail(1,0) = tail(0,0) : tail(1,1) = tail(0,1)
830 tail(0,0) = head(0) : tail(0,1) = head(1)
840 head(0) = nextpos(0) : head(1) = nextpos(1)
850 goto 200
851 rem
1000 text : clear

