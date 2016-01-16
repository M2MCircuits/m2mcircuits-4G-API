This readme is for other developers who are interested in continuing are work.  
If you want to use other AT commands, you can use the cModemStream and cModemInterface functions to interact directly with the device.  
For examples on how to send and parse commands, feel free to browse the source.

Our original library is in the Lib_old directory.  In addition to being C++, we had issues with modem latency and string/memory management.  Our library would stop reading before the modem finished responding, causing commands to appear as if they failed when in reality they succeeded. 	

Some notes about the library: 
Since this code runs on multiple systems, we do not allocate (aside from the stack) any memory, besides the ModemStream and ModemInterface structures in mmCreate on Arduino.  
I (Jagger) could not get timed readouts to work reliably on Linux, so the library does not rely on this functionality.  Unfortunately, that means that Find and FindChar will never stop searching until it finds its target.  
The library does not currently process unsolicited responses.  This means that the library is unable to respond to network loss.  Implementing this functionality would be a good candidate for future work.
The library also does not do much error-checking, in order to keep memory and processing requirements low.
if you have any questions, feel free to contact jagger.mccord@gmail.com or verneil.mesecher@utdallas.edu
