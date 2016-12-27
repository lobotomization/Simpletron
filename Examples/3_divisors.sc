10 rem This calculates the divisors of x 
20 input x 
30 let y = 1 
40 let z = x/y 
50 let r = (x-z*y) 
60 if r > 0 goto 80 
70 if r == 0 goto 110 
80 let y = y+1 
90 if y > x goto 150 
100 goto 40 
110 print y 
120 let y = y+1 
130 if y > x goto 150 
140 goto 40
150 end