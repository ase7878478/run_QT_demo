#include "Node.h"
#include "Connection.h"
#include "Commands.h"
#include "EditorScene.h"
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QInputDialog>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QDebug>

Node::Node(const QString &title, QGraphicsItem *parent)
    : QGraphicsRectItem(parent), m_title(title)
{
    setRect(0, 0, 150, 80);
    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);

    m_titleItem = new QGraphicsTextItem(this);
    m_titleItem->setPlainText(title);
    m_titleItem->setDefaultTextColor(Qt::white);
    QFont font = m_titleItem->font();
    font.setBold(true);
    m_titleItem->setFont(font);
    m_titleItem->setPos(5, 5);
}

Node::~Node()
{
    qDeleteAll(m_ports);
}

void Node::addPort(Port::PortType type, const QString &name)
{
    Port *port = new Port(type, this);
    port->setPortName(name.isEmpty() ? (type == Port::InputPort ? "In" : "Out") : name);
    m_ports.append(port);
    updatePortPositions();
}

void Node::addPort(Port *port)
{
    if (!m_ports.contains(port)) {
        port->setParentItem(this);
        m_ports.append(port);
        updatePortPositions();
    }
}

void Node::removePort(Port *port)
{
    if (m_ports.removeOne(port)) {
        updatePortPositions();
    }
}

void Node::setTitle(const QString &title)
{
    m_title = title;
    m_titleItem->setPlainText(title);
}

void Node::setIntParam(int value)
{
    m_intParam = value;
    update(); // 可触发重绘显示参数
}

void Node::updatePortPositions()
{
    qreal yStep = rect().height() / (m_ports.size() + 1);
    qreal yOffset = yStep;
    for (Port *port : m_ports) {
        qreal x = (port->portType() == Port::InputPort) ? 0 : rect().width();
        port->setPos(x, yOffset);
        yOffset += yStep;
    }
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged) {
        for (Port *port : m_ports) {
            for (Connection *conn : port->connections()) {
                conn->updatePath();
            }
        }
    }
    return QGraphicsRectItem::itemChange(change, value);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(QPen(isSelected() ? Qt::yellow : Qt::lightGray, 2));
    painter->setBrush(QBrush(QColor(60, 60, 70)));
    painter->drawRoundedRect(rect(), 5, 5);

    painter->setPen(QPen(Qt::darkGray, 1));
    painter->drawLine(0, 25, rect().width(), 25);

    // 显示参数值
    painter->setPen(Qt::white);
    painter->drawText(10, 45, QString("Value: %1").arg(m_intParam));
}

void Node::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *addInputAction = menu.addAction("Add Input Port");
    QAction *addOutputAction = menu.addAction("Add Output Port");
    menu.addSeparator();
    QAction *renameAction = menu.addAction("Rename");
    QAction *editParamAction = menu.addAction("Edit Parameter...");

    Port *selectedPort = nullptr;
    for (Port *port : m_ports) {
        if (port->isSelected()) {
            selectedPort = port;
            break;
        }
    }
    QAction *removePortAction = nullptr;
    if (selectedPort) {
        removePortAction = menu.addAction("Remove Selected Port");
    }

    QAction *selectedAction = menu.exec(event->screenPos());

    EditorScene *scene = qobject_cast<EditorScene*>(this->scene());
    QUndoStack *undoStack = scene ? scene->undoStack() : nullptr;

    if (selectedAction == addInputAction) {
        bool ok;
        QString name = QInputDialog::getText(nullptr, "Port Name", "Name:", QLineEdit::Normal, "In", &ok);
        if (ok && !name.isEmpty()) {
            if (undoStack) {
                undoStack->push(new AddPortCommand(this, Port::InputPort, name));
            } else {
                addPort(Port::InputPort, name);
            }
        }
    } else if (selectedAction == addOutputAction) {
        bool ok;
        QString name = QInputDialog::getText(nullptr, "Port Name", "Name:", QLineEdit::Normal, "Out", &ok);
        if (ok && !name.isEmpty()) {
            if (undoStack) {
                undoStack->push(new AddPortCommand(this, Port::OutputPort, name));
            } else {
                addPort(Port::OutputPort, name);
            }
        }
    } else if (selectedAction == renameAction) {
        bool ok;
        QString newTitle = QInputDialog::getText(nullptr, "Rename Node", "Title:", QLineEdit::Normal, m_title, &ok);
        if (ok && !newTitle.isEmpty()) {
            if (undoStack) {
                undoStack->push(new RenameNodeCommand(this, m_title, newTitle));
            } else {
                setTitle(newTitle);
            }
        }
    } else if (selectedAction == editParamAction) {
        editProperties();
    } else if (selectedAction == removePortAction && selectedPort) {
        if (undoStack) {
            undoStack->push(new RemovePortCommand(this, selectedPort));
        } else {
            removePort(selectedPort);
            delete selectedPort;
        }
    }
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    editProperties();
    QGraphicsRectItem::mouseDoubleClickEvent(event);
}

void Node::editProperties()
{
    QDialog dialog;
    dialog.setWindowTitle("Edit Node Properties");
    QFormLayout form(&dialog);

    QLineEdit titleEdit(m_title);
    form.addRow("Title:", &titleEdit);

    QSpinBox paramSpin;
    paramSpin.setRange(-1000, 1000);
    paramSpin.setValue(m_intParam);
    form.addRow("Parameter:", &paramSpin);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString newTitle = titleEdit.text();
        int newParam = paramSpin.value();

        EditorScene *scene = qobject_cast<EditorScene*>(this->scene());
        QUndoStack *undoStack = scene ? scene->undoStack() : nullptr;
        if (undoStack) {
            undoStack->beginMacro("Edit Node");
            if (newTitle != m_title) {
                undoStack->push(new RenameNodeCommand(this, m_title, newTitle));
            }
            if (newParam != m_intParam) {
                undoStack->push(new SetNodeParamCommand(this, m_intParam, newParam));
            }
            undoStack->endMacro();
        } else {
            setTitle(newTitle);
            setIntParam(newParam);
        }
    }
}
