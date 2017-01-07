#include "FlowView.h"

#include <QtWidgets/QGraphicsScene>

#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtWidgets/QMenu>

#include <QtCore/QRectF>

#include <QtWidgets>

#include <QDebug>
#include <iostream>

#include "FlowScene.h"

#include "DataModelRegistry.h"

#include "Node.h"
#include "NodeGraphicsObject.h"

#include "ConnectionGraphicsObject.h"

#include "StyleCollection.h"

FlowView::
FlowView(FlowScene *scene)
    : QGraphicsView(scene)
    , _scene(scene)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);

    auto const &flowViewStyle = StyleCollection::flowViewStyle();

    setBackgroundBrush(flowViewStyle.BackgroundColor);

    //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    //setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setCacheMode(QGraphicsView::CacheBackground);

    //setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
}


void
FlowView::
contextMenuEvent(QContextMenuEvent *event)
{
    QMenu modelMenu;

    for (auto const &modelRegistry : _scene->registry().registeredModels())
    {
        QString const &modelName = modelRegistry.first;
        modelMenu.addAction(modelName);
    }

    if (QAction * action = modelMenu.exec(event->globalPos()))
    {
        QString modelName = action->text();

        auto type = _scene->registry().create(modelName);

        if (type)
        {
            auto node = _scene->createNode(std::move(type));

            QPoint pos = event->pos();

            QPointF posView = this->mapToScene(pos);

            node->nodeGraphicsObject().setPos(posView);
        }
        else
        {
            qDebug() << "Model not found";
        }
    }
}


void
FlowView::
wheelEvent(QWheelEvent *event)
{
    QPoint delta = event->angleDelta();

    if (delta.y() == 0)
    {
        event->ignore();
        return;
    }

    double const d = delta.y() / std::abs(delta.y());

    if (d > 0.0)
        scaleUp();
    else
        scaleDown();
}


void
FlowView::
scaleUp()
{
    double const step   = 1.2;
    double const factor = std::pow(step, 1.0);

    QTransform t = transform();

    if (t.m11() > 2.0)
        return;

    scale(factor, factor);
}


void
FlowView::
scaleDown()
{
    double const step   = 1.2;
    double const factor = std::pow(step, -1.0);

    scale(factor, factor);
}


void
FlowView::
keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        _scene->clearSelection();
        break;

    case Qt::Key_Delete:
    {
        std::vector<std::shared_ptr<Node>> nodesToDelete;
        std::vector<std::shared_ptr<Connection>> connectionsToDelete;
        for (QGraphicsItem * item : _scene->selectedItems())
        {
            if (auto n = dynamic_cast<NodeGraphicsObject*>(item))
                nodesToDelete.push_back(n->node().lock());

            if (auto c = dynamic_cast<ConnectionGraphicsObject*>(item))
                connectionsToDelete.push_back(c->connection().lock());
        }

        for( auto & n : nodesToDelete )
            _scene->removeNode(n);

        for( auto & c : connectionsToDelete )
            _scene->removeConnection(c);

    }

        break;

    case Qt::Key_Shift:
        setDragMode(QGraphicsView::RubberBandDrag);
        break;

    default:
        break;
    }

    QGraphicsView::keyPressEvent(event);
}


void
FlowView::
keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Shift:
        setDragMode(QGraphicsView::ScrollHandDrag);
        break;

    default:
        break;
    }
    QGraphicsView::keyReleaseEvent(event);
}


void
FlowView::
drawBackground(QPainter* painter, const QRectF& r)
{
    QGraphicsView::drawBackground(painter, r);

    auto drawGrid =
            [&](double gridStep)
    {
        QRect   windowRect = rect();
        QPointF tl = mapToScene(windowRect.topLeft());
        QPointF br = mapToScene(windowRect.bottomRight());

        double left   = std::floor(tl.x() / gridStep - 0.5);
        double right  = std::floor(br.x() / gridStep + 1.0);
        double bottom = std::floor(tl.y() / gridStep - 0.5);
        double top    = std::floor (br.y() / gridStep + 1.0);

        // vertical lines
        for (int xi = int(left); xi <= int(right); ++xi)
        {
            QLineF line(xi * gridStep, bottom * gridStep,
                        xi * gridStep, top * gridStep );

            painter->drawLine(line);
        }

        // horizontal lines
        for (int yi = int(bottom); yi <= int(top); ++yi)
        {
            QLineF line(left * gridStep, yi * gridStep,
                        right * gridStep, yi * gridStep );
            painter->drawLine(line);
        }
    };

    auto const &flowViewStyle = StyleCollection::flowViewStyle();

    QBrush bBrush = backgroundBrush();

    QPen pfine(flowViewStyle.FineGridColor, 1.0);

    painter->setPen(pfine);
    drawGrid(15);

    QPen p(flowViewStyle.CoarseGridColor, 1.0);

    painter->setPen(p);
    drawGrid(150);
}


void
FlowView::
showEvent(QShowEvent *event)
{
    _scene->setSceneRect(this->rect());
    QGraphicsView::showEvent(event);
}


void
FlowView::
mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
}


void
FlowView::
mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
}
