#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <QPointF>
#include "Node.h"
#include "Port.h"
#include "Connection.h"

class EditorScene;

// 添加节点命令
class AddNodeCommand : public QUndoCommand
{
public:
    AddNodeCommand(EditorScene *scene, const QString &type, const QPointF &pos);
    void undo() override;
    void redo() override;

private:
    EditorScene *m_scene;
    QString m_type;
    QPointF m_pos;
    Node *m_node = nullptr;
};

// 删除节点命令
class RemoveNodeCommand : public QUndoCommand
{
public:
    RemoveNodeCommand(EditorScene *scene, Node *node);
    void undo() override;
    void redo() override;

private:
    EditorScene *m_scene;
    Node *m_node;
    QList<Connection*> m_removedConnections;
    QPointF m_pos;
};

// 移动节点命令（用于移动后一次性提交）
class MoveNodeCommand : public QUndoCommand
{
public:
    MoveNodeCommand(Node *node, const QPointF &oldPos, const QPointF &newPos);
    void undo() override;
    void redo() override;
    int id() const override { return 1001; }
    bool mergeWith(const QUndoCommand *other) override;

private:
    Node *m_node;
    QPointF m_oldPos;
    QPointF m_newPos;
};

// 添加连线命令
class AddConnectionCommand : public QUndoCommand
{
public:
    AddConnectionCommand(EditorScene *scene, Port *start, Port *end);
    void undo() override;
    void redo() override;

private:
    EditorScene *m_scene;
    Port *m_startPort;
    Port *m_endPort;
    Connection *m_connection = nullptr;
};

// 删除连线命令
class RemoveConnectionCommand : public QUndoCommand
{
public:
    RemoveConnectionCommand(EditorScene *scene, Connection *conn);
    void undo() override;
    void redo() override;

private:
    EditorScene *m_scene;
    Connection *m_connection;
    Port *m_startPort;
    Port *m_endPort;
};

// 重命名节点命令
class RenameNodeCommand : public QUndoCommand
{
public:
    RenameNodeCommand(Node *node, const QString &oldTitle, const QString &newTitle);
    void undo() override;
    void redo() override;

private:
    Node *m_node;
    QString m_oldTitle;
    QString m_newTitle;
};

// 修改节点参数命令（示例：整数参数）
class SetNodeParamCommand : public QUndoCommand
{
public:
    SetNodeParamCommand(Node *node, int oldValue, int newValue);
    void undo() override;
    void redo() override;

private:
    Node *m_node;
    int m_oldValue;
    int m_newValue;
};

// 添加端口命令
class AddPortCommand : public QUndoCommand
{
public:
    AddPortCommand(Node *node, Port::PortType type, const QString &name);
    void undo() override;
    void redo() override;

private:
    Node *m_node;
    Port *m_port = nullptr;
    Port::PortType m_type;
    QString m_name;
};

// 删除端口命令
class RemovePortCommand : public QUndoCommand
{
public:
    RemovePortCommand(Node *node, Port *port);
    void undo() override;
    void redo() override;

private:
    Node *m_node;
    Port *m_port;
    QList<Connection*> m_removedConnections;
};

#endif // COMMANDS_H
