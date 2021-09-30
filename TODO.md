### TODO not in particular order~:
What needs to be done
- [x] ~~SCROLLING!!!!!!!~~
- [ ] Fix pngwen's pcx rendering code it dose not render large relolution pcx files well at all! almost done
- [ ] tweak animation frame perloop and make the dpad controls a case switch (look at commander keen code?) and vsync horizontal animation
- [x] ~~movement and player movement when reaching edge of map~~
- [x] sprite rendering
- [ ] text box rendering
- [x] text rendering
- [x] ~~YAY! map loading~~
- [x] ~~YAY! wwww map rendering~~~
- [ ] map and item interation
- [ ] item inventory~
* ==working on extending==
- [ ] Memory management [EMS + XMS] done but not tested...
- [x] case switch with keyboard code
- when your holding 2 arrow keys and a key is released the keyboard controller dose not read input from the port anymore for some weired reason
- [ ] masking layer! [i think this is not easy to accomplish!]
- [x] ~~8088 detection to turn off vsync!~~
- [x] *very important!* optimize draw row and draw col
- [ ] Study verge2's map system,
- [ ] save feature!
- [ ] config feature!
- [ ] *also important* interperator! and possibly custom map system
- [ ] extend the map system!
- [x] ~~opl2/3 audio!!!!~~
- [ ] engine core!
* ==working==
- [ ] *MOST important!* FUCKING MASSIVE DATA MANAGEMENT!!!
- [ ] MUSIC PLAYBACK!!!! WWWW
- [ ] basic SFX from opl2 + pc speaker!
- [ ] clean up repository after event 1.??
- [ ] script system for text and interaction
- [ ] ====00 IMPORTANT! 00==== AS PART OF FINISHING THE GRAPHICAL LIB. Make a palette manager that updates the display palette with the pallet of images and sprites. in a stack and reuse same colors in the pallette on the image by changing the image's values to the matching color.
- Use a little database to keep track of the images loaded and have manipulated the display pallette.
- [ ] migrate from malloc and related to mm stuff

- [ ] cache manager to work on newer file formats and the way the game will work
- [ ] 4 button input on joystick stuff

### stuff needs to be done that is realized in 2021:
- [ ] return to ylookuptable found in wolf3d
- [ ] clean up existing video system and organize the existing code into a clean library 16_VL and 16_VW
- [ ] *not sure* return ID engine code variables to their old variables
- [ ] *again not sure* use doslib's variables for video
- [ ] find a way to make adaptive tile refresh in p16

### just stuff in general:
- [ ] fix any bugs found
- [ ] make the game wwww


get dos_sj.rar
It may be very useful to us...
