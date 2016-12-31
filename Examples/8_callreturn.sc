10 rem This is an example of call and return functionality
15 goto 50
20 let y = x*x
30 print y
40 return
50 let x = 0
60 call 20
70 let x = x+1
80 if x < 6 goto 60
90 end

