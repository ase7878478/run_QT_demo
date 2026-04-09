#ifndef EDITORVIEW_H
#define EDITORVIEW_H

#include <QGraphicsView>
#include "EditorScene.h"

class EditorView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit EditorView(QWidget *parent = nullptr);

    // 供外部调用，在指定视图坐标创建节点
    void createNodeAtViewPos(const QPoint &viewPos, const QString &nodeType = "Default");

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
    EditorScene *m_scene;
};

#endif // EDITORVIEW_H
