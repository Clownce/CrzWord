#include "myword.h"
#include <QIcon>
#include <QKeySequence>
#include <QToolTip>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QFileInfo>
#include <QFileDialog>
#include <QColorDialog>
#include <QMessageBox>
#define USE_CHINESE QStringLiteral


MyWord::MyWord(QWidget *parent)
    : QMainWindow(parent)
{
    mdiArea=new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    connect(mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(updateMenus()));
    windowMapper=new QSignalMapper(this);
    connect(windowMapper,SIGNAL(mapped(QWidget*)),this,SLOT(setActiveSubWindow(QWidget*)));

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    langUpdate();
    updateMenus();
    transEnAct->setDisabled(true);

    move(200,150);
    resize(800,500);

    setWindowTitle(tr("CrzWord"));
    setUnifiedTitleAndToolBarOnMac(true);
    setWindowIcon(QIcon(":/images/app.png"));
}

MyWord::~MyWord()
{
}
void MyWord::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if(mdiArea->currentSubWindow())
        event->ignore();
    else
        event->accept();
}
void MyWord::updateMenus()
{
    bool hasMyChild=(activeMyChild()!=0);
    saveAct->setEnabled(hasMyChild);
    saveAsAct->setEnabled(hasMyChild);
    printAct->setEnabled(hasMyChild);
    printPreviewAct->setEnabled(hasMyChild);
    pasteAct->setEnabled(hasMyChild);
    closeAct->setEnabled(hasMyChild);
    closeAllAct->setEnabled(hasMyChild);
    tileAct->setEnabled(hasMyChild);
    cascadeAct->setEnabled(hasMyChild);
    nextAct->setEnabled(hasMyChild);
    previousAct->setEnabled(hasMyChild);
    separatorAct->setVisible(hasMyChild);

    bool hasSelection=(activeMyChild()&&activeMyChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
    boldAct->setEnabled(hasSelection);
    italicAct->setEnabled(hasSelection);
    underlineAct->setEnabled(hasSelection);
    leftAlignAct->setEnabled(hasSelection);
    centerAct->setEnabled(hasSelection);
    rightAlignAct->setEnabled(hasSelection);
    justifyAct->setEnabled(hasSelection);
    colorAct->setEnabled(hasSelection);
}
void MyWord::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addSeparator();

    QList<QMdiSubWindow *> windows=mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());
    for(int i=0;i<windows.size();i++)
    {
        MyChild *child=qobject_cast<MyChild *>(windows.at(i)->widget());
        QString text;
        if(i<9)
            text=tr("&%1 %2").arg(i+1).arg(child->userFriendlyCurrentFile());
        else
            text=tr("%1 %2").arg(i+1).arg(child->userFriendlyCurrentFile());
        QAction *action=windowMenu->addAction(text);
        action->setCheckable(true);
        action->setChecked(child==activeMyChild());
        connect(action,SIGNAL(triggered()),windowMapper,SLOT(map()));
        windowMapper->setMapping(action,windows.at(i));
    }
    enabledText();
}
MyChild *MyWord::createMyChild()
{
    MyChild *child=new MyChild;
    mdiArea->addSubWindow(child);
    connect(child,SIGNAL(copyAvailable(bool)),cutAct,SLOT(setEnabled(bool)));
    connect(child,SIGNAL(copyAvailable(bool)),copyAct,SLOT(setEnabled(bool)));
    return child;
}

