10 rem This calculates the primes up to 75 
20 let p = 0 
25 let d = 0 
26 let p = p+1 
30 let y = 1 
40 let q = p/y 
50 let r = (p-q*y) 
60 if r > 0 goto 80 
70 if r == 0 goto 110 
80 let y = y+1 
90 if y > p goto 150 
100 goto 40 
110 let d = d+1 
120 let y = y+1 
130 if y > p goto 150 
140 goto 40 
150 if d != 2 goto 170 
160 print p 
170 if p < 75 goto 25 
180 end
