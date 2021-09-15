# CHILLY CHEESE MANUAL

Chilly Cheese is a digital recreation/homage (made with permission!) of the analog eurorack module "Cold Mac", made by Mannequins / Whimsical Raps. **However, it was not made by Whimsical Raps, and is not sponsored or endorsed by them.** If you are interested in purchasing a real Cold Mac, please check here for more information on their availability: https://www.whimsicalraps.com/products/cold-mac

There is so much that can be said about Cold Mac - It's design is simply brilliant and I find it very inspiring. I describe it as an "Esoteric Macro-Utility", which can do so many things (but unified by the concept of a unified macro controlling it all).

**It takes voltages from -5 to +5 for all it's inputs - this is to mirror the behavior of the original module. The audio mixer and slew, however, can output up to 10v total before clipping.**

This module is more than the sum of it's parts - however, it may be useful to list these parts to wrap your head around how the module functions. It can be described as:

* 6 channel audio mixer (filters out dc)
* analog OR + AND gate (outputs max / min of two different inputs)
* voltage controlled crossfader with a voltage offset and two outputs (one on the left and one on the right side of the fade, if that makes sense)
* unique crease-shape wavefolder, great for processing lfos or audio
* signal rectifier
* envelope follower
* integrator

The real fascination with it's design, however, is in how all of these functions are interconnected through normalling. By following the gray lines and bubbles on the module, you can get a sense for how these are distributed, but in practice almost all the normallings can be broken if desire. However, the joy of cold mac is in it's one big knob (called "SURVEY" on Cold Mac, but called "MACRO" on Chilly Cheese).

Using this one big knob, you can control many or all of the function blocks simultaneously, leading to massive changes in sound or cv happening with just small tweaks of the knob. It's a genius design, and it's this concept (as well as the relative simplicity of each of the building blocks) which made this the perfect module for me to learn VCVrack development with.

All this information is really just a teaser of what you can do with Chilly Cheese - however, any documentation I could write would pale in comparison to the wonderful work which has already been done for Cold Mac. For this reason, I highly recommend you check out the following links:

* The Official Cold Mac Technical Map: https://github.com/whimsicalraps/Mannequins-Technical-Maps/blob/master/cold-mac/cold-mac-web.md
* Patching Cold Mac: https://doudoroff.com/cold-mac/
* Cold Mac Ideas (on Lines Forum): https://llllllll.co/t/cold-mac-ideas/3840

These will get you up and running patching Chilly Cheese - **the only differences to keep in mind between Cold Mac and Chilly Cheese are:**

1. Naming - "SURVEY" has become "MACRO", and "MAC" has become "CHEESE". Yum.
2. This is an imperfect digital emulation of an analog module - it will behave differently in some scenarios (especially feedback patching), but most patches should work almost identically on both units. Please send me a message if that is not the case!
3. Finally, the one added feature (for now!) is that you can adjust the speed of the "LOCATION" (aka integrator) circuit by Right-Clicking the module and choosing from the 5 available speeds: The "default" setting is halfway between "Slowish" and "Quickish"

Please enjoy Chilly Cheese responsibly, and send me an email if you find any issues with Cold Mac, or if you just wanna say hey! You can also open a github issue for the former case.

Thanks for reading :)

- Love, Allie
- alliewayaudio@gmail.com
