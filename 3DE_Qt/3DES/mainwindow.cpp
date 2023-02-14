#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


QString hex2bin(QString key)   //16进制转二进制
{
    QString s="";
    for (int i=0;i<key.size();++i)
    {
        QChar ch=key[i];
        if (ch=='0') s+="0000";
        else if (ch=='1') s+="0001";
        else if (ch=='2') s+="0010";
        else if (ch=='3') s+="0011";
        else if (ch=='4') s+="0100";
        else if (ch=='5') s+="0101";
        else if (ch=='6') s+="0110";
        else if (ch=='7') s+="0111";
        else if (ch=='8') s+="1000";
        else if (ch=='9') s+="1001";
        else if (ch=='A') s+="1010";
        else if (ch=='B') s+="1011";
        else if (ch=='C') s+="1100";
        else if (ch=='D') s+="1101";
        else if (ch=='E') s+="1110";
        else if (ch=='F') s+="1111";
    }
    return s;
}

QString bin2hex(QString key)   //二进制转16进制
{
    QString s="";
    for (int i=0;i<(key.size()/4);++i)
    {
        QString subs=key.mid(i*4,4);
        if (subs=="0000") s+='0';
        else if (subs=="0001") s+='1';
        else if (subs=="0010") s+='2';
        else if (subs=="0011") s+='3';
        else if (subs=="0100") s+='4';
        else if (subs=="0101") s+='5';
        else if (subs=="0110") s+='6';
        else if (subs=="0111") s+='7';
        else if (subs=="1000") s+='8';
        else if (subs=="1001") s+='9';
        else if (subs=="1010") s+='A';
        else if (subs=="1011") s+='B';
        else if (subs=="1100") s+='C';
        else if (subs=="1101") s+='D';
        else if (subs=="1110") s+='E';
        else if (subs=="1111") s+='F';
        else break;
    }
    return s;
}

QString permute(QString k,int *arr,int n)   //查表转换数值
{
    QString s="";
    for (int i=0;i<n;++i)
    {
        s+=k[arr[i]-1];   //表下标从0开始，所以-1
    }
    return s;
}

QString shiftLeft(QString k,int shifts)   //向左移动一位或两位
{
    QString s="";
    for (int i=0;i<shifts;++i)
    {
        s="";
        for (int j=1;j<28;++j)
        {
            s+=k[j];
        }
        s+=k[0];
        k=s;
    }
    return s;
}

void roundKey(QString key,QVector<QString> *rkb,QVector<QString> *rkh)   ///生成子密钥
{
    rkb->clear(),rkh->clear();
    //将密钥从16进制转化为二进制
    key=hex2bin(key);

    //置换选择1表
    int keyp[56]={  57,49,41,33,25,17,9,
                    1,58,50,42,34,26,18,
                    10,2,59,51,43,35,27,
                    19,11,3,60,52,44,36,
                    63,55,47,39,31,23,15,
                    7,62,54,46,38,30,22,
                    14,6,61,53,45,37,29,
                    21,13,5,28,20,12,4};

    key=permute(key,keyp,56);   //置换选择   64b->56b

    //位移动表
    int shift_table[16]={   1,1,2,2,
                            2,2,2,2,
                            1,2,2,2,
                            2,2,2,1};

    //置换选择2表
    int key_comp[60]={  14,17,11,24,1,5,
                        3,28,15,6,21,10,
                        23,19,12,4,26,8,
                        16,7,27,20,13,2,
                        41,52,31,37,47,55,
                        30,40,51,45,33,48,
                        44,49,39,56,34,53,
                        46,42,50,36,29,32};
    //分为左右两部分
    QString left=key.mid(0,28);
    QString right=key.mid(28,28);

    //16轮运算得到子密钥
    for (int i=0;i<16;++i)
    {
        left=shiftLeft(left,shift_table[i]);
        right=shiftLeft(right,shift_table[i]);

        QString combine=left+right;
        QString RoundKey=permute(combine,key_comp,48);   //合并左右两部分后再置换选择   56b->48b
        rkb->push_back(RoundKey);
        rkh->push_back(bin2hex(RoundKey));
    }
}

QString _xor(QString s1,QString s2)   //二进制异或操作
{
    QString s="";
    for (int i=0;i<s1.size();++i)
    {
        if (s1[i]==s2[i]) s+='0';
        else s+='1';
    }
    return s;
}

