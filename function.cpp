#include"A_search.cpp"
#include"RobotDispatch.cpp"

void getBerthConvenience(){
    srand(time(NULL)); // 设置随机数种子
    int coordinates_x,coordinates_y;
    int total = 0;
    int score[5] = {};
    for (int i = 0; i < 300; i++) {
        coordinates_x = rand() % 200; // 随机生成行索引
        coordinates_y = rand() % 200; // 随机生成列索引
        if(blockedmap[coordinates_x][coordinates_y]!=0){
            total++;
            int distance[5] = {};
            for(int j = 0;j < 5;j++){
                if (blockedmap[coordinates_x][coordinates_y]!=blockedmap[berth[j].x][berth[j].y])
                {
                    distance[j] = 10001;
                }else{
                    distance[j] = aStarDispatch(coordinates_x,coordinates_y,berth[j].x,berth[j].y);
                }
            }
            int min = 10000;
            int min_index = -1;
            for (int j = 0;j < 5;j++){
                if (distance[j]<min)
                {
                    min = distance[j];
                    min_index = j;
                }  
            }
            if (min_index!=-1)
            {
                score[min_index]++;
            }
        }
    }
    for (int j = 0;j < 5;j++){
        berth[j].convenience = (float)score[j] / total;             
    }
    Berth::blockrate = (float)total / 300;

}


void init()//读取初始化数据
{
    int numth1 = 0;
    //读取地图将输入
    for(int i=0;i<200;i++)
    {
        for(int j=0;j<200;j++)
        {
            cin>>map[i][j];
            if (map[i][j] == 'A')
            {
                robot[numth1].x = i;
                robot[numth1].y = j;
                numth1++;
            }
        }
    }
    Berth::num = 10;
    //读取泊位信息
    for(int i=0;i<Berth::num;i++)
    {
        cin>>berth[i].id>>berth[i].x>>berth[i].y>>berth[i].time>>berth[i].velocity;
    }
    //读取船信息
    cin>>Ship::capacity;
    char ok[2];
    cin>>ok;
    while (std::cin.peek() != '\n' && std::cin.peek() != EOF) {
        std::cin.get();
    }
    //cerr<<ok;
    //广度优先搜索算法对地图分块
}

int sumofgoods = 0;

void ReadFrame()//读取每一帧的信息
{
    int timetmp = 0;
    cin >> timetmp >> moneyget;
    skipFrameSymbol = timetmp - nowtime -1;//刷新跳帧标志
    //cerr<<timetmp<<endl;

    if(skipFrameSymbol!=0){
        for (int i = 0; i < 10; i++)
        {
            //处理跳帧
            robot[i].route->time = robot[i].route->time + skipFrameSymbol;
        }

        for (int i = 0; i < 5; i++)
        {
            ship[i].time = ship[i].time + skipFrameSymbol;
        }
            //cerr<<"11111111111"<<" "<<skipFrameSymbol<<endl;
    }
    nowtime = timetmp;
    int n;//此帧场上新增的货物数量
    cin >> n;
    //改：广度优先算法对地图分块，不读取处于封闭区域刷新的物品

    //设置调度中断标志
    if(n!=0){
        goods.node->symbol=1;
    }else{
        goods.node->symbol=0;
    }
    //获取场上新增货物的信息
    for(int i=0;i<n;i++)
    {
        int x,y,price;
        cin >> x >> y >> price;
        //cerr << x << y << price <<endl;
        goods.node[goods.tail].x = x;
        goods.node[goods.tail].y = y;
        goods.node[goods.tail].price = price;
        goods.node[goods.tail].time = nowtime;
        goods.tail++;
        sumofgoods+=price;
    }
    for (int i = goods.head; i < goods.tail; i++)
    {
        if (goods.node[i].time < nowtime - 1000)
        {
            goods.head++;
        }
        break;
    }

    //获取此时场上机器人的信息
    for(int i = 0;i < 10;i++)
    {
        cin >> robot[i].iftake >> robot[i].x >> robot[i].y >> robot[i].status;
        //cerr << robot[i].iftake << robot[i].x << robot[i].y << robot[i].status <<endl;
    }
    //获取此时场上船的信息
    for(int i = 0;i < 5;i++)
    {
        cin >> ship[i].status >> ship[i].BerthId;
        //cerr << ship[i].status << ship[i].BerthId <<endl;
    }

    //判断之前的货物是否消失
    for(int i = 0;i < goods.tail;i++)
    {
        if(nowtime - goods.node[i].time > 1000)
        {
            goods.node[i].validSymbol = 0;//表示货物已经消失
        }
    }

    //此时判题器会给出OK
    char ok[2];
    cin >> ok;
    //cerr << ok;
    while (std::cin.peek() != '\n' && std::cin.peek() != EOF) {
        std::cin.get();
    }
}

void ShipDispatch()//船调度
{
    float berthvalue[10] = {0};
    int berthgoodsprice[10] = {0};//每个港口的货物价格
    for(int shipid = 0;shipid<5;shipid++) {
        for (int i = 0; i < Berth::num; i++) {
            int j = berth[i].goodsprice.head;
            for (int k = 0; k < Ship::num; k++) {
                if (ship[k].Berthdispatch == i) {
                    if (berth[i].goodsprice.tail - berth[i].goodsprice.head > Ship::capacity) {
                        j = j + ship[k].capacity;
                    } else {
                        j = berth[i].goodsprice.tail;
                    }
                }
            }

            for (; j <= berth[i].goodsprice.tail; j++) {
                berthgoodsprice[i] += berth[i].goodsprice.node[j];
            }

            float w1 = 1.5;
            float w2 = 0;
            float w3 = 0;
            berthvalue[i] = berthgoodsprice[i] * w1 + berth[i].time * w2 + berth[i].convenience * w3 + rand() % 200;
            if (14998 - nowtime < berth[i].time * 2)//来不及的港口不参与分配
            {
                berthvalue[i] = -9999999;
            }
            //港口分数 = 堆积货物价格 / 到达港口所需时间
        }

            if (ship[shipid].status == 1 && ship[shipid].ifwait == 1 && ship[shipid].BerthId == -1) {
                int max = 0;
                for (int j = 0; j < Berth::num; j++) {
                    if (berthvalue[j] > berthvalue[max]) {
                        max = j;
                    }
                }
                ship[shipid].Berthdispatch = max;
                ship[shipid].ifwait = 0;
                ship[shipid].time = nowtime;
            }

    }
}