void MyWord::createActions()
{
    //文件菜单
    newAct=new QAction(QIcon(":/images/filenew.png"),tr("New(&N)"),this);
    newAct->setShortcut(QKeySequence::New);
    newAct->setToolTip(tr("New"));
    newAct->setStatusTip(tr("Create a new document"));
    connect(newAct,SIGNAL(triggered()),this,SLOT(fileNew()));

    openAct = new QAction(QIcon(":/images/fileopen.png"), tr("Open(&O)..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setToolTip(tr("Open"));
    openAct->setStatusTip(tr("Open an existing document"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(fileOpen()));

    saveAct=new QAction(QIcon(":/images/filesave.png"),tr("Save(&S)"),this);
    saveAct->setShortcut(QKeySequence::Save);
    saveAct->setToolTip(tr("Save"));
    saveAct->setStatusTip(tr("Save current document"));
    connect(saveAct,SIGNAL(triggered()),this,SLOT(fileSave()));

    saveAsAct=new QAction(tr("Save As(&A)..."),this);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setToolTip(tr("Save As"));
    saveAsAct->setStatusTip(tr("Save as a new document"));
    connect(saveAsAct,SIGNAL(triggered()),this,SLOT(fileSaveAs()));

    printAct=new QAction(QIcon(":/images/fileprint.png"),tr("Print(&P)"),this);
    printAct->setShortcut(QKeySequence::Print);
    printAct->setToolTip(tr("Print"));
    printAct->setStatusTip(tr("Print this document"));
    connect(printAct,SIGNAL(triggered()),this,SLOT(filePrint()));

    printPreviewAct=new QAction(tr("Print Preview..."),this);
    printPreviewAct->setToolTip(tr("Print Preview"));
    printPreviewAct->setStatusTip(tr("Preview the print effert"));
    connect(printPreviewAct,SIGNAL(triggered()),this,SLOT(filePrintPreview()));

    exitAct=new QAction(tr("Exit(&X)"),this);
    exitAct->setShortcut(QKeySequence::Quit);
    exitAct->setToolTip(tr("Exit"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct,SIGNAL(triggered()),qApp,SLOT(closeAllWindows()));

    //编辑菜单
    undoAct=new QAction(QIcon(":/images/editundo.png"),tr("Undo(&U)"),this);
    undoAct->setShortcut(QKeySequence::Undo);
    undoAct->setToolTip(tr("Undo"));
    undoAct->setStatusTip(tr("Undo the current operation"));
    connect(undoAct,SIGNAL(triggered()),this,SLOT(undo()));

    redoAct=new QAction(QIcon(":/images/editredo.png"),tr("Redo(&R)"),this);
    redoAct->setShortcut(QKeySequence::Redo);
    redoAct->setToolTip(tr("Redo"));
    redoAct->setStatusTip(tr("Redo the last operation"));
    connect(redoAct,SIGNAL(triggered()),this,SLOT(redo()));

    cutAct=new QAction(QIcon(":/images/editcut.png"),tr("Cut(&T)"),this);
    cutAct->setShortcut(QKeySequence::Cut);
    cutAct->setToolTip(tr("Cut"));
    cutAct->setStatusTip(tr("Cut the selected content and put it into the clipboard"));
    connect(cutAct,SIGNAL(triggered()),this,SLOT(cut()));

    copyAct=new QAction(QIcon(":/images/editcopy.png"),tr("Copy(&C)"),this);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setToolTip(tr("Copy"));
    copyAct->setStatusTip(tr("Copy the selected content and put it into the clipboard"));
    connect(copyAct,SIGNAL(triggered()),this,SLOT(copy()));

    pasteAct=new QAction(QIcon(":/images/editpaste.png"),tr("Paste(&P)"),this);
    pasteAct->setShortcut(QKeySequence::Paste);
    pasteAct->setToolTip(tr("Paste"));
    pasteAct->setStatusTip(tr("Paste the content into the document from clipboard"));
    connect(pasteAct,SIGNAL(triggered()),this,SLOT(paste()));

    //格式菜单
    boldAct=new QAction(QIcon(":/images/textbold.png"),tr("Bold(&B)"),this);
    boldAct->setCheckable(true);
    boldAct->setShortcut(Qt::CTRL + Qt::Key_B);
    boldAct->setToolTip(tr("Bold"));
    boldAct->setStatusTip(tr("Bold the selected content"));
    QFont bold;
    bold.setBold(true);
    boldAct->setFont(bold);
    connect(boldAct, SIGNAL(triggered()), this, SLOT(textBold()));

    italicAct=new QAction(QIcon(":/images/textitalic.png"),tr("Italic(&I)"),this);
    italicAct->setCheckable(true);
    italicAct->setShortcut(Qt::CTRL+Qt::Key_I);
    italicAct->setToolTip(tr("Italic"));
    italicAct->setStatusTip(tr("Italic the selected content"));
    QFont italic;
    italic.setItalic(true);
    italicAct->setFont(italic);
    connect(italicAct, SIGNAL(triggered()), this, SLOT(textItalic()));

    underlineAct = new QAction(QIcon(":/images/textunderline.png"),tr("Underline(&U)"), this);
    underlineAct->setCheckable(true);
    underlineAct->setShortcut(Qt::CTRL + Qt::Key_U);
    underlineAct->setToolTip(tr("Underline"));
    underlineAct->setStatusTip(tr("Underline the selected content"));
    QFont underline;
    underline.setUnderline(true);
    underlineAct->setFont(underline);
    connect(underlineAct, SIGNAL(triggered()), this, SLOT(textUnderline()));

    QActionGroup *grp = new QActionGroup(this);
    connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(textAlign(QAction*)));

    if (QApplication::isLeftToRight()) {
        leftAlignAct = new QAction(QIcon(":/images/textleft.png"),tr("Align Left(&L)"), grp);
        centerAct = new QAction(QIcon(":/images/textcenter.png"),tr("Align Center(&E)"), grp);
        rightAlignAct = new QAction(QIcon(":/images/textright.png"),tr("Align Right(&R)"), grp);
    } else {
        rightAlignAct = new QAction(QIcon(":/images/textright.png"),tr("Align Right(&R)"), grp);
        centerAct = new QAction(QIcon(":/images/textcenter.png"),tr("Align Center(&E)"), grp);
        leftAlignAct = new QAction(QIcon(":/images/textleft.png"),tr("Align Left(&L)"), grp);
    }
    justifyAct = new QAction(QIcon(":/images/textjustify.png"),tr("Align Justify(&J)"), grp);

    leftAlignAct->setShortcut(Qt::CTRL + Qt::Key_L);
    leftAlignAct->setCheckable(true);
    leftAlignAct->setToolTip(tr("Align Left"));
    leftAlignAct->setStatusTip(tr("Set the selected content left alignment"));

    centerAct->setShortcut(Qt::CTRL + Qt::Key_E);
    centerAct->setCheckable(true);
    centerAct->setToolTip(tr("Align Center"));
    centerAct->setStatusTip(tr("Set the selected content center alignment"));

    rightAlignAct->setShortcut(Qt::CTRL + Qt::Key_R);
    rightAlignAct->setCheckable(true);
    rightAlignAct->setToolTip(tr("Align Right"));
    rightAlignAct->setStatusTip(tr("Set the selected content right alignment"));

    justifyAct->setShortcut(Qt::CTRL + Qt::Key_J);
    justifyAct->setCheckable(true);
    justifyAct->setToolTip(tr("Align Justify"));
    justifyAct->setStatusTip(tr("Set the selected content justify alignment"));

    QPixmap pix(16, 16);
    pix.fill(Qt::red);
    colorAct = new QAction(pix, tr("Color(&C)..."), this);
    colorAct->setToolTip(tr("Color"));
    colorAct->setStatusTip(tr("Set text color"));
    connect(colorAct, SIGNAL(triggered()), this, SLOT(textColor()));

    //窗口菜单
    closeAct = new QAction(tr("Close(&O)"), this);
    closeAct->setStatusTip(tr("Close activated window"));
    connect(closeAct, SIGNAL(triggered()),mdiArea, SLOT(closeActiveSubWindow()));

    closeAllAct = new QAction(tr("Close All(&A)"), this);
    closeAllAct->setStatusTip(tr("Close all windows"));
    connect(closeAllAct, SIGNAL(triggered()),mdiArea, SLOT(closeAllSubWindows()));

    tileAct = new QAction(tr("Tile(&T)"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("Cascade(&C)"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Next(&X)"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Focus on next window"));
    connect(nextAct, SIGNAL(triggered()),mdiArea, SLOT(activateNextSubWindow()));
    previousAct = new QAction(tr("Previous(&V)"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Focus on previous window"));
    connect(previousAct, SIGNAL(triggered()),mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    //帮助菜单
    aboutAct = new QAction(tr("About(&A)"), this);
    aboutAct->setStatusTip(tr("About CrzWord"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    transEnAct=new QAction(tr("English"),this);
    connect(transEnAct,SIGNAL(triggered()),this,SLOT(transEn()));

    transZhAct=new QAction(tr("Chinese"),this);
    connect(transZhAct,SIGNAL(triggered()),this,SLOT(transZh()));

    aboutQtAct = new QAction(tr("About Qt(&Q)"), this);
    aboutQtAct->setStatusTip(tr("About Qt Library"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}
void MyWord::createMenus()
{
    //文件
    fileMenu=menuBar()->addMenu(tr("File(&F)"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(printAct);
    fileMenu->addAction(printPreviewAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    //编辑
    editMenu = menuBar()->addMenu(tr("Edit(&E)"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    //格式
    formatMenu = menuBar()->addMenu(tr("Format(&O)"));
    fontMenu = formatMenu->addMenu(tr("Font(&D)"));   //【字体】子菜单
    fontMenu->addAction(boldAct);
    fontMenu->addAction(italicAct);
    fontMenu->addAction(underlineAct);
    alignMenu = formatMenu->addMenu(tr("Alignment"));      //【段落】子菜单
    alignMenu->addAction(leftAlignAct);
    alignMenu->addAction(centerAct);
    alignMenu->addAction(rightAlignAct);
    alignMenu->addAction(justifyAct);

    formatMenu->addAction(colorAct);
    //窗口
    windowMenu = menuBar()->addMenu(tr("Window(&W)"));
    updateWindowMenu();
    connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));
    menuBar()->addSeparator();
    //帮助
    helpMenu = menuBar()->addMenu(tr("Help(&H)"));
    langMenu=helpMenu->addMenu(tr("Language(&L)"));
    langMenu->addAction(transEnAct);
    langMenu->addAction(transZhAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}
void MyWord::createToolBars()
{
    //文件
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(printAct);

    //编辑
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(undoAct);
    editToolBar->addAction(redoAct);
    editToolBar->addSeparator();
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    //格式
    formatToolBar = addToolBar(tr("Format"));
    formatToolBar->addAction(boldAct);
    formatToolBar->addAction(italicAct);
    formatToolBar->addAction(underlineAct);
    formatToolBar->addSeparator();
    formatToolBar->addAction(leftAlignAct);
    formatToolBar->addAction(centerAct);
    formatToolBar->addAction(rightAlignAct);
    formatToolBar->addAction(justifyAct);
    formatToolBar->addSeparator();
    formatToolBar->addAction(colorAct);

    //组合
    addToolBarBreak(Qt::TopToolBarArea);    //使这个工具条在界面上另起一行显示
    comboToolBar = addToolBar(tr("Combination"));
    comboStyle = new QComboBox();
    comboToolBar->addWidget(comboStyle);
    comboStyle->addItem(tr("Normal"));
    comboStyle->addItem(tr("ListStyle")+QStringLiteral(" (●)"));
    comboStyle->addItem(tr("ListStyle")+QStringLiteral(" (○)"));
    comboStyle->addItem(tr("ListStyle")+QStringLiteral(" (■)"));
    comboStyle->addItem(tr("Number")+QStringLiteral(" ( 1.2.3.)"));
    comboStyle->addItem(tr("Number")+QStringLiteral(" ( a.b.c.)"));
    comboStyle->addItem(tr("Number")+QStringLiteral(" ( A.B.C.)"));
    comboStyle->addItem(tr("Number")+QStringLiteral(" (ⅰ.ⅱ.ⅲ.)"));
    comboStyle->addItem(tr("Number")+QStringLiteral(" (Ⅰ.Ⅱ.Ⅲ.)"));
    comboStyle->setStatusTip(tr("Add serial flag"));
    connect(comboStyle, SIGNAL(activated(int)), this, SLOT(textStyle(int)));

    comboFont = new QFontComboBox();
    comboToolBar->addWidget(comboFont);
    comboFont->setStatusTip(tr("Change font"));
    connect(comboFont, SIGNAL(activated(QString)), this, SLOT(textFamily(QString)));

    comboSize = new QComboBox();
    comboToolBar->addWidget(comboSize);
    comboSize->setEditable(true);
    comboSize->setStatusTip(tr("Change serial number"));

    QFontDatabase db;
    foreach(int size, db.standardSizes())
        comboSize->addItem(QString::number(size));

    connect(comboSize, SIGNAL(activated(QString)), this, SLOT(textSize(QString)));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font().pointSize())));
}
void MyWord::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}
MyChild *MyWord::activeMyChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<MyChild *>(activeSubWindow->widget());
    return 0;
}
QMdiSubWindow *MyWord::findMyChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        MyChild *myChild = qobject_cast<MyChild *>(window->widget());
        if (myChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}
void MyWord::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}
void MyWord::fileNew()
{
    MyChild *child = createMyChild();
    child->newFile();
    child->show();
    enabledText();
}
void MyWord::fileOpen()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open"),QString(), tr("HTML Document (*.htm *.html);;All files (*.*)"));
    if (!fileName.isEmpty()) {
        QMdiSubWindow *existing = findMyChild(fileName);
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }
        MyChild *child = createMyChild();
        if (child->loadFile(fileName)) {
            statusBar()->showMessage(tr("File loaded"), 2000);
            child->show();
            enabledText();
        } else {
            child->close();
        }
    }
}
void MyWord::fileSave()
{
    if (activeMyChild() && activeMyChild()->save())
        statusBar()->showMessage(tr("Save Success"), 2000);
}
void MyWord::fileSaveAs()
{
    if (activeMyChild() && activeMyChild()->saveAs())
        statusBar()->showMessage(tr("Save Success"), 2000);
}
void MyWord::undo()
{
    if(activeMyChild())
        activeMyChild()->undo();
}

void MyWord::redo()
{
    if(activeMyChild())
        activeMyChild()->redo();
}

void MyWord::cut()
{
    if (activeMyChild())
        activeMyChild()->cut();
}

void MyWord::copy()
{
    if (activeMyChild())
        activeMyChild()->copy();
}
void MyWord::paste()
{
    if (activeMyChild())
        activeMyChild()->paste();
}

void MyWord::enabledText()
{
    boldAct->setEnabled(true);
    italicAct->setEnabled(true);
    underlineAct->setEnabled(true);
    leftAlignAct->setEnabled(true);
    centerAct->setEnabled(true);
    rightAlignAct->setEnabled(true);
    justifyAct->setEnabled(true);
    colorAct->setEnabled(true);
}
void MyWord::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(boldAct->isChecked() ? QFont::Bold : QFont::Normal);
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void MyWord::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(italicAct->isChecked());
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}
void MyWord::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(underlineAct->isChecked());
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void MyWord::textAlign(QAction *a)
{
    if(activeMyChild())
    {
        if (a == leftAlignAct)
            activeMyChild()->setAlign(1);
        else if (a == centerAct)
            activeMyChild()->setAlign(2);
        else if (a == rightAlignAct)
            activeMyChild()->setAlign(3);
        else if (a == justifyAct)
            activeMyChild()->setAlign(4);
    }
}
void MyWord::textStyle(int styleIndex)
{
    if(activeMyChild())
    {
        activeMyChild()->setStyle(styleIndex);
    }
}

void MyWord::textFamily(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}
void MyWord::textSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        if(activeMyChild())
            activeMyChild()->mergeFormatOnWordOrSelection(fmt);
    }
}

void MyWord::textColor()
{
    if(activeMyChild())
    {
        QColor col = QColorDialog::getColor(activeMyChild()->textColor(), this);
        if (!col.isValid())
            return;
        QTextCharFormat fmt;
        fmt.setForeground(col);
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
        colorChanged(col);
    }
}
void MyWord::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    boldAct->setChecked(f.bold());
    italicAct->setChecked(f.italic());
    underlineAct->setChecked(f.underline());
}

void MyWord::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    colorAct->setIcon(pix);
}
void MyWord::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        leftAlignAct->setChecked(true);
    else if (a & Qt::AlignHCenter)
        centerAct->setChecked(true);
    else if (a & Qt::AlignRight)
        rightAlignAct->setChecked(true);
    else if (a & Qt::AlignJustify)
        justifyAct->setChecked(true);
}

void MyWord::filePrint()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (activeMyChild()->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Document Print"));
    if (dlg->exec() == QDialog::Accepted)
        activeMyChild()->print(&printer);
    delete dlg;
}
void MyWord::filePrintPreview()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(printPreview(QPrinter*)));
    preview.exec();
}

