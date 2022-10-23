PLEASE READ THIS
--------------------------------------------------------------------------------
This is a custom firmware plugin that allows you to play music while using the
vsh or playing a umd/homebrew game. If you don't have any other plugins,
you can just copy the seplugins folder to the root of your memory stick.

If you do have plugins, copy music_conf.txt, music.prx to the seplugins
folder and add ms0:/seplugins/music.prx to vsh.txt/game.txt.
*If you have the old version, you can delete hw.prx, it isn't needed anymore.

This has been tested on 3.52-M33 2, on 3.40OE, you should not clock above
222/111 in the vsh, (either manually or with the settings file), because it
will cause the vsh to not work/crash.

Also, if you are on 3.40OE , you wont be able to use this during homebrew
(unless you manually flash it, but updating to the newest cfw would be better).

This version doesn't support pops, I'm not sure if its possible to get it working
but I'll do what I can.
--------------------------------------------------------------------------------
Once installed, take a look at the music_conf.txt, which has various options you
can change:
DIR specifies where your music is located (includes all subdirs),

then you can modify the default controls if you want,

OC_WLAN is really only for people on custom firmware 3.03OE or older, since in
newer cfw this is built in, it lets you use the wireless while overclocked

set ENABLE_VSH_CLOCK = 0, if you want to disable the vsh from
messing with the clock speed.

There are also some settings you can change to determine default behavior
(mode = rand/seq, volume, and cpu_speed).
--------------------------------------------------------------------------------
This version now supports atrac3 as well as mp3 files. Copy any music files to
the music dir, files copied to OMGAUDIO using sonic stage are also supported.

NOTE - Sonic stage files must not have copy protection enabled, or they won't play.
 Newer versions should let you remove copy proctetion, make sure when converting,
 the "Add copy protection" button is unchecked.

Also this version supports *.at3 files (atrac3 with a wave header). Most games use
this format for their music, so if you like a song in a game you can use file 
assistant (or whatever) to copy the *.at3 files to the music dir. 
--------------------------------------------------------------------------------
The new hardware decoder will work with all umd games/the vsh/ and most homebrew.
NOTE - It will not work with homebrew that uses the ME (SnesTYL ME, dosbox .71, etc)
     - In the next release I'll have it switch back to the software decoder when
       the ME is in use, but until then you'll have to use the non-ME version
       or not use the music plugin
     - Also when playing an mp3 that has a samplerate different from 44.1kHz,
       you need to pause the song before the vsh's video/music player will work

If you have a problem with a game starting, try waiting until you are at the 
main menu to start playing music.

If you get an error message/if you hit play and don't hear any music, turn
on the on screen display, and look for a line in blue. Please post the error message in
the release thread along with a description of what you were playing.
(Make sure that you installed the prx correctly)

Default Controls:
(The Note button is to the left of select)
Note + Left Trigger = pause/resume playback
Note + Right Trigger = switch playback mode between random and seqeuntial
Note + D-Pad Left  = previous song (only in sequential mode)
Note + D-Pad Right  = next song
Note + D-Pad Up = Volume up (for the music)
Note + D-Pad Down = Volume Down (for the music)
Note + Triangle = Change CPU speed forwards
Note + Cross = Change CPU speed backwards
Note + Square = Toggle On Screen Display (default off)
Note + Circle = Reload list of mp3 files (needed if you change the files in the music folder using usb)
Note + Home = In game mute (stops the vsh/game from outputting sounds)
Note + Start = Loop the current song, if you press next/prev, the next song will be looped

Changelog:
0.1: Initial release
0.2: Fixed usb,suspend and resume bugs
     Added a seperate flasher for vsh mode
0.2b: Fixed a bug which could crash the psp if the mp3 filename was too long (Note there is still a limit of 100 mp3 files)
      Added subdirectory scanning (there can be any number of subdirectories)
      Added previous mode in Sequential mode 
      Modified the controls
0.2c: Fixed a bug that would cause a crash if there were more than the max # of mp3s on the memorystick
      Raised the playlist limit to 300 files
      Made on screen display off by default because it sometimes causes a crash 
      Moved allocation of playlist to user mem. This may increase compatibility.
