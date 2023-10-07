彼岸花 Project 01 話の地球 //Higanbana Project 01 ~ Story of Chikyuu
==============
Open watcom to compile the code of project 16
--------------

### Directory Structure!

16.exe = actual planned game executable!
/ = root of project where executables are at
16/ = experimental branch and example codes
16/modex16/ = pngwen's code archived~
src/ = source of project~
doc/ = documentation of the project~
doc/faq.txt = frequently asked questions file!

### To compile:

You need Open Watcom stable to compile and just run wmake

###TODO
https://github.com/sparky4/16/blob/master/TODO.md

	project 16 is in deep dump right now
	it is leaving the pre alpha state and heading into alpha state www
	which means cache manager work but... it needs much rewriting for the new file formats

#### info about the game:
top down scroller similar to the game Ib
my budget is small because i am a broke collage student
the main issues i have is reworking the chache manager of the old id engine to work with the new file formats and getting it all together to work
and reworking old experimental code into id_RF
there is alot of work that i dont know how to make it work
i will do alot of coding but i am stuck on the game engine cache manager and sound issues
yeah there is no sound system for the game either
there is the latest stuff from  https://github.com/sparky4/16/issues/7
the game is a 16 bit top down scroller in 4 directions where is loads more levels and maps within levels similar to the game Ib
it will have a save feature, OPL3 and PC speacker sounds, a custom palette manager that loads palette for different sprites and stuff on the fly and merge matching colors together
i need to return the ylookup table for the graphical system
which is what wolf 3d used