void MyWord::printPreview(QPrinter *printer)
{
    activeMyChild()->print(printer);
}

void MyWord::about()
{
    QMessageBox::about(this, tr("About"), tr("This is a Qt based text editor,just like Microsoft Office Word."));
}
void MyWord::transEn()
{
    if(translator!=NULL)
    {
        qApp->removeTranslator(translator);
        delete translator;
        translator=NULL;
    }
    translator=new QTranslator;
    translator->load(":/lang_en.qm");
    qApp->installTranslator(translator);
    transZhAct->setEnabled(true);
    transEnAct->setDisabled(true);
    langUpdate();
}
void MyWord::transZh()
{
    if(translator!=NULL)
    {
        qApp->removeTranslator(translator);
        delete translator;
        translator=NULL;
    }
    translator=new QTranslator;
    translator->load(":/lang_zh.qm");
    qApp->installTranslator(translator);
    transEnAct->setEnabled(true);
    transZhAct->setDisabled(true);
    langUpdate();
}
void MyWord::langUpdate()
{
    fileMenu->setTitle(tr("File(&F)"));
    editMenu->setTitle(tr("Edit(&E)"));
    formatMenu->setTitle(tr("Format(&O)"));
    fontMenu->setTitle(tr("Font(&D)"));
    alignMenu->setTitle(tr("Alignment"));
    windowMenu->setTitle(tr("Window(&W)"));
    helpMenu->setTitle(tr("Help(&H)"));
    langMenu->setTitle(tr("Language(&L)"));

    newAct->setText(tr("New(&N)"));
    newAct->setToolTip(tr("New"));
    newAct->setStatusTip(tr("Create a new document"));
    openAct->setText(tr("Open(&O)..."));
    openAct->setToolTip(tr("Open"));
    openAct->setStatusTip(tr("Open an existing document"));
    saveAct->setText(tr("Save(&S)"));
    saveAct->setToolTip(tr("Save"));
    saveAct->setStatusTip(tr("Save current document"));
    saveAsAct->setText(tr("Save As(&A)..."));
    saveAsAct->setToolTip(tr("Save As"));
    saveAsAct->setStatusTip(tr("Save as a new document"));
    printAct->setText(tr("Print(&P)"));
    printAct->setToolTip(tr("Print"));
    printAct->setStatusTip(tr("Print this document"));
    printPreviewAct->setText(tr("Print Preview..."));
    printPreviewAct->setToolTip(tr("Print Preview"));
    printPreviewAct->setStatusTip(tr("Preview the print effert"));
    exitAct->setText(tr("Exit(&X)"));
    exitAct->setToolTip(tr("Exit"));
    exitAct->setStatusTip(tr("Exit the application"));
    undoAct->setText(tr("Undo(&U)"));
    undoAct->setToolTip(tr("Undo"));
    undoAct->setStatusTip(tr("Undo the current operation"));
    redoAct->setText(tr("Redo(&R)"));
    redoAct->setToolTip(tr("Redo"));
    redoAct->setStatusTip(tr("Redo the last operation"));
    cutAct->setText(tr("Cut(&T)"));
    cutAct->setToolTip(tr("Cut"));
    cutAct->setStatusTip(tr("Cut the selected content and put it into the clipboard"));
    copyAct->setText(tr("Copy(&C)"));
    copyAct->setToolTip(tr("Copy"));
    copyAct->setStatusTip(tr("Copy the selected content and put it into the clipboard"));
    pasteAct->setText(tr("Paste(&P)"));
    pasteAct->setToolTip(tr("Paste"));
    pasteAct->setStatusTip(tr("Paste the content into the document from clipboard"));
    boldAct->setText(tr("Bold(&B)"));
    boldAct->setToolTip(tr("Bold"));
    boldAct->setStatusTip(tr("Bold the selected content"));
    italicAct->setText(tr("Italic(&I)"));
    italicAct->setToolTip(tr("Italic"));
    italicAct->setStatusTip(tr("Italic the selected content"));
    underlineAct->setText(tr("Underline(&U)"));
    underlineAct->setToolTip(tr("Underline"));
    underlineAct->setStatusTip(tr("Underline the selected content"));
    leftAlignAct->setText(tr("Align Left(&L)"));
    leftAlignAct->setToolTip(tr("Align Left"));
    leftAlignAct->setStatusTip(tr("Set the selected content left alignment"));
    rightAlignAct->setText(tr("Align Right(&R)"));
    rightAlignAct->setToolTip(tr("Align Right"));
    rightAlignAct->setStatusTip(tr("Set the selected content right alignment"));
    centerAct->setText(tr("Align Center(&E)"));
    centerAct->setToolTip(tr("Align Center"));
    centerAct->setStatusTip(tr("Set the selected content center alignment"));
    justifyAct->setText(tr("Align Justify(&J)"));
    justifyAct->setToolTip(tr("Align Justify"));
    justifyAct->setStatusTip(tr("Set the selected content justify alignment"));
    colorAct->setText(tr("Color(&C)..."));
    colorAct->setToolTip(tr("Color"));
    colorAct->setStatusTip(tr("Set text color"));
    closeAct->setText(tr("Close(&O)"));
    closeAct->setStatusTip(tr("Close activated window"));
    closeAllAct->setText(tr("Close All(&A)"));
    closeAllAct->setStatusTip(tr("Close all windows"));
    tileAct->setText(tr("Tile(&T)"));
    tileAct->setStatusTip(tr("Tile the windows"));
    cascadeAct->setText(tr("Cascade(&C)"));
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    nextAct->setText(tr("Next(&X)"));
    nextAct->setStatusTip(tr("Focus on next window"));
    previousAct->setText(tr("Previous(&V)"));
    previousAct->setStatusTip(tr("Focus on previous window"));
    aboutAct->setText(tr("About(&A)"));
    aboutAct->setStatusTip(tr("About CrzWord"));
    transEnAct->setText(tr("English"));
    transZhAct->setText(tr("Chinese"));
    aboutQtAct->setText(tr("About Qt(&Q)"));
    aboutQtAct->setStatusTip(tr("About Qt Library"));

    fileToolBar->setObjectName(tr("File"));
    editToolBar->setObjectName(tr("Edit"));
    formatToolBar->setObjectName(tr("Format"));
    comboToolBar->setObjectName(tr("Combination"));
    comboFont->setStatusTip(tr("Change font"));
    comboSize->setStatusTip(tr("Change serial number"));
}
