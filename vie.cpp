#include "vie.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QMessageBox>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QFont>
#include <QFile>
#include <QDir>
#include <fstream>
#include "io.h"
#include <QDebug>
#include <QTextStream>
#include "Windows.h"

/*              Vie                */

Vie::Vie(QWidget *parent)
    : QWidget(parent)  //构造函数的:语法赋值
{
    qApp->installNativeEventFilter(this);
    setWindowTitle(tr("Note"));
    QDesktopWidget* desktop = QApplication::desktop();
    setFixedSize((desktop->width() + desktop->height()) / 4,desktop->height() /2);
    move((desktop->width() - width())/2, (desktop->height() - height())/2);
    setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint); //最前台
    setAttribute(Qt::WA_TranslucentBackground, true);  //透明
    mMoveing = false;
    cSetting = false;

    grid = new QGridLayout(this);
    setLayout(grid);

    nBox = new QVBoxLayout;
    nBox->setContentsMargins(0, 2, 15, 2);  //note列表盒子 坐上右下
    nBox->setSpacing(8);
    grid->addLayout(nBox,0,0);   //文件列表盒子

//    Stg *stg = new Stg();
//    stg->show();
    help();  //帮助界面的加载
    readyNote();  //编辑框加载
    registerGlobalKey();   //注册热键

    cDirName = "vnt/";             //文件夹名  例如 vnt/
    cFileType = "vnt";
    isHide = true;
    lastChose = 0;

    sfsize = cfgv.labelFontSize;

    transfer();    //拉取文件更新列表
    openNote(6);    //打开一个note

}

Vie::~Vie()
{
    UnregisterAllHotKey();
}

void Vie::registerGlobalKey(){ //注册全局键
    bool registerH = RegisterHotKey((HWND)this->winId(), 72, MOD_CONTROL|MOD_ALT,72);
    bool registerN = RegisterHotKey((HWND)this->winId(), 78, MOD_CONTROL|MOD_ALT,78);
    bool registerQ = RegisterHotKey((HWND)this->winId(), 81, MOD_CONTROL|MOD_ALT,81);

    if(!registerH){
        qDebug()<<"H registered for Vie fail.";
    }
    if(!registerN){
        qDebug()<<"N registered for Vie fail.";
    }
    if(!registerQ){
        qDebug()<<"Q registered for Vie fail.";
    }
}

void Vie::UnregisterAllHotKey()
{
    UnregisterHotKey((HWND)this->winId(),     72);
    UnregisterHotKey((HWND)this->winId(),     78);
    UnregisterHotKey((HWND)this->winId(),     81);
    qDebug() << "UnregisterHotKey";
}

/*             Hps                */

Hps::Hps(QWidget *parent)
    : QWidget(parent)  //帮助界面
{
    setWindowTitle(tr("Helps"));
    QDesktopWidget* desktop = QApplication::desktop();
    setFixedSize((desktop->width() + desktop->height()) /8,(desktop->height() / 2) - 40);
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);
    setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    grid = new QGridLayout(this);
    setLayout(grid);
    vtb = new QVBoxLayout; //help的布局
    grid->addLayout(vtb,0,0);
    tbr = new QTextBrowser;
    tbr->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tbr->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vtb->addWidget(tbr);

    setHelp();  //显示帮助文件
}

Hps::~Hps() {  }

/*  NotesList  */

fNameLabel::fNameLabel(std::string s)
{
    mChose = false;
    mChanged = false;
    QFont lbl_font;
    lbl_font.setPointSize(cfgv.labelFontSize);
    setFont(lbl_font);
    refresh();  //更新样式
    setText(QString::fromStdString(s));
}

fNameLabel::~fNameLabel() {   }

/*  NoteEdit  */



/*   Events   */

