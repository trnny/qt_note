#ifndef VIE_H
#define VIE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTextEdit>
#include <QTextBrowser>
#include "fstream"
#include "io.h"
#include <vector>
#include <string>
#include <QString>
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QAbstractNativeEventFilter>
#include "Windows.h"
#include "stgc.h"

#define QDBG qDebug()<<__FILE__<<__FUNCTION__<<__LINE__

class fNameLabel : public QLabel
{
    Q_OBJECT

public:
    fNameLabel(std::string s);
    ~fNameLabel();
    void setId(int id);
    int  getId();
    void refresh();  //判断是否更改、是否是正在编辑并更新视图
    bool    mChose;
    bool    mChanged;

protected:

private:
    int     mid;
    std::string m_s;

private slots:

};

class Hps : public QWidget  //帮助
{
    Q_OBJECT

public:
    Hps(QWidget *parent = 0);
    ~Hps();
    bool   mShowing;  //显示状态

protected:

private:
    QGridLayout * grid;
    QVBoxLayout * vtb;
    QTextBrowser * tbr;
    void setHelp(std::string s = ":/htm/help");  //help文件的位置
};

class Vie : public QWidget,QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    Vie(QWidget *parent = 0);
    ~Vie();

    void refresh();  //加载并更新文件列表
    bool transfer();        //拉取note文件
    std::string shortSn (std::string lname);           //字符串缩短为可显示并隐藏格式
    std::string getUntitledName();             //获得新建note的名字
    void readyNote();  //加载编辑框(暂不加载编辑的文件)
    void openNote(int index = 0);  //加载编辑的文件
    void newNote();      //新建一个note
    void delNote();   //删除一个note
    void renNote();   //更改名
    void savNote();   //保存

    void help();  //创建help界面(暂不显示)

    Hps * whelp;   //帮助界面
    Stg * wstg;
//    Stg * stg;     //设置  ??为啥引入该指正就会crash掉??
    bool isHide;   //是否最小化
    std::vector<std::string>  vf;   //v note的文件

    void registerGlobalKey();  //注册热键
    std::string cDirName;   //文件夹地址
    std::string cFileType;  //文件类型

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void closeEvent(QCloseEvent *event);
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *);

private:
    bool        mMoveing;   //是否在移动
    QPoint      mMovePosition;     //鼠标位置

    QVBoxLayout * nBox;   //文件列表的盒子
    QSpacerItem * stm;    //列表的空白占位
    QHBoxLayout * eBox;   //水平盒子
    QGridLayout * grid;   //各个盒子的总盒子
    std::vector<fNameLabel *> noteList;  //各个文件名标签
    QTextEdit * tEdit;   //文件编辑的框

    bool        cSetting;   //是否正在编辑
    int  lastChose;     //上次选择的号码
    int  cNoteNum;     //note的数量
    int  sl ;          //最长不折叠字符长度
    int sfsize;       //标签字体大小
    int tfsize;       //文本字体大小

    void UnregisterAllHotKey();  //删除热键

};

#endif // VIE_H
