100 dim array1(2,3,4) : dim array2(2,3)
110 a=1 : ?a
120 b=5 : a=b : ?a
130 array1(0,0,0) = 123 : ?array1(0,0,0)
140 a = array1(0,0,0) : ?a
150 a = peek(0xf000) : ?a
160 array2(0,0) = b : ?array2(0,0)
170 array1(0,0,0) = array2(0,0) : ?array1(0,0,0)
180 array2(1,2) = peek(0xf001) : ?array2(1,2)
190 rem
200 dim array1$(2,3,4) : dim array2$(2,3)
210 a$="foo" : ?a$
220 b$="bar" : a$=b$ : ?a$
230 array1$(0,0,0) = "baz" : ?array1$(0,0,0)
240 a$ = array1$(0,0,0) : ?a$
250 array2$(0,0) = b$ : ?array2$(0,0)
260 array1$(0,0,0) = array2$(0,0) : ?array1$(0,0,0)

