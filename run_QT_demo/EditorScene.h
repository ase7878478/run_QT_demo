#ifndef EDITORSCENE_H
#define EDITORSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QUndoStack>
#include "Port.h"

class Node;
class Connection;
class QUndoStack;

class EditorScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit EditorScene(QObject *parent = nullptr);

    void createNodeAt(const QPointF &scenePos, const QString &nodeType = "Default");
    void setUndoStack(QUndoStack *stack) { m_undoStack = stack; }
    QUndoStack* undoStack() const { return m_undoStack; }

    // 复制粘贴
    void copySelectedNodes();
    void pasteNodes();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QUndoStack *m_undoStack = nullptr;
    Connection *m_tempConnection = nullptr;
    Port *m_startPort = nullptr;
    QPointF m_tempEndPos;

    // 用于记录节点移动起始位置，以便提交移动命令
    QMap<Node*, QPointF> m_movingNodesStartPos;

    Port* findPortAt(const QPointF &scenePos) const;
    void deleteSelectedItems();

signals:
    void selectionChanged();
};

#endif // EDITORSCENE_H
