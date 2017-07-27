#ifndef MYCHILD_H
#define MYCHILD_H

#include <QObject>
#include <QWidget>
#include <QTextEdit>
#include <QCloseEvent>
#include <QtPrintSupport/QPrinter>

class MyChild : public QTextEdit
{
    Q_OBJECT
public:
    void newFile();
    bool loadFile(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(QString fileName);
    QString userFriendlyCurrentFile();
    QString currentFile(){return curFile;}
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);   //格式字体设置
    void setAlign(int align);                                           //段落对齐设置
    void setStyle(int style);
    MyChild();
    ~MyChild();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void documentWasModified();
private:
    bool maybeSave();
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    QString curFile;
    bool isUntitled;
};

#endif // MYCHILD_H
