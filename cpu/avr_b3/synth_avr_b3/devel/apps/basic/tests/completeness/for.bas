100 dim a(2,3)
110 i=0
120 for r=0 to 1
130 for c=0 to 2
140 a(r,c) = i : i=i+1
150 next
160 next r
170 for r=1 to 0 step -1
180 for c=2 to 0 step -1
190 print "a(";r;",";c;")=";a(r,c)
200 next
210 next r
220 input n
230 goto 110

