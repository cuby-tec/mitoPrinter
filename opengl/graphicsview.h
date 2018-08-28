#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QtGui>
//#include <QGLWidget>
#include <QtWidgets>


class GraphicsView : public QGraphicsView
{
public:
    GraphicsView()
    {
        setWindowTitle(tr("3D Model Viewer"));
        setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform); //added
    }

    GraphicsView(QWidget *parent){
        setWindowTitle(tr("3D Model Viewer"));
        setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform); //added
    }

protected:
    void resizeEvent(QResizeEvent *event) {
        if (scene())
            scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        QGraphicsView::resizeEvent(event);
    }
};

#endif // GRAPHICSVIEW_H
