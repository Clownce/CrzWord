#ifndef MYWORD_H
#define MYWORD_H
#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QMdiArea>
#include <QComboBox>
#include <QFontComboBox>
#include <QMdiSubWindow>
#include <QSignalMapper>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintPreviewDialog>
#include <QtPrintSupport/QPrintDialog>
#include <QTranslator>
#include "mychild.h"



class MyWord : public QMainWindow
{
    Q_OBJECT

public:
    MyWord(QWidget *parent = 0);
    ~MyWord();
private:
    //菜单
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *formatMenu;
    QMenu *fontMenu;
    QMenu *alignMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QMenu *langMenu;
    //工具栏
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *formatToolBar;
    QToolBar *comboToolBar;
    QComboBox *comboStyle;
    QFontComboBox *comboFont;
    QComboBox *comboSize;
    //动作
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *printAct;
    QAction *printPreviewAct;
    QAction *exitAct;

    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

    QAction *boldAct;
    QAction *italicAct;
    QAction *underlineAct;
    QAction *leftAlignAct;
    QAction *centerAct;
    QAction *rightAlignAct;
    QAction *justifyAct;
    QAction *colorAct;

    QAction *closeAct;
    QAction *closeAllAct;
    QAction *tileAct;
    QAction *cascadeAct;
    QAction *nextAct;
    QAction *previousAct;
    QAction *separatorAct;

    QAction *transEnAct;
    QAction *transZhAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    MyChild *activeMyChild();
    QMdiSubWindow *findMyChild(const QString &fileName);
    QMdiArea *mdiArea;
    QSignalMapper *windowMapper;
    QTranslator *translator;
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void enabledText();
    void langUpdate();
    void fontChanged(const QFont &f);
    void colorChanged(const QColor &c);
    void alignmentChanged(Qt::Alignment a);

protected:
    void closeEvent(QCloseEvent *);
private slots:
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void filePrint();
    void filePrintPreview();
    void printPreview(QPrinter *);
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void about();
    void transEn();
    void transZh();
    void textBold();
    void textItalic();
    void textUnderline();
    void textAlign(QAction *a);
    void textStyle(int styleIndex);
    void textFamily(const QString &f);
    void textSize(const QString &p);
    void textColor();
    void updateMenus();
    void updateWindowMenu();
    MyChild *createMyChild();
    void setActiveSubWindow(QWidget *window);
};

#endif // MYWORD_H
