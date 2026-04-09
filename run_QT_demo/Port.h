#ifndef PORT_H
#define PORT_H

#include <QGraphicsEllipseItem>
#include <QList>

class Node;
class Connection;

class Port : public QGraphicsEllipseItem
{
public:
    enum PortType { InputPort, OutputPort };

    Port(PortType type, Node *parentNode);
    ~Port();

    PortType portType() const { return m_type; }
    Node* parentNode() const { return m_parentNode; }
    void setPortName(const QString &name) { m_name = name; }
    QString portName() const { return m_name; }

    void addConnection(Connection *conn);
    void removeConnection(Connection *conn);
    QList<Connection*> connections() const { return m_connections; }

    bool canConnectTo(Port *other) const;

    // 扩大点击区域
    QPainterPath shape() const override;
    QRectF boundingRect() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    PortType m_type;
    Node *m_parentNode;
    QString m_name;
    QList<Connection*> m_connections;
    bool m_hovered = false;
};

#endif // PORT_H
