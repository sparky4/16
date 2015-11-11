This is XLIB the mode X graphics library. There is no organization to the
files in this archive; that is left up to each users preferences.

Famous last words ;-)

"This is the last version I intend to release, unless of course someone has
something really nifty they'd like to contribute."

MOUSE TIP:

Theres a slight design flaw in the mouse driver and lots of people have had
problems with mouse droppings in the lower RH corner of the screen. This 
happens under some circulstances because the code does not clip the saved
background as is done for the cursor graphic itself so bits of the mouse 
graphic stored in the video ram just below the visual page get picked up into 
the background.

To bypass this problem is simple when you know how. If you are having this 
problem insert NonVisual_Offs+=ScrnLogicalByteWidth*14 just before the call
to x_init_mouse.

Sorry about not fixing the problem more elegantly, but I didn't want to 
do any major work on the mouse driver.

Themie







