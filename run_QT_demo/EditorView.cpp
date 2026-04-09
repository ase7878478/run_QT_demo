#include "EditorView.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>
#include <QDebug>

EditorView::EditorView(QWidget *parent)
    : QGraphicsView(parent)
{
    m_scene = new EditorScene(this);
    setScene(m_scene);

    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setDragMode(QGraphicsView::RubberBandDrag);

    // 必须启用拖放
    setAcceptDrops(true);
}

void EditorView::createNodeAtViewPos(const QPoint &viewPos, const QString &nodeType)
{
    QPointF scenePos = mapToScene(viewPos);
    m_scene->createNodeAt(scenePos, nodeType);
    qDebug() << "Node created at" << scenePos << "type:" << nodeType;
}

void EditorView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-node-type")) {
        event->acceptProposedAction();
        qDebug() << "dragEnter accepted";
    } else {
        event->ignore();
    }
}

void EditorView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-node-type")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void EditorView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-node-type")) {
        QString nodeType = event->mimeData()->data("application/x-node-type");
        createNodeAtViewPos(event->pos(), nodeType);
        event->acceptProposedAction();
        qDebug() << "drop handled, nodeType:" << nodeType;
    } else {
        event->ignore();
    }
}

void EditorView::drawBackground(QPainter *painter, const QRectF &rect)
{
    // 与之前相同
    QGraphicsView::drawBackground(painter, rect);
    painter->fillRect(rect, QColor(40, 40, 40));

    const int gridSize = 20;
    QPen pen(QColor(60, 60, 60));
    pen.setWidth(1);
    painter->setPen(pen);

    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

    QVarLengthArray<QLineF, 100> lines;
    for (qreal x = left; x < rect.right(); x += gridSize)
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += gridSize)
        lines.append(QLineF(rect.left(), y, rect.right(), y));

    painter->drawLines(lines.data(), lines.size());
}
