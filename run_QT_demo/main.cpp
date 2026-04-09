#include <QApplication>
#include "EditorView.h"
#include "FloatingToolBar.h"
#include "EditorScene.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    EditorView view;
    view.setWindowTitle("Node Editor Demo");
    view.resize(800, 600);
    view.show();

    // 获取场景并设置撤销栈（已在EditorScene构造中创建）
    EditorScene *scene = qobject_cast<EditorScene*>(view.scene());
    // 可选：将撤销栈暴露给外部以便创建撤销视图
    // QUndoView *undoView = new QUndoView(scene->undoStack());

    FloatingToolBar *toolBar = new FloatingToolBar(&view);
    toolBar->show();

    return a.exec();
}
