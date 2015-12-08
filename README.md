# LIBKSS

LIBKSS is a music player library for MSX music formats, forked from MSXplug.
Supported formats are .kss, .mgs, .bgm, .opx, .mbm.

# How to build

The following step builds `libkss.a` library.

```
$ git clone https://github.com/digital-sound-antiques/libkss.git
$ cd libkss
$ git submodule update --init --recursive
$ mkdir build; cd build
$ cmake ..
$ make
```

You can also build the KSS to WAV converter binary as follows.

```
$ make kss2wav
```