bool Vie::nativeEventFilter(const QByteArray &eventType, void *message, long *)
{
    if (eventType == "windows_generic_MSG")
    {
        MSG *msg = static_cast<MSG *>(message);
        if(msg->message == WM_KEYDOWN)
        {
            if (VK_F1 == msg->wParam)
            {
                qDebug() << "F1";
                if(whelp->mShowing)
                {
                    whelp->mShowing = false;
                    whelp->hide();
                }
                else
                {
                    whelp->mShowing = true;
                    whelp->show();
                }
                return  true;
            }
            if (VK_F2 == msg->wParam)
            {
                qDebug() << "F2";
                //设置
                if(eSetting)
                {
                    eSetting = false;
                    wstg->hide();
                    /* 备注掉则需用户重启后设置生效 */
//                    refresh();
//                    openNote(lastChose); //打开上次的note
                }
                else
                {
                    eSetting = true;
                    wstg->show();
                }
                return  true;
            }
            if (40 == msg->wParam && GetKeyState(VK_CONTROL)<0)
            {
                int nex = lastChose + 1;
                nex = nex < cNoteNum ? nex : 0;
                openNote(nex);
                return true;
            }
            if (38 == msg->wParam && GetKeyState(VK_CONTROL)<0)
            {
                int nex = lastChose - 1;
                nex = nex < 0 ? cNoteNum - 1 : nex;
                openNote(nex);
                return true;
            }
            if ('D' == msg->wParam && GetKeyState(VK_CONTROL)<0)
            {
                delNote();   //删除
                return true;
            }
            if ('R' == msg->wParam && GetKeyState(VK_CONTROL)<0)
            {
                renNote();  //重命名
                return true;
            }
            if ('S' == msg->wParam && GetKeyState(VK_CONTROL)<0)
            {
                savNote();  //保存
                return true;
            }
        }
        else if(msg->message == WM_HOTKEY)
        {
            qDebug() << "Get : " << (char)msg->wParam;
            if(msg->wParam == 72)
            {
                //emit getHHotKey();
                if(isHidden())
                {
                    show();
                    activateWindow();
                }
                else
                {
                    hide();
                }
                return  true;
            }
            if(msg->wParam == 78)
            {
                //emit getNHotKey();
                newNote();
                show();
                return  true;
            }
            if(msg->wParam == 81)
            {
                //emit getQHotKey();
                close();
                return  true;
            }
        }
    }
    return false;
}

void Vie::closeEvent(QCloseEvent *event)
{
    //在这里添加你希望执行关闭事件需要处理的事情
    //弹出消息框，关闭其他窗口
    QCoreApplication::quit();    //完全退出
}

void Vie::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(QColor(240,240,235,246)));
    painter.setPen(QColor(Qt::transparent));  //边框
    painter.fillRect(this->rect(),QColor(0,0,0,0));
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 5, 5);  //边框弧度

    QPainter pLine(this);
    pLine.setPen(QPen(QColor(180, 180, 180, 180),2,Qt::SolidLine));
    pLine.drawLine(width()/4, 15, width()/4, height() - 15);  //画线

    QWidget::paintEvent(event);

}

void Vie::mousePressEvent(QMouseEvent *event)
{
    mMoveing = true;
    //记录下鼠标相对于窗口的位置
    //event->globalPos()鼠标按下时，鼠标相对于整个屏幕位置
    //pos() this->pos()鼠标按下时，窗口相对于整个屏幕位置
    mMovePosition = event->globalPos() - pos();
    return QWidget::mousePressEvent(event);
}

//重写鼠标移动事件
void Vie::mouseMoveEvent(QMouseEvent *event)
{
    if (mMoveing && (event->buttons() && Qt::LeftButton)
        && (event->globalPos()-mMovePosition).manhattanLength() > QApplication::startDragDistance())
    {
        move(event->globalPos()-mMovePosition);
        mMovePosition = event->globalPos() - pos();
    }
    return QWidget::mouseMoveEvent(event);
}

void Vie::mouseReleaseEvent(QMouseEvent *event)
{
    mMoveing = false;
}

/*  MemberFunctions  */

void Hps::setHelp(std::string s)
{
    QFile file(QString::fromStdString(s));
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream textStream(&file);
        tbr->setHtml(textStream.readAll());
        file.close();
    }
    else
    {
        tbr->append("<h1 style=\"color:#ec0053;line-height:100%;text-algin:center;\">Error 404</h1>");
    }
    mShowing = true;
}

std::string Vie::shortSn (std::string lname)
{
    QString fname = QString::fromStdString(lname);
    fname = fname.left(fname.lastIndexOf('.'));
    if(fname.length() < sl)   //sl由refresh取值
        return fname.toStdString();
    else if(fname.length() == sl)
        return (fname.left(sl - 1) + "..").toStdString();
    else
        return (fname.left(sl - 2) + "...").toStdString();
}

void Vie::refresh()
{
    cNoteNum = vf.size();

    sl = (width() / 4 - 50) / (sfsize - 4) + 1;
    if (cNoteNum == 0)
    {
    }
    else
    {
        int preSize = noteList.size();
        if(preSize){
            nBox->removeItem(stm);
            delete stm;
            stm = NULL;
            for(int i = 0;i < preSize ;i++)
            {
                nBox->removeWidget(noteList[0]);
                delete noteList[0];
                noteList[0] = NULL;
                noteList.erase(noteList.begin());
            }
        }
        stm = new QSpacerItem(20, height(), QSizePolicy::Expanding);
        for(int i = 0;i < cNoteNum;i++)
        {
            noteList.push_back(new fNameLabel(shortSn(vf[i])));
            noteList[i]->setId(i);
            nBox->addWidget(noteList[i]);
        }
        nBox->addSpacerItem(stm);
    }
}

void Vie::readyNote()
{
    tEdit = new QTextEdit;
    QFont t_font;
    t_font.setPointSize(cfgv.noteFontSize);  // 字号
    //t_font.setPixelSize(16);
    tEdit->setFont(t_font);
    tEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tEdit->setStyleSheet("\
                         border-width: 1px;\
                         border-style: solid;\
                         border-color: rgb(200, 180, 160);\
                         border-radius:3px;\
                         color:rgb(105,35,36);\
                         ");
    grid->addWidget(tEdit,0,1);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 3);
}

