#ifndef MY_UNIQUE_HEADER
#define MY_UNIQUE_HEADER

#include<iostream>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<algorithm>
#include<time.h>
using namespace std;

//路线搜索因为跳帧中断
//机器人调度因为物品刷新中断
//船调度不知道是否允许中断

// 1. 区域分块
// 2. 机器人调度，对被调度机器人，找处在同一区块的物品，再对物品进行遍历，首先，将阈值作为最小值，比较物品的绝对距离是否小于最小值，若小于最小值，算a星，若a星同样小于最小值，更换最小值为a星，循环完所有同一区块的物品。若最后没有距离找到小于阈值的物品，取消阈值，重新计算。
// 3. 在初始化期间，在陆地上随机生成n个点，采用a星算法（同样判断是否同一区块，区块不同，认为距离为1000），找最近的港口，按照n个点各自最近港口不同，分为五个相加小于等于n的分数，作为港口的陆地便利性分。



char map[200][200] = {};//在地图初始化时，把所有固定障碍读到这里
int nowtime = 0;//当前帧序号
int moneyget;//当前获得金额

int skipFrameSymbol;//跳帧信号，跳几帧值为几

struct RobotRoute
{
    RobotRoute(){
        validsymbol = 0;
        step = new int[1000];
        location_x = new int[1000];
        location_y = new int[1000];
    };
    bool validsymbol;
    int time;//机器人出发时刻，由路径搜索算法确定
    int steptotal;//总步骤数
    int* step;//机器人路线,上下左右分别为2、3、1、0，静止为-1
    int x;//目的货物坐标
    int y;
    int* location_x;//机器人预测实时位置坐标
    int* location_y;//机器人预测实时位置坐标
};

template<class T>
struct Queue
{
    Queue(){
        head = 0;
        tail = 0;
    }
    T node[5000];//可能不够
    int head;//头
    int tail;//尾
};


class Berth
{
public: 
    Berth(){};
    static int num;//数量
    static float blockrate;//开放率,(0-1)，任意两个点，越是互联概率高越接近于1
    int id;//港口编号
    int x;//坐标
    int y;//坐标
    int time;//港口到虚拟点时间
    int velocity;//该港口装卸速度
    float convenience;//该港口的便利程度指数（0-1）
    Queue<int> goodsprice;//港口囤积物品
};
int Berth::num = 5;
float Berth::blockrate = 1;
Berth berth[10];

 

class Ship
{
public:
    Ship(){
        ifwait = 1;
    };
    static int num;//数量
    int status;//船的目前状态
    int BerthId;//船的目标泊位（题给）
    int Berthdispatch;//船的目标泊位（调度算法得）
    int time;//船的出发时刻，由路径搜索算法确定
    bool ifwait;//船是否等待调度
    static int capacity;//船的容积
};
int Ship::num = 5;
int Ship::capacity = 0;
Ship ship[5];

class Robot
{
public:
    Robot(){
        ifwait = 1;

        route = new RobotRoute();
    };
    static int num;//数量
    int x;//坐标
    int y;//坐标
    bool iftake;//是否携带货物
    bool status;//运行正常状态
    RobotRoute* route;//机器人路径
    bool ifwait;//机器人是否等待调度  
    int takegoods;
    int forBerth; 
};
Robot robot[10];
int Robot::num = 10;


class xy
{
public:
    int x;
    int y;
};

xy detailposition(int RobotId, int BerthId)//机器人位置细节处理
{
    xy tmp;
    tmp.x = 0,tmp.y = 0;
    int min = 99999999;

    for(int pos_x = berth[BerthId].x;pos_x < berth[BerthId].x + 4;pos_x++)
    {
        for(int pos_y = berth[BerthId].y;pos_y < berth[BerthId].y + 4;pos_y++)
        {
            int dis = abs(robot[RobotId].x - pos_x) + abs(robot[RobotId].y - pos_y);
            if(dis < min)
            {
                min = dis;
                tmp.x = pos_x;
                tmp.y = pos_y;
            }
        }
    }

    return tmp;
}

struct Interrupt_t{
    Interrupt_t(){
        num = 0;
    }
    int num;
    int id[10];
};

class Goods
{
public:
    Goods(){
        validSymbol = 1;
    };
    static int num;//数量
    static bool symbol;//机器人调度中断标志
    bool validSymbol;//货物有效标志
    int x;//坐标
    int y;//坐标
    int time;//刷新时间
    int price;//价格
};
int Goods::num = 0;
bool Goods::symbol = false;
Queue<Goods> goods;



//输出
class Output {
public:
    Output() {};

    void move(int robot, int op) {
        cout << "move " << robot << " " << op << endl;
        //cerr <<endl<< "move " << robot << " " << op << endl<<endl;
    }

    void get(int robot)
    {
        cout << "get " << robot << endl;
        //cerr << "get " << robot << endl;
    }
    void pull(int robot)
    {
        cout << "pull " << robot << endl;
        //cerr << "pull " << robot << endl;
    }
    void ship(int shipId,int BerthId)
    {
        cout << "ship " << shipId << " " << BerthId << endl;
        //cerr << "ship " << shipId << " " << BerthId << endl;
    }
    void go(int shipId){
        cout << "go " << shipId << endl;
        //cerr << "go " << shipId << endl;
    }
    void OK()
    {

        cout << "OK" <<endl;
        //cout<<fflush;
        fflush(stdout);
    }
};

Output output;

#endif
