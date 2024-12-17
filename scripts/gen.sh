#!/bin/bash

# A script to generate code for embedding of the image and font assets
# Should be run in the same folder where it resides

python imgconvert.py -i ../assets/DST.png -o ../src/imgs/DST.h -n ImgDST

python imgconvert.py -i ../assets/TLo.png -o ../src/imgs/TempLo.h -n ImgTempLo
python imgconvert.py -i ../assets/THi.png -o ../src/imgs/TempHi.h -n ImgTempHi
python imgconvert.py -i ../assets/TFL.png -o ../src/imgs/TempFL.h -n ImgTempFL

python imgconvert.py -i ../assets/AirTree.png -o ../src/imgs/AirTree.h -n ImgAIQ
python imgconvert.py -i ../assets/Gauge0A.png -o ../src/imgs/Gauge0.h -n ImgGaugeFrame0
python imgconvert.py -i ../assets/Gauge1A.png -o ../src/imgs/Gauge1.h -n ImgGaugeFrame1
python imgconvert.py -i ../assets/Gauge2A.png -o ../src/imgs/Gauge2.h -n ImgGaugeFrame2
python imgconvert.py -i ../assets/Gauge3.png -o ../src/imgs/Gauge3.h -n ImgGaugeFrame3
python imgconvert.py -i ../assets/Gauge4.png -o ../src/imgs/Gauge4.h -n ImgGaugeFrame4

python imgconvert.py -i ../assets/xSunset.png -o ../src/imgs/sunset.h -n sunset

# Before running the lines below, you'll need to download and extract in this folder the relevant
# OpenSans ttf files from here https://fonts.google.com/specimen/Open+Sans. The static versiosn are used.

# python fontconvert.py --compress OpenSans6B 6 OpenSans-Bold.ttf  > ../src/fonts/opensans6b.h
# python fontconvert.py --compress --range lowercase --range digits --range colon --range semi --range dot --range space \
#    OpenSans5CB_Special2 5 OpenSans_Condensed-Bold.ttf > ../src/fonts/opensans5cb_special2.h