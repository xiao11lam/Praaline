#ifndef ANNOTATIONGRIDLAYER_H
#define ANNOTATIONGRIDLAYER_H

#include "pngui/model/annotation/AnnotationGridModel.h"
#include "svgui/layer/SingleColourLayer.h"

#include <QObject>
#include <QColor>

class View;
class QPainter;

class AnnotationGridLayer : public SingleColourLayer
{
    Q_OBJECT

public:
    enum PlotStyle {
        PlotBlendedSpeakers,
        PlotSpeakersThenLevelAttributes,
        PlotLevelAttributesThenSpeakers
    };

    AnnotationGridLayer();

    std::string getType() const { return "AnnotationGrid"; }

    virtual const Model *getModel() const { return m_model; }
    virtual bool trySetModel(Model *);
    void setModel(AnnotationGridModel *model);

    virtual void paint(View *v, QPainter &paint, QRect rect) const;

    virtual QString getFeatureDescription(View *v, QPoint &) const;

    virtual bool snapToFeatureFrame(View *v, sv_frame_t &frame,
                                    int &resolution, SnapType snap) const;

    virtual void drawStart(View *v, QMouseEvent *);
    virtual void drawDrag(View *v, QMouseEvent *);
    virtual void drawEnd(View *v, QMouseEvent *);

    virtual void eraseStart(View *v, QMouseEvent *);
    virtual void eraseDrag(View *v, QMouseEvent *);
    virtual void eraseEnd(View *v, QMouseEvent *);

    virtual void editStart(View *v, QMouseEvent *);
    virtual void editDrag(View *v, QMouseEvent *);
    virtual void editEnd(View *v, QMouseEvent *);

    virtual void moveSelection(Selection s, sv_frame_t newStartFrame);
    virtual void resizeSelection(Selection s, Selection newSize);
    virtual void deleteSelection(Selection s);

    virtual void copy(View *v, Selection s, Clipboard &to);
    virtual bool paste(View *v, const Clipboard &from, sv_frame_t frameOffset,
                       bool interactive);

    virtual bool editOpen(View *, QMouseEvent *); // on double-click

    virtual PropertyList getProperties() const;
    virtual QString getPropertyLabel(const PropertyName &) const;
    virtual PropertyType getPropertyType(const PropertyName &) const;
    virtual int getPropertyRangeAndValue(const PropertyName &,
                                         int *min, int *max, int *deflt) const;
    virtual QString getPropertyValueLabel(const PropertyName &,
                                          int value) const;
    virtual void setProperty(const PropertyName &, int value);

    virtual bool isLayerScrollable(const View *v) const;

    virtual bool isLayerEditable() const { return true; }

    virtual int getCompletion(View *) const { return 100; }

    virtual bool getValueExtents(double &min, double &max,
                                 bool &logarithmic, QString &unit) const;

    virtual int getVerticalScaleWidth(View *, bool, QPainter &) const { return 0; }

    virtual void toXml(QTextStream &stream, QString indent = "",
                       QString extraAttributes = "") const;

    void setProperties(const QXmlAttributes &attributes);

    void setPlotStyle(PlotStyle style);
    PlotStyle getPlotStyle() const { return m_plotStyle; }

protected:
    int getYForTierIndex(View *v, int tierIndex) const;
    int getTierIndexForY(View *v, int y) const;

    virtual int getDefaultColourHint(bool dark, bool &impose);

    // AnnotationGridModel::PointList getLocalPoints(View *v, int x, int y) const;

    // bool getPointToDrag(View *v, int x, int y, AnnotationGridModel::Point &) const;

    AnnotationGridModel *m_model;
    PlotStyle m_plotStyle;
    QList<AnnotationGridModel::TierTuple> m_tierTuples;

    bool m_editing;
    QPoint m_editOrigin;

//    AnnotationGridModel::Point m_originalPoint;
//    AnnotationGridModel::Point m_editingPoint;
//    AnnotationGridModel::EditCommand *m_editingCommand;

//    void finish(AnnotationGridModel::EditCommand *command) {
//        SVCommand *c = command->finish();
//        if (c) CommandHistory::getInstance()->addCommand(c, false);
//    }
};

#endif // ANNOTATIONGRIDLAYER_H