# Praaline
# (c) George Christodoulides 2012-2014

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += pncore pnlib pngui \
           sub_dataquay svcore svgui svapp \
           libpraat \
           libs/qscintilla/Qt4Qt5/qscintilla.pro \
           libs/qcustomplot/qcustomplot.pro \
           plugins #\
           #praalinepy
SUBDIRS += app

sub_dataquay.file = dataquay/libdataquay.pro

pnlib.depends = pncore
pngui.depends = pncore
plugins.depends = pncore

svgui.depends = svcore
svapp.depends = svcore svgui

app.depends = pncore pnlib pngui sub_dataquay svcore svgui svapp
