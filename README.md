controlfilter.lv2
=================

LV2 plugins to filter/process Control Parameters, intended to be used with
modular synthesizers, in particular [ingen](http://drobilla.net/software/ingen/)

So far 5 filters have been implemented:

*   Linear:  `out = a * in + b`
*   Invert:  `out = 1.0 / in`
*   Exponential:  `out = a ^ in`
*   Base-n Logarithm:  `out = a * log ( abs (b * in) ) / log (c)`
*   Low Pass Filter, separate time-constants for rise (attack) and fall (release)

Install
-------

Compiling the plugins requires LV2 SDK, gnu-make and a c-compiler.

```bash
  git clone https://github.com/x42/controlfilter.lv2.git
  cd controlfilter.lv2
  make
  sudo make install PREFIX=/usr
```
