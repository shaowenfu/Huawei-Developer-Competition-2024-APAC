#include"function.cpp"

bool ifrobotinberth(int RobotId,int BerthId){
    if(robot[RobotId].x >= berth[BerthId].x && robot[RobotId].x <= berth[BerthId].x + 3 && robot[RobotId].y >= berth[BerthId].y && robot[RobotId].y <= berth[BerthId].y + 3){
        return true;
    }
    return false;
}


void addtogoods(int robotid){
    for(int i = 0;i < Berth::num;i++){
        if(robot[robotid].x >= berth[i].x - 1 && robot[robotid].x <= berth[i].x + 5 && robot[robotid].y >= berth[i].y - 1 && robot[robotid].y <= berth[i].y + 5){
            berth[i].goodsprice.node[berth[i].goodsprice.tail] = goods.node[robot[i].takegoods].price;
            berth[i].goodsprice.tail++;
        }
    }
}
int sum = 0;
void execute()//根据调度结果执行，按顺序输出
{   
    //机器人路径执行
    //对所有被调度，但未规划路线的机器人进行路线搜索
    for(int i=0;i<10;i++){
        if (robot[i].ifwait==false&&robot[i].route->validsymbol==false)
        {
            aStarSearch(map,i);
        }
    }
    //对所有机器人下一帧位置进行判断，对即将相撞的两机器人路线设置为无效
    int next_x[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    int next_y[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    
    for (int i = 0; i < 10; i++)
    {    
        if (robot[i].status==0)
        {
            robot[i].route->validsymbol = 0;
        }
        if(robot[i].route->validsymbol == 1 && robot[i].status==1){
            next_x[i] = robot[i].route->location_x[nowtime - robot[i].route->time + 1];
            next_y[i] = robot[i].route->location_y[nowtime - robot[i].route->time + 1];
            if(ifrobotinberth(i,robot[i].forBerth)&&robot[i].iftake == 1){
                next_x[i] = robot[i].x;
                next_y[i] = robot[i].y;
            }
            
        }else{
            next_x[i] = robot[i].x;
            next_y[i] = robot[i].y;
        }
    }   
    for (int i = 0; i < 10; i++)
    {
        if (!isValid(next_x[i],next_y[i]))
        {
            robot[i].route->validsymbol = 0;
            continue;
        }
        if (isObstacle(map,next_x[i],next_y[i]))
        {
            robot[i].route->validsymbol = 0;
            continue;
        }
        for (int j = i + 1; j < 10; j++)
        {
            //防重合
            if (next_x[i]==next_x[j]&&next_y[i]==next_y[j])
            {
                robot[i].route->validsymbol = 0;
                robot[j].route->validsymbol = 0;
            }
            //防对撞
            if (next_x[i]==robot[j].x&&next_y[i]==robot[j].y&&robot[i].x==next_x[j]&&robot[i].y==next_y[j])
            {
                robot[i].route->validsymbol = 0;
                robot[j].route->validsymbol = 0;
            }
            
        }
    }

    //cerr<<robot[0].route->validsymbol<<endl;
    //cerr<<robot[0].route->steptotal<<endl;
    //cerr<<robot[0].ifwait<<endl;
    //根据预先设定路线行进
    for(int i=0;i<10;i++){
        if (robot[i].status==1&&ifrobotinberth(i,robot[i].forBerth)&&robot[i].iftake==1)
        {

            output.pull(i);
            addtogoods(i);
            sum += goods.node[robot[i].takegoods].price;

            //让机器人路线无效,并且重新等待调度
            robot[i].route->validsymbol=0;
            robot[i].ifwait = 1; 
        }
        
        //如果机器人所在位置与目标可以交互，则根据机器人状态进行交互
        if(robot[i].x==robot[i].route->x&&robot[i].y==robot[i].route->y&&robot[i].status==1&&robot[i].iftake == 0){
            output.get(i);
            //让机器人路线无效,并且重新等待调度
            robot[i].route->validsymbol=0;
            robot[i].ifwait = 1;     
        }

        if(robot[i].route->validsymbol==true&&robot[i].ifwait==0&&nowtime-robot[i].route->time<robot[i].route->steptotal){
            //cerr<<robot[i].route->step[nowtime-robot[i].route->time]<<endl;
            output.move(i,robot[i].route->step[nowtime-robot[i].route->time]);
        }
    }
    //船操作执行
    for(int i=0;i<5;i++) {
        if (ship[i].status == 1 && ship[i].ifwait == 0 && ship[i].BerthId == -1) {
            output.ship(i, ship[i].Berthdispatch);
            //cerr << "ship" << i << " " << ship[i].Berthdispatch <<" nowtime "<< nowtime << endl;
        }
        if(ship[i].status == 0 && ship[i].BerthId == -1){
            ship[i].ifwait = 1;
        }
        /*
        if(ship[i].ifwait == 0 && ship[i].status == 2 && ship[i].time + berth[ship[i].Berthdispatch].time <= nowtime){
            ship[i].time ++;
        }
         */
        if(ship[i].status != 0 && ship[i].ifwait == 0 && ship[i].time + berth[ship[i].Berthdispatch].time <= nowtime){
            if(ship[i].status == 2){
                output.go(i);
                //cerr << "go " << i <<" nowtime"<<nowtime<<endl;
                berth[ship[i].Berthdispatch].goodsprice.head =berth[ship[i].Berthdispatch].goodsprice.tail;
            }
            if(14990 - nowtime < berth[ship[i].Berthdispatch].time){
                output.go(i);
                //cerr << "go " << i <<" nowtime"<<nowtime<<endl;
            }
            if(ship[i].capacity / berth[ship[i].Berthdispatch].velocity+ berth[ship[i].Berthdispatch].time + ship[i].time <= nowtime){
                output.go(i);
                //cerr << "go " << i <<" nowtime"<<nowtime<<endl;
                berth[ship[i].Berthdispatch].goodsprice.head += ship[i].capacity;
            }

        }
    }
    //末尾OK
    output.OK();
    
}