QString encrypt(QString m,QVector<QString> rkb)   ///DES加密
{
    //将16进制的原始数据转化为二进制数据
    m=hex2bin(m);
    int initial_perm[64]={  58,50,42,34,26,18,10,2,
                            60,52,44,36,28,20,12,4,
                            62,54,46,38,30,22,14,6,
                            64,56,48,40,32,24,16,8,
                            57,49,41,33,25,17,9,1,
                            59,51,43,35,27,19,11,3,
                            61,53,45,37,29,21,13,5,
                            63,55,47,39,31,23,15,7};

    m=permute(m,initial_perm,64);
    //cout<<"初始置换："<<bin2hex(pt)<<endl;

    QString left=m.mid(0,32);
    QString right=m.mid(32,32);
    //cout<<"left:"<<bin2hex(left)<<"   right:"<<bin2hex(right)<<endl;

    //E扩展置换表
    int exp_d[48]={ 32,1,2,3,4,5,4,5,
                    6,7,8,9,8,9,10,11,
                    12,13,12,13,14,15,16,17,
                    16,17,18,19,20,21,20,21,
                    22,23,24,25,24,25,26,27,
                    28,29,28,29,30,31,32,1};
    //S盒置换表
    int s[8][4][16]={
                    14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
                    0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
                    4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
                    15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13,

                    15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
                    3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
                    0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
                    13,8,10,1,3,15,4,2,11,6,7,12,10,5,14,9,

                    10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
                    13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
                    13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
                    1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12,

                    7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
                    13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
                    10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
                    3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14,

                    2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
                    14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
                    4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
                    11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3,

                    12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
                    10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
                    9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
                    4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13,

                    4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
                    13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
                    1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
                    6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12,

                    13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
                    1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
                    7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
                    2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11};
    //P置换表
    int per[32]= { 16,7,20,21,
                    29,12,28,17,
                    1,15,23,26,
                    5,18,31,10,
                    2,8,24,14,
                    32,27,3,9,
                    19,13,30,6,
                    22,11,4,25};

    //轮函数
    for (int i=0;i<16;++i)
    {
        QString right_expanded=permute(right,exp_d,48);   //E扩展 32b->48b
        QString x=_xor(rkb[i],right_expanded);   //异或   48b->48b
        QString op="";
        //S盒置换   48b->32b
        for (int j=0;j<8;++j)
        {
            QChar x0=x[j*6+0],x5=x[j*6+5];
            int row=2*(x0.toLatin1()-'0')+int(x5.toLatin1()-'0');
            QChar x1=x[j*6+1],x2=x[j*6+2],x3=x[j*6+3],x4=x[j*6+4];
            int col=8*(x1.toLatin1()-'0')+4*(x2.toLatin1()-'0')+2*(x3.toLatin1()-'0')+(x4.toLatin1()-'0');
            int val=s[j][row][col];
            //将10进制的val转换为二进制的op
            op+=char(val/8+'0');
            val%=8;
            op+=char(val/4+'0');
            val%=4;
            op+=char(val/2+'0');
            val%=2;
            op+=char(val+'0');
        }

        op=permute(op,per,32);   //P盒置换   32b->32b
        left=_xor(left,op);

        if (i!=15)
        {
            swap(left,right);   //左右交换
        }
        //cout<<"Round"<<i+1<<"   "<<bin2hex(left)<<","<<bin2hex(right)<<"   "<<rk[i]<<endl;
    }
    QString combine=left+right;
    //逆置换表
    int final_perm[64]={40,8,48,16,56,24,64,32,
                        39,7,47,15,55,23,63,31,
                        38,6,46,14,54,22,62,30,
                        37,5,45,13,53,21,61,29,
                        36,4,44,12,52,20,60,28,
                        35,3,43,11,51,19,59,27,
                        34,2,42,10,50,18,58,26,
                        33,1,41,9,49,17,57,25};
    QString c=bin2hex(permute(combine,final_perm,64));
    return c;
}
QString decrypt(QString c,QVector<QString> rkb,QVector<QString> rkh)   ///DES解密
{
//    reverse(rkb.begin(),rkb.end());   //反转密钥
//    reverse(rkh.begin(),rkh.end());
    //反转，所以怎么使用reverse。。。好麻烦
    QVector<QString> temp;
    for (auto i:rkb)
    {
        temp.push_back(i);
    }
    rkb.clear();
    for (int i=temp.size()-1;i>=0;--i)
    {
        rkb.push_back(temp[i]);
    }
    temp.clear();
    for (auto i:rkh)
    {
        temp.push_back(i);
    }
    rkh.clear();
    for (int i=temp.size()-1;i>=0;--i)
    {
        rkh.push_back(temp[i]);
    }
    return encrypt(c,rkb);   //再调用加密即可完成解密
}

void padding(QString &s)   //ANSI X9.23填充方式将16进制明文填充，8字节（16个16进制数,64位）为一组
{
    if (s.size()%16==0) return;
    int cnt=1;
    while ((s.size()+2)%16!=0)
    {
        s+="00";
        ++cnt;
    }
    s=s+'0'+char(cnt+'0');
    return;
}

