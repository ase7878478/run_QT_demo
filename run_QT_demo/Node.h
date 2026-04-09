#ifndef NODE_H
#define NODE_H

#include <QGraphicsRectItem>
#include <QList>
#include "Port.h"

class Node : public QGraphicsRectItem
{
public:
    explicit Node(const QString &title, QGraphicsItem *parent = nullptr);
    ~Node();

    void addPort(Port::PortType type, const QString &name = QString());
    void addPort(Port *port); // 用于命令
    void removePort(Port *port);
    QList<Port*> ports() const { return m_ports; }

    void setTitle(const QString &title);
    QString title() const { return m_title; }

    // 示例参数：一个整数
    void setIntParam(int value);
    int intParam() const { return m_intParam; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString m_title;
    QList<Port*> m_ports;
    QGraphicsTextItem *m_titleItem = nullptr;
    int m_intParam = 0; // 示例参数

    void updatePortPositions();
    void editProperties();
};

#endif // NODE_H
