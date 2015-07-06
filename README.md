彼岸花 Project 01 話の地球 //Higanbana Project 01 ~ Story of Chikyuu
==============
Open watcom to compile the code of project 16
-----------
``16.exe`` = actual planned game executable!
/ = root of project where executables are at
16/ = experimental branch and example codes
16/modex16/ = pngwen's code archived~
src/ = source of project~
doc/ = documentation of the project~
doc/faq.txt = frequently asked questions file!

TODO not in particular order~:
-----------
::
DONE 1. SCROLLING!!!!!!!
2. Fix pngwen's pcx rendering code it dose not render large relolution pcx files well at all!
almost done  3. tweak animation frame perloop and make the dpad controls a case switch (look at commander keen code?) and vsync horizontal animation
DONE 4. movement and player movement when reaching edge of map
DONE 5. sprite rendering
6. text box rendering
7. text rendering
DONE! YAY! 8. map loading
DONE! YAY! wwww 9. map rendering~
10. map and item interation
11. item inventory~
==working on extending== DONE! but needs work! 12. Memory management (EMS + XMS)
done but not tested... 13. case switch with keyboard code (when your holding 2 arrow keys and a key is released the keyboard controller dose not read input from the port anymore for some weired reason)
14. masking layer! (i think this is not easy to accomplish!)
DONE 15. 8088 detection to turn off vsync!

==working== 16. (very important!) optoimize draw row and draw col

not needed ... really... 17. merge map system and interperator from verge 2
18. save feature!
19. (also important) interperator! and possibly custom map system
20. (also important) lua scripting system!
21. extend the map system!
22. opl2/3 audio!!!!

DONE! use IN_ReadControl!!

23. engine core!
==working== 24. (MOST important!) FUCKING MASSIVE DATA MANAGEMENT!!! related to 12.


get dos_sj.rar

It may be very useful to us...

::

To compile:
You need Open Watcom stable to compile and just run wmake
