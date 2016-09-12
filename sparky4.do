todo of sparky4
use space??
rm segmlen ok
code it!

what?



test which modex library is faster! workings w
edit input and switch unecessary words to bytes ok

font! ok
jis! ...
memory segmentation! trying ow2

update mapreading! to the new system!
	replace png with pcx ^^;
		so the game can read it~

dro from dosbox
dro to vgm get

16_in
16_mm

READ DESIGN.TXT






[13:11]	Dark_Fox	Well, it's hard for me to unspaghetti the code to see what's going on here, but it looks like you're writing directly to the video part of the memory each time you have an update
[13:12]	sparky4	Dark_Fox:  yeah
[13:12]	Dark_Fox	It also looks like a significant amount of your functions use global variables
[13:12]	sparky4	i know
[13:12]	sparky4	it's a game
[13:12]	Dark_Fox	That's why your code is shit, though
[13:12]	sparky4	....
[13:12]	Dark_Fox	You don't have a video buffer and you're using way too many global variables
[13:12]	sparky4	if you want to then fix ww
[13:13]	Dark_Fox	This is what I was talking about, you don't understand what you're writing
[13:13]	sparky4	is it called global variables to pointers?
[13:13]	sparky4	Dark_Fox: where the variables at?
[13:13]	Dark_Fox	Global variables are stored in a different part of memory, so you have to make a far call every time you access it
[13:14]	Dark_Fox	Which is much less efficient than a near call (like a variable that has been instantiated inside of your function)
[13:14]	sparky4	Dark_Fox: ah~
[13:14]	Dark_Fox	Which basically amounts to taking up extra CPU time everytime you use a global variable as opposed to a local variable
[13:14]	sparky4	i know i know!
[13:14]	Dark_Fox	You have the same problem with the video memory, which is partially unavoidable, but you need to be smarter about when and how you access the video memory
[13:15]	Dark_Fox	Instead of touching it every time there's a change, you should set a timer that says, like, thirty times per second you update the video memory
[13:15]	Dark_Fox	It could be even lower, like ten times per second you make a draw call
[13:15]	sparky4	Dark_Fox: ah that is in there
[13:15]	Dark_Fox	And everytime there's a change between those updates, you just store the update in a buffer instead
