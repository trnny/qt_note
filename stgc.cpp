#include "stgc.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>

struct config cfgv ={0,1,0,13,13};   //最起初的默认设置，后由config决定值
bool eSetting = false;

Stg::Stg(QWidget *parent)
    : QWidget(parent)  //设置界面
{
    setWindowTitle(tr("Setup"));
    setWindowFlags(windowFlags()&~Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    configPath = "setup.ini";
    getConfig();

    grid = new QGridLayout(this);
    grid->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(grid);

    vbll = new QVBoxLayout;
    vbll->setContentsMargins(10,5,15,2);
    vbll->setSpacing(8);
    grid->addLayout(vbll,0,0);

    vblr = new QVBoxLayout;
    vblr->setContentsMargins(15,5,10,2);
    vblr->setSpacing(8);
    grid->addLayout(vblr,0,1);

    for(int i = 0;i < vll.size() && i < vlr.size();i++)     //由参数到界面
    {
        vbll->addWidget(vll[i]);
        vblr->addWidget(vlr[i]);
    }

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    grid->addWidget(buttonBox, 1, 0, 1, 2);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(saveCfg()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(jected()));

    mChange = false;
}

Stg::~Stg()
{
    //if(mChange)
        setConfig();
}

void Stg::saveCfg()     //由界面到参数
{
    //
    for(int i = 0;i < vll.size() && i < vlr.size();i++)     //由参数到界面
    {
        if(vll[i]->text() == "labelFontSize")
            cfgv.labelFontSize = vlr[i]->text().toInt();
        else if(vll[i]->text() == "noteFontSize")
            cfgv.noteFontSize = vlr[i]->text().toInt();
        else if(vll[i]->text() == "audioType")
            cfgv.audioType = vlr[i]->text().toInt();
        else if(vll[i]->text() == "autoSave")
            cfgv.autoSave = vlr[i]->text().toInt();
        else if(vll[i]->text() == "autoRun")
            cfgv.autoRun = vlr[i]->text().toInt();
    }

    setConfig();
    close();
    eSetting = false;
    /* 若不备注掉表示保存后重启 */
//    qApp->closeAllWindows();
//    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}

void Stg::jected()
{
    close();
    eSetting = false;
}

void Stg::getConfig()    //由文件到参数
{
    QFile file(configPath);
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&file);
        QString line_in;
        stream.seek(0);//将当前读取文件指针移动到文件开始
        while( !stream.atEnd()){
            line_in = stream.readLine();
            QStringList list1 = line_in.split(' ', QString::SkipEmptyParts);
            if(list1.count() == 2)
            {
                vll.push_back(new QLabel(list1[0]));
                vlr.push_back(new QLineEdit(list1[1]));
                if(list1[0] == "labelFontSize")
                    cfgv.labelFontSize = list1[1].toInt();
                else if(list1[0] == "noteFontSize")
                    cfgv.noteFontSize = list1[1].toInt();
                else if(list1[0] == "audioType")
                    cfgv.audioType = list1[1].toInt();
                else if(list1[0] == "autoSave")
                    cfgv.autoSave = list1[1].toInt();
                else if(list1[0] == "autoRun")
                    cfgv.autoRun = list1[1].toInt();
            }
        }
        file.close();
    }
    else
    {
        defaultConfig();
    }
}

void Stg::setConfig()    //由参数到文件
{
    QFile file(configPath);
    if(file.open(QFile::ReadWrite | QFile::Text))
    {
        QTextStream stream(&file);

        stream.seek(0);//将当前读取文件指针移动到文件首

        stream << "#Please do not modify the data in this file";
        stream << "\nautoRun " << cfgv.autoRun;
        stream << "\nautoSave " << cfgv.autoSave;
        stream << "\naudioType " << cfgv.audioType;
        stream << "\nlabelFontSize " << cfgv.labelFontSize;
        stream << "\nnoteFontSize " << cfgv.noteFontSize;

        file.close();
    }
    else
    {
        qDebug() << "Create setup.ini defeated";
    }
}

void Stg::defaultConfig()
{
    QFile file(":/ini/defaultConfig");
    if(!file.open(QFile::ReadOnly | QFile::Text))
        qDebug() << "default config error";
    else
    {
        QTextStream stream(&file);
        QString line_in;
        while( !stream.atEnd()){
            line_in = stream.readLine();
            QStringList list1 = line_in.split(' ', QString::SkipEmptyParts);
            if(list1.count() == 2)
            {
                vll.push_back(new QLabel(list1[0]));
                vlr.push_back(new QLineEdit(list1[1]));
                if(list1[0] == "labelFontSize")
                    cfgv.labelFontSize = list1[1].toInt();
                else if(list1[0] == "noteFontSize")
                    cfgv.noteFontSize = list1[1].toInt();
                else if(list1[0] == "audioType")
                    cfgv.audioType = list1[1].toInt();
                else if(list1[0] == "autoRun")
                    cfgv.autoRun = list1[1].toInt();
                else if(list1[0] == "autoSave")
                    cfgv.autoSave = list1[1].toInt();
            }
        }
        file.close();
    }
}
