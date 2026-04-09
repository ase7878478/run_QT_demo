#include "Port.h"
#include "Node.h"
#include "Connection.h"
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QDebug>

Port::Port(PortType type, Node *parentNode)
    : QGraphicsEllipseItem(parentNode), m_type(type), m_parentNode(parentNode)
{
    setRect(-6, -6, 12, 12);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);

    setBrush(m_type == InputPort ? QColor(100, 200, 100) : QColor(200, 100, 100));
    setPen(QPen(Qt::white, 2));
}

Port::~Port()
{
    for (Connection *conn : m_connections) {
        conn->breakConnection();
    }
}

void Port::addConnection(Connection *conn)
{
    if (!m_connections.contains(conn))
        m_connections.append(conn);
}

void Port::removeConnection(Connection *conn)
{
    m_connections.removeOne(conn);
}

bool Port::canConnectTo(Port *other) const
{
    if (!other) return false;
    if (other == this) return false;
    if (other->parentNode() == this->parentNode()) return false;
    if (other->portType() == this->portType()) return false;
    return true;
}

QPainterPath Port::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect()); // 使用更大的 boundingRect
    return path;
}

QRectF Port::boundingRect() const
{
    // 扩大点击区域：半径10像素的圆
    return QRectF(-10, -10, 20, 20);
}

void Port::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // 绘制实际端口圆点（仍为6像素半径）
    painter->setBrush(brush());
    if (m_hovered) {
        painter->setPen(QPen(Qt::yellow, 2));
    } else {
        painter->setPen(pen());
    }
    painter->drawEllipse(-6, -6, 12, 12);

    // 绘制端口名称（如果需要）
    if (!m_name.isEmpty()) {
        painter->setPen(Qt::white);
        painter->drawText(rect().right() + 3, rect().center().y() + 4, m_name);
    }
}

void Port::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_hovered = true;
    update();
    QGraphicsEllipseItem::hoverEnterEvent(event);
}

void Port::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_hovered = false;
    update();
    QGraphicsEllipseItem::hoverLeaveEvent(event);
}
