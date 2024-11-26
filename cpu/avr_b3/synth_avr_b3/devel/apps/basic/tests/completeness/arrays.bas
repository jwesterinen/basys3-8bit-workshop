100 R = 40
110 C = 80
120 dim a(R,C)
130 i = 0
140 for r = 0 to R-1
150 for c = 0 to C-1
160 a(r,c) = i : i=i+1
170 print "a(";r;",";c;") = ";a(r,c)
180 next c
190 next r

