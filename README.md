# pwlgen
Generate spice PWL statements from a table of digital values

## Building the Application

It's as plain vanilla as it can be, straight C using the standard C libraries, command line driven.

It was compiled in Windows using Pelles C (https://pellesc.se/) a free C compiler. It should be able to be compiled under Linux using gcc.

## Invocation

pwlgen in-file out-file options



## File format

A number sign (\#) in the first character of the line indicates a comment.

Spaces are ignored.

The first line that is not a comment must be a comma delimited list of signal names.

Here is an example of a file with 8 data lines, reset (rst_bar), write (wr) and lookup (lu).

>\# The first line gives the signal names  
>rst_bar, wr, lu, d7, d6, d5, d4, d3, d2, d1, d0  
>   0      0   0   0   0   0   0   0   0   0   0  
>   1      0   0   0   0   0   0   0   0   0   0  
>   1      0   1   0   1   0   0   0   0   1   0  
>   1      0   0   0   0   0   0   0   0   0   0  
>   1      1   0   1   0   0   1   1   1   0   0  
>   1      0   0   1   0   0   1   1   1   0   0  
>   1      0   1   0   1   0   0   0   0   1   0  
>   1      0   0   0   0   0   0   0   0   0   0  
>   1      0   1   1   0   1   1   1   1   0   1  
>   1      0   0   1   0   0   1   1   1   0   0  

The output is:

>\* Digital signals for cam.pwl  
>\* Clock width = 50ns, rise time = 5ns, fall time = 5ns  
>V1 clk 0 PULSE(0 1.80 0n 5n 5n 20n 50n)  
>V2 rst_bar 0 PWL(0n 0.00 50n 0.00 55n 1.80)  
>V3 wr 0 PWL(0n 0.00 200n 0.00 205n 1.80 250n 1.80 255n 0.00)  
>V4 lu 0 PWL(0n 0.00 100n 0.00 105n 1.80 150n 1.80 155n 0.00 300n 0.00 305n 1.80 350n 1.80 355n 0.00 400n 0.00 405n 1.80 450n 1.80 455n 0.00)  
>V5 d7 0 PWL(0n 0.00 200n 0.00 205n 1.80 300n 1.80 305n 0.00 400n 0.00 405n 1.80)  
>V6 d6 0 PWL(0n 0.00 100n 0.00 105n 1.80 150n 1.80 155n 0.00 300n 0.00 305n 1.80 350n 1.80 355n 0.00)  
>V7 d5 0 PWL(0n 0.00 400n 0.00 405n 1.80 450n 1.80 455n 0.00)  
>V8 d4 0 PWL(0n 0.00 200n 0.00 205n 1.80 300n 1.80 305n 0.00 400n 0.00 405n 1.80)  
>V9 d3 0 PWL(0n 0.00 200n 0.00 205n 1.80 300n 1.80 305n 0.00 400n 0.00 405n 1.80)  
>V10 d2 0 PWL(0n 0.00 200n 0.00 205n 1.80 300n 1.80 305n 0.00 400n 0.00 405n 1.80)  
>V11 d1 0 PWL(0n 0.00 100n 0.00 105n 1.80 150n 1.80 155n 0.00 300n 0.00 305n 1.80 350n 1.80 355n 0.00)  
>V12 d0 0 PWL(0n 0.00 400n 0.00 405n 1.80 450n 1.80 455n 0.00)  
>.END  

## Author

Pete Koziar
(retired firmware engineer, also the author of 3 science fiction novels:

* *Dauntless Homecoming* (<https://www.amazon.com/Dauntless-Homecoming-Pete-Koziar/dp/1453637958/>),
* *Seeking Adam* (<https://www.amazon.com/Seeking-Adam-Book-Galactic-Redemption/dp/1475227388/>)
* *Holy War* (<https://www.amazon.com/Holy-War-Galactic-Redemption-2/dp/1499288816/>)
