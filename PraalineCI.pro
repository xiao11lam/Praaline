# Praaline
# (c) George Christodoulides 2012-2017

TEMPLATE = subdirs
CONFIG += ordered

# Dependencies
SUBDIRS += dependencies/qt-solutions/qtpropertybrowser
SUBDIRS += dependencies/qtilities/src/Qtilities.pro
SUBDIRS += dependencies/qscintilla/Qt4Qt5/qscintilla.pro
SUBDIRS += dependencies/qcustomplot/qcustomplot.pro
# Praaline
SUBDIRS += pncore pnlib pngui
SUBDIRS += sub_dataquay svcore 
 SUBDIRS += svgui svapp
# SUBDIRS += libpraat
# SUBDIRS += plugins
# SUBDIRS += praalinepy
SUBDIRS += app

sub_dataquay.file = dataquay/libdataquay.pro

pnlib.depends = pncore
pngui.depends = pncore
plugins.depends = pncore

svgui.depends = svcore
svapp.depends = svcore svgui

app.depends = pncore pnlib pngui sub_dataquay svcore svgui svapp
