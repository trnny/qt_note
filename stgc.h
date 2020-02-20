#ifndef STGC_H
#define STGC_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QString>
#include <QDialogButtonBox>

//设置的测试页面

struct config
{
    bool autoRun;
    bool autoSave;
    int audioType;
    int labelFontSize;
    int noteFontSize;
};

extern bool eSetting;

extern struct config cfgv;

class Stg : public QWidget //设置
{
    Q_OBJECT

public:
    Stg(QWidget *parent = 0);
    ~Stg();

    bool mChange;  //是否有设置更改
    bool mShowing; //是否显示
    QString configPath;
    void getConfig();
    void setConfig();
    void defaultConfig();

protected slots:
    void jected();
    void saveCfg();

private:
    QGridLayout *grid;
    QVBoxLayout *vbll;
    QVBoxLayout *vblr;
    QHBoxLayout *hbl;
    QDialogButtonBox *buttonBox;
    std::vector<QLabel *> vll;
    std::vector<QLineEdit *> vlr;

};

#endif // STGC_H
