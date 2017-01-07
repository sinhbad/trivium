# trivium
Experimental C implementation of trivium

http://it.folkcovers.org/trivium-simple-stream-cipher/

Using:

$ sha1sum 1.txt
> 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12  1.txt
$ ./encrypt 1.txt 1.enc.txt
> Trivium (encryption) by sinhbad. January, 2017.
> Using: trivium.exe input.file output.file
> Type key in hexadecimal format (80 bit):
> 12867bcaaa0db7a935bd7
> Initial state:
> 12    86    7B    CA    AA    0D    B7    A9    35    BD
> 00    05    53    1A    81    AD    77    BB    9B    BE
> 7F    28    00    00    00    00    00    00    00    00
> 00    00    00    00    00    07    
$ sha1sum  1.enc.txt
> fc13fcbdc53b82c9452a334681d723b474bcc76d  1.enc.txt
$ ./decrypt 1.enc.txt 1dec.txt
> Trivium (decryption) by sinhbad. January, 2017.
> Using: trivium.exe input.file output.file
> Type key in hexadecimal format (80 bit):
> 12867bcaaa0db7a935bd7
> Initial state:
> 12    86    7B    CA    AA    0D    B7    A9    35    BD
> 00    05    53    1A    81    AD    77    BB    9B    BE
> 7F    28    00    00    00    00    00    00    00    00
> 00    00    00    00    00    07    
$ sha1sum 1dec.txt 
> 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12  1dec.txt
