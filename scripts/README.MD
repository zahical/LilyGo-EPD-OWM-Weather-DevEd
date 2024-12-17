Updated README
==============

This folder contains slightly modified versions of the support scripts from the [LilyGo-EPD47 library](https://github.com/Xinyuan-LilyGO/LilyGo-EPD47/tree/v1.0.1/scripts). The following changes have been implemented:

imgconvert
----------

The script no longer resizes the input image. Now, it's just converted to grayscale and then transformed to a C byte array preserving its dimensions. Images with odd width and/or height are still supported.

fontconvert
-----------

One no longer needs to edit the `fontconvert.py` file to specify the unicode ranges that will be supported in the generated bitmap font. Instead, the script now supports a `--range` parameter.

The `--range` parameter allows for several different ways to specify the requested range.

* A tuple of ints

    `--range 32,34` : This specifies a range of code points (in base-10). The end value is inclusive.

* A single char literal

    `--range a` : This specifies a range including that single char. **Single** digit chars, e.g. `--range 5` are also interpreted this way.

* A multi-digit number

    `--range 32` : This specifies (in base-10) a range including that single code point.

* A simplified (written without the leading & and trailing ;) HTML entity reference

    `--range ast` :  This specifies a range including that single code point (in that case `*`). Only ASCII code-points are supported.

* A well-known name

    `--range ascii` :  The following values are supported: `ascii` (32,126), `lowercase` (97, 122), `uppercase` (65,  90), `digits` (48,  57) and `space` (32,  32).

The `--range` parameter can be specified multiple times on the command line with cumulative effect. The ranges can be specified in any order and may overlap. Before generation, the script will normalize and merge the ranges, if needed.

See `gen.sh` in that folder for a sample usage.

dependencies
------------

If you are using the .devcontainer environment, the prerequisites (`freetype-py` and `pillow`) have already been installed. There's no need to install or run `pip` as described below.

Original README
==============

fontconvert
-----------

Prerequisite you need to install python3 and install `freetype-py` using pip
The approximate process is like this:

.. code-block:: shell

    sudo apt install python3-pip
    python3 -m pip install freetype-py

The previous is the prerequisite for implementation, and then you need to store the font file you want to convert in the same path as fontconvert.py. This is just for more convenient operation. You can also fill in the font path.

Then just follow the command below to convert the font.

.. code-block:: shell

    python3 fontconvert.py --compress demo 16 msyh.ttc > font.h

Explanation of specific parameters:

.. code-block:: shell

    python3 fontconvert.py --compress [generated font name] [font size] [font file path] > [generated font file]

Of course, this only demonstrates the generation of standard ascii codes. If you need other fonts, you only need to fill in the unicode encoding of the font to be generated in the `fontconvert.py`  `intervals` list.

Please make sure that the unicode encoding in the fontconvert.py intervals list is included in your font file, otherwise please comment other encodings and only keep the 32,126 range!

.. image:: https://user-images.githubusercontent.com/22990954/104092951-446c1880-52c2-11eb-852e-8c50626eb340.png

imgconvert
----------

1. Prerequisites

.. code-block:: shell

    python3 -m pip install pillow

2. Instructions

.. code-block:: shell

    imgconvert.py [-h] [-i INPUTFILE] [-n NAME] [-o OUTPUTFILE]

3. examples:

.. code-block:: shell

    python imgconvert.py -i demo.png  -n demo -o demo.h
