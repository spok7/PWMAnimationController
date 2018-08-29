To the user:

WARNING!
The library is functional, but not fully tested. Although addAnimation exists for adding animations to animation chains, is not a working feature. Other bugs may arise.

To use this library, open the Arduino IDE and import library from zip. Chose the PWMControllerWithLoconet.zip file. If that doesn't work, unzip the  file, and add the folder to the Arduino libraries folder manually. Examples are enclosed within.

Add Serial.begin(9600); at the beginning of the setup loop, program will fail otherwise.


To the dev:

Don't panic!

Todo:
- fix addAnimation in AnimationChain
- improve waterfall stuff, such as colours (other project)
- add example for a more difficult implementation that uses addAnimation in both nodes and chains
- the documentation was written using draw.io; import the .xml into the website to edit it