void Vie::openNote(int index)
{
    if(index >= cNoteNum)
    {
        index = cNoteNum - 1;
    }
    if(lastChose >= cNoteNum)   //一般是删掉了最后的一个文件
    {
        lastChose = cNoteNum - 1;
    }
    if(index < cNoteNum && index >= 0)
    {
        if(lastChose != -1){
            noteList[lastChose]->mChose = false;   //??
            noteList[lastChose]->refresh();
        }
        lastChose = index;
        noteList[lastChose]->mChose = true;
        noteList[lastChose]->refresh();
        QFile file(QString::fromStdString(cDirName + vf[lastChose]));
        if(file.open(QFile::ReadWrite | QFile::Text))
        {
            QTextStream textStream(&file);
            tEdit->setText(textStream.readAll());
            file.close();
        }
        else
        {
            tEdit->setHtml("<h1 style=\"color:#ec0053;line-height:100%;text-algin:center;\">Error</h1>");
        }
    }
    else
    {
        QFile file(":/htm/nonote");
        if(file.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream textStream(&file);
            tEdit->setHtml(textStream.readAll());
            file.close();
        }
        else
        {
            tEdit->append("<h1 style=\"color:#ec0053;line-height:100%;text-algin:center;\">Error 404</h1>");
        }
    }
}

void Vie::delNote()
{
    if(lastChose >= vf.size()) return;
    QFile file(QString::fromStdString(cDirName + vf[lastChose]));
    if(file.exists())
    {
        file.remove();
    }
    transfer();
    openNote(lastChose);
}

void Vie::renNote()
{
    //rename
}

void Vie::savNote()
{
    //savefile
    if(lastChose >= vf.size()) return;
    QFile file(QString::fromStdString(cDirName + vf[lastChose]));
    if (file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream stream(&file);
        stream.seek(0);
        QString cBuf = tEdit->toPlainText();
        stream << cBuf;
        stream.flush();
        file.close();
    }else{
        qDebug() << "failed to save file";
    }
    openNote(lastChose);
}


std::string Vie::getUntitledName()
{
    int count = 0;
    int maxn = 0;
    for(int i = 0;i < vf.size();i++)
    {
        QString un = QString::fromStdString(vf[i]);
        un = un.left(un.lastIndexOf('.'));
        if(un.startsWith("Untitled"))
        {
            count++;
            un = un.mid(8);
            if(un.length() > 0)
            {
                maxn = maxn > un.toInt() ? maxn : un.toInt();
            }
        }
    }
    if (count == 0)return "Untitled";
    return QString("Untitled%1").arg(maxn+1).toStdString();
}

void Vie::newNote()
{
    if(cDirName.length() > 0)
    {
        QString sPath = QDir::currentPath() + "/" + QString::fromStdString(cDirName);
        sPath = sPath.left(sPath.lastIndexOf('/'));
        QDir dir(sPath);
        if(!dir.exists(sPath))
        {
            if(!dir.mkdir(sPath))
                qDebug() << sPath;
        }
    }
    tEdit->clear();
    std::string newFileName = getUntitledName();
    vf.push_back(newFileName + "." + cFileType);
    refresh();
    openNote(vf.size() - 1);
}

void Vie::help()
{
    whelp = new Hps();
    whelp->mShowing = false;
    wstg = new Stg();
    wstg->mShowing = false;
}

bool Vie::transfer()
{
    vf.clear();
    cNoteNum = 0;
    std::string fileName = cDirName + "*." + cFileType;
    int tNum = 0;
    _finddata_t fileInfo;
    long long handle = _findfirst(fileName.c_str(), &fileInfo);
    if (handle == -1LL)
    {
        qDebug() << "failed to transfer files" ;
        return false;
    }

    do
    {
        tNum ++;
        vf.push_back(fileInfo.name);
    } while (_findnext(handle, &fileInfo) == 0);
    qDebug() << "files' number:  " << tNum ;
    refresh();

    return true;
}


void fNameLabel::setId(int id)
{
    this->mid = id;
}

int fNameLabel::getId()
{
    return this->mid;
}

void fNameLabel::refresh()
{
    if(mChose)
    {
        setStyleSheet("\
            border-width: 1px;\
            border-style: solid;\
            border-color: #3aad66;\
            border-radius:3px;\
            padding:1px;\
            ");
    }
    else if(mChanged)
    {
        setStyleSheet("\
            border-width: 1px;\
            border-style: solid;\
            border-color: #d666ad;\
            border-radius:3px;\
            padding:1px;\
            ");
    }
    else
    {
        setStyleSheet("\
            border-width: 1px;\
            border-style: solid;\
            border-color: #c8b4a0;\
            border-radius:3px;\
            padding:1px;\
            ");
    }
}
