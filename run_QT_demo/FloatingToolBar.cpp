#include "FloatingToolBar.h"
#include "EditorView.h"
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include <QPainter>
#include <QDebug>

FloatingToolBar::FloatingToolBar(EditorView *editorView, QWidget *parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , m_editorView(editorView)
{
    setWindowTitle("Node Toolbar");
    setFixedSize(120, 180);
    setStyleSheet("background-color: #2b2b2b; border: 1px solid #555;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    m_defaultNodeBtn = new QPushButton("Default Node", this);
    m_mathNodeBtn = new QPushButton("Math Node", this);

    QString btnStyle =
        "QPushButton { background-color: #3c3c3c; color: white; padding: 8px; border-radius: 3px; min-width: 100px; min-height: 30px; }"
        "QPushButton:hover { background-color: #505050; }";
    m_defaultNodeBtn->setStyleSheet(btnStyle);
    m_mathNodeBtn->setStyleSheet(btnStyle);

    layout->addWidget(m_defaultNodeBtn);
    layout->addWidget(m_mathNodeBtn);
    layout->addStretch();

    // 安装事件过滤器捕获按钮的鼠标按下以启动拖放创建节点
    m_defaultNodeBtn->installEventFilter(this);
    m_mathNodeBtn->installEventFilter(this);

    // 初始位置设置
    if (m_editorView) {
        QPoint globalPos = m_editorView->mapToGlobal(QPoint(10, 50));
        move(globalPos);
    }

    // 设置鼠标追踪以接收移动事件（即使没有按下按钮）
    setMouseTracking(true);
}

bool FloatingToolBar::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            QPushButton *btn = qobject_cast<QPushButton*>(watched);
            if (btn == m_defaultNodeBtn) {
                startDrag("Default Node");
                return true;
            } else if (btn == m_mathNodeBtn) {
                startDrag("Math Node");
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void FloatingToolBar::startDrag(const QString &nodeType)
{
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-node-type", nodeType.toUtf8());
    drag->setMimeData(mimeData);

    QPixmap pixmap(60, 40);
    pixmap.fill(QColor(100, 100, 150));
    QPainter painter(&pixmap);
    painter.setPen(Qt::white);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, nodeType);
    drag->setPixmap(pixmap);
    drag->setHotSpot(QPoint(30, 20));

    drag->exec(Qt::CopyAction);
}

// ---------- 窗口拖动实现 ----------
void FloatingToolBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_draggingWindow = true;
        m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
    QWidget::mousePressEvent(event);
}

void FloatingToolBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_draggingWindow && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - m_dragStartPosition);
        event->accept();
    }
    QWidget::mouseMoveEvent(event);
}

void FloatingToolBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_draggingWindow = false;
        event->accept();
    }
    QWidget::mouseReleaseEvent(event);
}
