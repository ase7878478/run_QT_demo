#include "Connection.h"
#include "Port.h"
#include "Commands.h"
#include "EditorScene.h"
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <cmath>

// 静态成员初始化（默认正交）
Connection::PathType Connection::s_defaultPathType = Connection::Orthogonal;

Connection::Connection(Port *startPort, Port *endPort, QGraphicsItem *parent)
    : QGraphicsPathItem(parent), m_startPort(startPort), m_endPort(endPort)
{
    setFlag(QGraphicsItem::ItemIsSelectable);
    setPen(QPen(Qt::cyan, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    setZValue(-1);
    if (m_startPort && m_endPort) {
        updatePath();
    }
}

Connection::~Connection()
{
    breakConnection();
}

void Connection::setTempEnd(const QPointF &pos)
{
    m_isTemp = true;
    m_tempEnd = pos;
    updatePath();
}

void Connection::updatePath()
{
    if (!m_startPort) return;

    QPointF startPos = m_startPort->scenePos();
    QPointF endPos;
    Port *endPort = nullptr;
    if (m_isTemp) {
        endPos = m_tempEnd;
    } else {
        endPort = m_endPort;
        endPos = m_endPort ? m_endPort->scenePos() : startPos;
    }

    if (s_defaultPathType == Bezier) {
        updateBezierPath(startPos, endPos);
    } else {
        updateOrthogonalPath(startPos, endPos, m_startPort, endPort);
    }
}

void Connection::updateBezierPath(const QPointF &start, const QPointF &end)
{
    QPainterPath path;
    path.moveTo(start);
    qreal dx = end.x() - start.x();
    qreal ctrlOffset = std::max(std::abs(dx) * 0.5, 50.0);
    QPointF ctrl1 = start + QPointF(ctrlOffset, 0);
    QPointF ctrl2 = end - QPointF(ctrlOffset, 0);
    path.cubicTo(ctrl1, ctrl2, end);
    setPath(path);
}

void Connection::updateOrthogonalPath(const QPointF &start, const QPointF &end, Port *startPort, Port *endPort)
{
    QPainterPath path;
    path.moveTo(start);

    // 确定起始方向：输出端口向右，输入端口向左（根据端口所在节点侧）
    bool startIsOutput = (startPort->portType() == Port::OutputPort);
    bool endIsInput = (endPort && endPort->portType() == Port::InputPort) || (!endPort); // 临时连线假设终点为输入

    qreal midX;
    if (startIsOutput) {
        // 从输出端口出发，先向右延伸一段
        midX = start.x() + 40;
        path.lineTo(midX, start.y());
    } else {
        // 输入端口作为起点（极少见），先向左
        midX = start.x() - 40;
        path.lineTo(midX, start.y());
    }

    // 垂直移动到终点Y坐标
    path.lineTo(midX, end.y());

    // 水平移动到终点
    if (endIsInput) {
        // 终点是输入端口，从左侧进入
        path.lineTo(end.x() - 10, end.y()); // 留一点间隙
    } else {
        path.lineTo(end.x(), end.y());
    }

    setPath(path);
}

void Connection::breakConnection()
{
    if (m_startPort) {
        m_startPort->removeConnection(this);
        m_startPort = nullptr;
    }
    if (m_endPort) {
        m_endPort->removeConnection(this);
        m_endPort = nullptr;
    }
    if (scene())
        scene()->removeItem(this);
}

void Connection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (isSelected()) {
        QPen selectedPen = pen();
        selectedPen.setColor(Qt::yellow);
        selectedPen.setWidth(3);
        painter->setPen(selectedPen);
    } else {
        painter->setPen(pen());
    }
    painter->drawPath(path());
}

void Connection::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    deleteSelf();
    QGraphicsPathItem::mouseDoubleClickEvent(event);
}

void Connection::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *deleteAction = menu.addAction("Delete Connection");
    QAction *selected = menu.exec(event->screenPos());
    if (selected == deleteAction) {
        deleteSelf();
    }
}

void Connection::deleteSelf()
{
    EditorScene *scene = qobject_cast<EditorScene*>(this->scene());
    if (scene && scene->undoStack()) {
        scene->undoStack()->push(new RemoveConnectionCommand(scene, this));
    } else {
        breakConnection();
        delete this;
    }
}