void dePadding(QString &s)   //去掉填充部分
{
    auto it=s.end()-1;
    QChar x=*(it-1),y=*it;
    if (x=='0' && y!='0')   //例如 00 02
    {
        s=s.mid(0,s.length()-(y.toLatin1()-'0')*2);   //去掉填充
    }
    return;
}

QString CBCEncrypt(QString &m,QString &IV,QVector<QString> rkb)
{
    QString s="",x=IV;
    int cnt=m.size()/16;   //组数（每16个数一组）
    for (int i=0;i<cnt;++i)
    {
        QString subs=m.mid(16*i,16);
        subs=bin2hex(_xor(hex2bin(subs),hex2bin(x)));
        subs=encrypt(subs,rkb);
        x=subs;
        s+=subs;
    }
    return s;
}

QString CBCDecrypt(QString &c,QString &IV,QVector<QString> rkb,QVector<QString> rkh)
{
    QString s="",x=IV;
    int cnt=c.size()/16;
    for (int i=0;i<cnt;++i)
    {
        QString subs=c.mid(16*i,16);
        subs=decrypt(subs,rkb,rkh);
        subs=bin2hex(_xor(hex2bin(subs),hex2bin(x)));
        s+=subs;
        x=c.mid(16*i,16);
    }
    return s;
}

void _3DESEncrypt(QString &m,QString &c,QString &IV,QString key[],QVector<QString> rkb,QVector<QString> rkh)
{
    padding(m);
    //3DES加密：加密->解密->加密
    for (int i=1;i<=3;++i)
    {
        roundKey(key[i],&rkb,&rkh);   ///生成子密钥
        if (i==1 || i==3)   ///CBC模式_DES加密
        {
            c=CBCEncrypt(m,IV,rkb);   //CBC模式DES加密函数，生成密文
        }
        else   ///CBC模式_DES解密
        {
            m=CBCDecrypt(c,IV,rkb,rkh);   //CBC模式DES解密函数，生成明文
        }
    }
    return;
}

void _3DESDecrypt(QString &m,QString &c,QString &IV,QString key[],QVector<QString> rkb,QVector<QString> rkh)
{
    //3DES解密：解密->加密->解密
    for (int i=3;i>=1;--i)
    {
        roundKey(key[i],&rkb,&rkh);   ///生成子密钥
        if (i==1 || i==3)   ///DES解密
        {
            m=CBCDecrypt(c,IV,rkb,rkh);
        }
        else   ///DES加密
        {
            c=CBCEncrypt(m,IV,rkb);
        }
    }
    dePadding(m);
    return;
}

QString filePath;
void fileWrite(MainWindow *w,const QString &s)
{
    if (filePath.isEmpty())
    {
        QMessageBox::warning(w,"警告","路径不能为空");
    }
    QFile file(filePath);
    file.open(QIODevice::Append);
    QByteArray ba=s.toLatin1()+'\n';
    const char *ch=ba.data();
    file.write(ch);
    file.close();
    return;
}

void MainWindow::on_pushButton_2_clicked()   //加密按钮
{
    //输入 m
    key[1]=ui->lineEdit_2->text();
    key[2]=ui->lineEdit_3->text();
    key[3]=ui->lineEdit_4->text();
    IV=ui->lineEdit_5->text();

    //3DES加密
    _3DESEncrypt(m,c,IV,key,rkb,rkh);
    //输出c
    //ui->lineEdit_7->setText(c);
    fileWrite(this,c);
}

void MainWindow::on_pushButton_3_clicked()   //解密按钮
{
    //3DES解密
    _3DESDecrypt(m,c,IV,key,rkb,rkh);
    //输出 m
    //ui->lineEdit_7->setText(m);
    fileWrite(this,m);
}

void MainWindow::on_pushButton_clicked()   //选择文件（输入）按钮
{
    QString filePath=QFileDialog::getOpenFileName(this,"打开文件","C:\\Users\\MI\\Desktop","(*.txt)");
    if (filePath.isEmpty())
    {
        QMessageBox::warning(this,"警告","路径不能为空");
    }
    else
    {
        ui->lineEdit->setText(filePath);
        QFile file(filePath);
        //文件打开方式
        file.open(QIODevice::ReadOnly);
        QByteArray arr;
        arr=file.readAll();    //读取全部信息
        m=arr;
        //ui->lineEdit_7->setText(arr);
        file.close();
    }
}

void MainWindow::on_pushButton_4_clicked()   //选择文件（输出）按钮
{
    filePath=QFileDialog::getOpenFileName(this,"打开文件","C:\\Users\\MI\\Desktop\\DESOutput.txt","(*.txt)");
    ui->lineEdit_6->setText(filePath);
}
