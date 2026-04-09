#ifndef FLOATINGTOOLBAR_H
#define FLOATINGTOOLBAR_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class EditorView;

class FloatingToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit FloatingToolBar(EditorView *editorView, QWidget *parent = nullptr);
    void setEditorView(EditorView *view) { m_editorView = view; }

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

    // 窗口拖动相关
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    EditorView *m_editorView = nullptr;
    QPushButton *m_defaultNodeBtn;
    QPushButton *m_mathNodeBtn;

    // 拖动状态
    bool m_draggingWindow = false;
    QPoint m_dragStartPosition;

    void startDrag(const QString &nodeType);
};

#endif // FLOATINGTOOLBAR_H
