#ifndef CONNECTION_H
#define CONNECTION_H

#include <QGraphicsPathItem>

class Port;

class Connection : public QGraphicsPathItem
{
public:
    enum PathType { Bezier, Orthogonal }; // 连线类型

    Connection(Port *startPort, Port *endPort, QGraphicsItem *parent = nullptr);
    ~Connection();

    Port* startPort() const { return m_startPort; }
    Port* endPort() const { return m_endPort; }

    void setTempEnd(const QPointF &pos);
    void updatePath();
    void breakConnection();

    static void setDefaultPathType(PathType type) { s_defaultPathType = type; }
    static PathType defaultPathType() { return s_defaultPathType; }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    Port *m_startPort = nullptr;
    Port *m_endPort = nullptr;
    QPointF m_tempEnd;
    bool m_isTemp = false;

    static PathType s_defaultPathType; // 默认路径类型

    void updateBezierPath(const QPointF &start, const QPointF &end);
    void updateOrthogonalPath(const QPointF &start, const QPointF &end, Port *startPort, Port *endPort = nullptr);
    void deleteSelf();
};

#endif // CONNECTION_H
