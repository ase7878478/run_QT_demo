#include "Commands.h"
#include "EditorScene.h"

// ---------- AddNodeCommand ----------
AddNodeCommand::AddNodeCommand(EditorScene *scene, const QString &type, const QPointF &pos)
    : m_scene(scene), m_type(type), m_pos(pos)
{
    setText("Add Node");
}

void AddNodeCommand::undo()
{
    if (m_node) {
        m_scene->removeItem(m_node);
        delete m_node;
        m_node = nullptr;
    }
}

void AddNodeCommand::redo()
{
    if (!m_node) {
        m_node = new Node(m_type);
        m_scene->addItem(m_node);
        m_node->setPos(m_pos);
        // 默认添加两个端口
        m_node->addPort(Port::InputPort, "In");
        m_node->addPort(Port::OutputPort, "Out");
    } else {
        m_scene->addItem(m_node);
    }
}

// ---------- RemoveNodeCommand ----------
RemoveNodeCommand::RemoveNodeCommand(EditorScene *scene, Node *node)
    : m_scene(scene), m_node(node), m_pos(node->pos())
{
    setText("Remove Node");
    // 收集与该节点端口关联的所有连线
    for (Port *port : node->ports()) {
        for (Connection *conn : port->connections()) {
            if (!m_removedConnections.contains(conn))
                m_removedConnections.append(conn);
        }
    }
}

void RemoveNodeCommand::undo()
{
    m_scene->addItem(m_node);
    m_node->setPos(m_pos);
    for (Connection *conn : m_removedConnections) {
        m_scene->addItem(conn);
        conn->startPort()->addConnection(conn);
        conn->endPort()->addConnection(conn);
    }
}

void RemoveNodeCommand::redo()
{
    // 删除节点前先移除其所有连线
    for (Connection *conn : m_removedConnections) {
        m_scene->removeItem(conn);
        conn->startPort()->removeConnection(conn);
        conn->endPort()->removeConnection(conn);
    }
    m_scene->removeItem(m_node);
}

// ---------- MoveNodeCommand ----------
MoveNodeCommand::MoveNodeCommand(Node *node, const QPointF &oldPos, const QPointF &newPos)
    : m_node(node), m_oldPos(oldPos), m_newPos(newPos)
{
    setText("Move Node");
}

void MoveNodeCommand::undo()
{
    m_node->setPos(m_oldPos);
}

void MoveNodeCommand::redo()
{
    m_node->setPos(m_newPos);
}

bool MoveNodeCommand::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id()) return false;
    const MoveNodeCommand *cmd = static_cast<const MoveNodeCommand*>(other);
    if (cmd->m_node != m_node) return false;
    m_newPos = cmd->m_newPos;
    return true;
}

// ---------- AddConnectionCommand ----------
AddConnectionCommand::AddConnectionCommand(EditorScene *scene, Port *start, Port *end)
    : m_scene(scene), m_startPort(start), m_endPort(end)
{
    setText("Add Connection");
}

void AddConnectionCommand::undo()
{
    if (m_connection) {
        m_scene->removeItem(m_connection);
        m_startPort->removeConnection(m_connection);
        m_endPort->removeConnection(m_connection);
        delete m_connection;
        m_connection = nullptr;
    }
}

void AddConnectionCommand::redo()
{
    if (!m_connection) {
        m_connection = new Connection(m_startPort, m_endPort);
        m_scene->addItem(m_connection);
        m_startPort->addConnection(m_connection);
        m_endPort->addConnection(m_connection);
        m_connection->updatePath();
    } else {
        m_scene->addItem(m_connection);
        m_startPort->addConnection(m_connection);
        m_endPort->addConnection(m_connection);
    }
}

// ---------- RemoveConnectionCommand ----------
RemoveConnectionCommand::RemoveConnectionCommand(EditorScene *scene, Connection *conn)
    : m_scene(scene), m_connection(conn), m_startPort(conn->startPort()), m_endPort(conn->endPort())
{
    setText("Remove Connection");
}

void RemoveConnectionCommand::undo()
{
    m_scene->addItem(m_connection);
    m_startPort->addConnection(m_connection);
    m_endPort->addConnection(m_connection);
    m_connection->updatePath();
}

void RemoveConnectionCommand::redo()
{
    m_scene->removeItem(m_connection);
    m_startPort->removeConnection(m_connection);
    m_endPort->removeConnection(m_connection);
}

// ---------- RenameNodeCommand ----------
RenameNodeCommand::RenameNodeCommand(Node *node, const QString &oldTitle, const QString &newTitle)
    : m_node(node), m_oldTitle(oldTitle), m_newTitle(newTitle)
{
    setText("Rename Node");
}

void RenameNodeCommand::undo()
{
    m_node->setTitle(m_oldTitle);
}

void RenameNodeCommand::redo()
{
    m_node->setTitle(m_newTitle);
}

// ---------- SetNodeParamCommand ----------
SetNodeParamCommand::SetNodeParamCommand(Node *node, int oldValue, int newValue)
    : m_node(node), m_oldValue(oldValue), m_newValue(newValue)
{
    setText("Change Node Parameter");
}

void SetNodeParamCommand::undo()
{
    m_node->setIntParam(m_oldValue);
}

void SetNodeParamCommand::redo()
{
    m_node->setIntParam(m_newValue);
}

// ---------- AddPortCommand ----------
AddPortCommand::AddPortCommand(Node *node, Port::PortType type, const QString &name)
    : m_node(node), m_type(type), m_name(name)
{
    setText("Add Port");
}

void AddPortCommand::undo()
{
    if (m_port) {
        m_node->removePort(m_port);
        delete m_port;
        m_port = nullptr;
    }
}

void AddPortCommand::redo()
{
    if (!m_port) {
        m_port = new Port(m_type, m_node);
        m_port->setPortName(m_name);
    }
    m_node->addPort(m_port);
}

// ---------- RemovePortCommand ----------
RemovePortCommand::RemovePortCommand(Node *node, Port *port)
    : m_node(node), m_port(port)
{
    setText("Remove Port");
    for (Connection *conn : port->connections()) {
        if (!m_removedConnections.contains(conn))
            m_removedConnections.append(conn);
    }
}

void RemovePortCommand::undo()
{
    m_node->addPort(m_port);
    for (Connection *conn : m_removedConnections) {
        conn->startPort()->addConnection(conn);
        conn->endPort()->addConnection(conn);
        if (conn->scene() == nullptr)
            m_node->scene()->addItem(conn);
        conn->updatePath();
    }
}

void RemovePortCommand::redo()
{
    // 先移除所有连线
    for (Connection *conn : m_removedConnections) {
        conn->startPort()->removeConnection(conn);
        conn->endPort()->removeConnection(conn);
        if (conn->scene())
            conn->scene()->removeItem(conn);
    }
    m_node->removePort(m_port);
}