0.3:  Re-did the playlist code so no memory allocation is needed. This saves alot of mem and will reduce crashes.
      Reset to default CPU speed on exit
0.3a: Added more CPU speeds
      Added a brightness toggle including maximum brightness(the one that normally requires your psp to be plugged in) (NOTE+X)  
0.3b: Added an independent volume button
      Should startup faster
      Made the on screen display stay on when Note + Square is pressed
      Added volume and filename to the on screen display
0.4:  Fixed for use under 3.02OE
0.4b: Uploaded fixed vsh.txt
0.4c: Added a config file ms0:/seplugins/mp3conf.txt to change the MP3 playback directory and have custom controls
0.4d: Fixed overclocking for speeds > 222MHz
      Made the 4th level brightness work by pressing the screen button
      Added an option (see the config file) to allow overclocking and wireless to be used at the same time
0.4e: Fixed the on screen display in other display modes
      Reduced the flicker of the on screen display
      Added an option (see the config file) to disable the vsh from changing the clock speed when the screen turns off
      Added a back to the cpu selection 
      (**NOTE: for some reason, the bus speed when going backwards is slower for speeds <222MHz than when going forwards)
      (I don't think this is a big problem, but I will see if I can fix it later)
0.4f: Fixed the wifi @ 333mHz for 3.03OE-A+
      Fixed disable underclocking in vsh while the screen is off for 3.03OE-A+
      Fixed a bug that messes up the sound in the browser (although something else may have been broken) (only works on 3.03OE-A+)
      Fixed a bug (that was introduced in the last release) that broke suspend support
0.5:
    - Added In Game mute
    - Added a HW decoder (thanks to cooleyes), which should make playback faster
    - Fixed various bugs, overclocking the vsh browser now works
    - Added wave atrac3 (*.at3) and sonic stage atrac3 (*.oma/*.omg/*.aa3) support
    - Added support for music copied to the psp using sonic stage (in OMGAUDIO)
    - Changed how the cpu is changed, it is now based on the current speed, even
      if some other program changed it
    - CPU + Mode settings are now saved, (if a game changes the cpu speed, it 
      isn't saved, but if you change the speed it will be saved, also nothing
      prevents a game from changing the speed after we set it )
    - Added support for mp3's at samplerates != 44.1kHz
    - Added a loop current song button
0.51 beta:
    - Fixed an out of memory bug which prevented music from playing in some games (gta vcs)
0.52 beta:
    - Fixed a bug which prevents some atrac3/atrac3 plus files from playing, please try this and see if this fixes the problems you were having
0.53 beta:
    - Fixed another bug which prevented some atrac3/atrac3 plus files from playing
0.54 beta:
    - Removed the settings file because it was causing a problem
    - Added some settings into music_conf.txt

Uses blitting code from the vshex module in the devhook 043sdk and from the unofficial psp sdk.
Uses a modified version of cooleyes sceAudiocodec music decoder
The source code is release under the GPL license (see copyright for details)
-----------------------------------------------------------------------
This is from the original sw decoder, but since I might put it back in, I'll
leave this here.

from orginal irsmp3.prx by AhMan
This MP3 Player plugin is based on the MP3 player from neogeoCD 0.91
from Yoyofr & ZeLurker and is released under the terms of the GUN
General Public License (GPL).  The original MP3 player from neogeoCD
is based on MAD (libmad) MPEG audio decoder library.

To compile this MP3 player, you'll need to download the neogeoCD 0.91
soruce code from http://www.rainemu.com/html/archive/tux/neocdpsp.html.  
After download and extracted the source codes, you'll need to remove
serveral "assert" statements (remove them or comment them out) from
timer.c & layer3.c from the libmad library source.
Then, compile neogeoCD and a libmad.a library will be generated.

To compile the MP3 player, modify the Makefile to point to the newly
generated libmad.a library & header file, or copy them to your MP3 player
source directory.

Cheers,
AhMan
-----------------------------------------------------------------------
//You probably won't need this
The precompiled libmad.a is included along with the source under neocdpspsrc/src/libmad.
To recompile it:
cd neocdpspsrc/src/libmad
./configure
cd ../..
make obj/libmad.a
