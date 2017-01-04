10 rem This is an example of call and return functionality
20 let x = 0
30 call 70
35 rem testrem
40 let x = x+1
50 if x < 6 goto 30
60 end
65 rem This is a test of putting rems all over
70 let y = x*x
80 print y
90 return
100 rem the above code is equivalent to
110 rem for(int x = 0; x < 6; x++)
120 rem printf("%d\n", x*x);