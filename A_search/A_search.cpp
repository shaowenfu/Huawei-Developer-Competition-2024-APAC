#ifndef MY_UNIQUE_HEADER_A_SEARCH
#define MY_UNIQUE_HEADER_A_SEARCH

#include "block.cpp"

#define ROWS 200
#define COLS 200

typedef struct Node {
    int row, col;  // 节点在迷宫中的位置
    int f, g, h;   // f(n), g(n), h(n)值
    struct Node* parent;  // 父节点
    struct Node* next;    // 指向下一个节点的指针
    struct Node* fordelete;    //仅用于释放内存
} Node;

// 创建Node
Node* createNode(int row, int col, int f, int g, int h, Node* parent) {
    Node* newNode = new Node();
    newNode->row = row;
    newNode->col = col;
    newNode->f = f;
    newNode->g = g;
    newNode->h = h;
    newNode->parent = parent;
    newNode->next = NULL;
    newNode->fordelete = NULL;
    return newNode;
}

// 检查节点是否在有效范围内
bool isValid(int row, int col) {
    return (row >= 0) && (row < ROWS) && (col >= 0) && (col < COLS);
}

// 检查节点是否是障碍物
bool isObstacle(char maze[][COLS], int row, int col) {
    if (maze[row][col]=='*'||maze[row][col]=='#')
    {
        return 1;
    }
    return 0;
}

// 检查节点是否为移动中的机器人,传入参数是预测的那一帧
bool isRobot(int row, int col,int time,int RobotId){
    for (int i = 0; i < 10; i++)
    {
        if (robot[i].route->validsymbol==true&&(time - robot[i].route->time)<=robot[i].route->steptotal)
        {
            if (i!=RobotId&&robot[i].route->location_x[time - robot[i].route->time]==row&&robot[i].route->location_y[time - robot[i].route->time]==col)
            {
                return 1;
            }
            if (time - robot[i].route->time>0&&i!=RobotId&&robot[i].route->location_x[time - robot[i].route->time - 1]==row&&robot[i].route->location_y[time - robot[i].route->time - 1]==col)
            {
                return 1;
            }
        }
    }
    if(time - robot[RobotId].route->time < 4){
        for (int i = 0; i < 10; i++){
            if (i!=RobotId&&robot[i].x == row&&robot[i].y == col)
            {
                return 1;
            }
        }
    }

    return 0;
}

// 检查节点是否是目标节点
bool isDestination(int row, int col, int destRow, int destCol) {
    return (row == destRow) && (col == destCol);
}

// 计算H值（启发式函数）
float calculateHValue(int row, int col, int destRow, int destCol) {
    return abs(row - destRow) + abs(col - destCol);
}

// 寻找开放列表中具有最小 f 值的节点
Node* findMinFNode(Node* openList) {
    if (openList == NULL)
    {
        return NULL;
    }
    
    Node* minNode = openList;
    Node* current = openList->next;
    while (current != NULL) {
        if (current->f < minNode->f || (current->f == minNode->f && current->g < minNode->g)) {
            minNode = current;
        }
        current = current->next;
    }
    return minNode;
}

//获取最后的路径长度
int Pathtime(Node* destNode) {
    int length = 0;
    while (destNode != NULL) {
        destNode = destNode->parent;
        //cerr<<destNode->row<<" "<<destNode->col<<endl;
        length++;
    }
    return length;
}

//将openlist中节点全部删除
void deleteNode(Node* openlist) {
    while (openlist != NULL) {
        Node* tmp = openlist;
        openlist = openlist->fordelete;
        delete tmp;
    }
}


int excessStep(int RobotId,Node* nextNode){
    return Pathtime(nextNode) - abs(robot[RobotId].route->x - robot[RobotId].x) - abs(robot[RobotId].route->y - robot[RobotId].y);
}

int lengthall(Node* destNode){
    int length = 0;
    while (destNode != NULL) {
        destNode = destNode->fordelete;
        //cerr<<destNode->row<<" "<<destNode->col<<endl;
        length++;
    }
    return length;
}

//寻路过程中的A*搜索算法
int aStarDispatch(int src_x,int src_y,int dest_x,int dest_y){
    // 坐标偏移，用于获取周围节点
    int rowOffset[] = {-1, 0, 0, 1};
    int colOffset[] = {0, -1, 1, 0};

    // 创建起始节点
    Node* startNode = createNode(src_x, src_y, 0, 0, 0, NULL);
    
    //初始化删除列表
    Node* deleteptr = startNode;

    // 初始化开放列表和闭合列表
    Node* openList = startNode;

    bool closedList[ROWS][COLS];
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            closedList[i][j] = false;
        }
    }

    while (true) {
        // 在开放列表中寻找具有最小f值的节点
        Node* nextNode = findMinFNode(openList);
        
        // 如果没有找到路径
        if (nextNode == NULL) {
            
            deleteNode(deleteptr);
            return 100000;
        }

        // 如果找到目标节点
        if (isDestination(nextNode->row, nextNode->col, dest_x, dest_y)) {
            int tmp = Pathtime(nextNode);
            deleteNode(deleteptr);
            return tmp;
        }
        // 将当前节点标记为已访问
        closedList[nextNode->row][nextNode->col] = true;

        // 从开放列表中移除当前节点
        if (openList == nextNode) {
            openList = openList->next;
            
        } else {
            Node* prevNode = openList;
            while (prevNode->next != nextNode) {
                prevNode = prevNode->next;
            }
            prevNode->next = nextNode->next;
            
        }
        // 遍历当前节点的相邻节点
        for (int i = 0; i < 4; ++i) {
            int newRow = nextNode->row + rowOffset[i];
            int newCol = nextNode->col + colOffset[i];
            if (!isValid(newRow, newCol))
            {
                continue;
            }
            if (isValid(newRow, newCol) && !isObstacle(map, newRow, newCol) && !closedList[newRow][newCol]) {
                int gValue = nextNode->g + 1;
                int hValue = calculateHValue(newRow, newCol, dest_x, dest_y);
                int fValue = gValue + hValue;
                Node* newNode = createNode(newRow, newCol, fValue, gValue, hValue, nextNode);
                // 将新节点加入开放列表
                newNode->next = openList;
                openList = newNode;
                //将新节点加入删除列表
                newNode->fordelete = deleteptr;
                deleteptr = newNode;
                // 将新节点标记为已访问
                closedList[newRow][newCol] = true;
            }
        }
    }
}




// A*搜索算法
void aStarSearch(char maze[][COLS], int RobotId) {

    //如果是未调度机器人或者报废机器人，直接返回
    if(robot[RobotId].ifwait==1||robot[RobotId].status==0){
        return;
    }
    //搜索算法开始即给机器人起始时间赋值，并获取目的地
    robot[RobotId].route->time = nowtime;
    int destRow = robot[RobotId].route->x;
    int destCol = robot[RobotId].route->y;
    //记录步数
    int time = 0;
    //获取机器人出发地点
    int srcRow = robot[RobotId].x;
    int srcCol = robot[RobotId].y;
    // 坐标偏移，用于获取周围节点
    int rowOffset[] = {-1, 0, 0, 1};
    int colOffset[] = {0, -1, 1, 0};
    // 创建起始节点
    Node* startNode = createNode(srcRow, srcCol, 0, 0, 0, NULL);
    
    //初始化删除列表
    Node* deleteptr = startNode;
    //cerr<<"bbbbbbbbbbbb"<<endl;
    // 初始化开放列表和闭合列表
    Node* openList = startNode;
    bool closedList[ROWS][COLS];
    for (int i = 0; i < ROWS; ++i) {
        for (int j = 0; j < COLS; ++j) {
            closedList[i][j] = false;
        }
    }
    while (true) {
        // 在开放列表中寻找具有最小f值的节点
        Node* nextNode = findMinFNode(openList);
        // 如果没有找到路径(绕路超过120步就是没找到路径、总搜索超过998)
        if (nextNode == NULL || Pathtime(nextNode) > 998) {
            robot[RobotId].route->validsymbol = false;
            deleteNode(deleteptr);
            return;
        }
        // 如果找到目标节点
        if (isDestination(nextNode->row, nextNode->col, destRow, destCol)) {
            time = Pathtime(nextNode) - 1;
            //cerr<<"time:"<<time<<endl;
            if (time>998)
            {
                robot[RobotId].route->validsymbol = false;
                return;
            }
            //cerr<<time<<endl;
            robot[RobotId].route->validsymbol = true;
            robot[RobotId].route->steptotal = time;
            //将节点存入机器人路线中
            for (int j = 0; j < time + 1; j++)
            {
                robot[RobotId].route->location_x[time - j] = nextNode->row;
                robot[RobotId].route->location_y[time - j] = nextNode->col;
                //cerr<<nextNode->row<<" "<<nextNode->col<<endl;
                nextNode = nextNode->parent;
            }
            int x_Offset = 0;
            int y_Offset = 0;
            //根据机器人路线，分析每一帧动作
            
            for (int j = 0; j < time ; j++)
            {
                x_Offset = robot[RobotId].route->location_x[j+1] - robot[RobotId].route->location_x[j];
                y_Offset = robot[RobotId].route->location_y[j+1] - robot[RobotId].route->location_y[j];
                if (x_Offset==0&&y_Offset==-1)
                {
                    robot[RobotId].route->step[j] = 1;//左
                }else if (x_Offset==0&&y_Offset==1)
                {
                    robot[RobotId].route->step[j] = 0;//右
                }else if (x_Offset==-1&&y_Offset==0)
                {
                    robot[RobotId].route->step[j] = 2;//上
                }else if (x_Offset==1&&y_Offset==0)
                {
                    robot[RobotId].route->step[j] = 3;//下
                }else{
                    robot[RobotId].route->step[j] = -1;//静止
                }
            }

            deleteNode(deleteptr);
            //deleteNode(openList);
            return;
        }
        // 将当前节点标记为已访问
        closedList[nextNode->row][nextNode->col] = true;
        // 从开放列表中移除当前节点
        if (openList == nextNode) {
            openList = openList->next;
            
        } else {
            Node* prevNode = openList;
            while (prevNode->next != nextNode) {
                prevNode = prevNode->next;
            }
            prevNode->next = nextNode->next;
            
        }
        // 遍历当前节点的相邻节点
        for (int i = 0; i < 4; ++i) {
            int newRow = nextNode->row + rowOffset[i];
            int newCol = nextNode->col + colOffset[i];
            //cerr<<"1"<<endl;
            if (!isValid(newRow, newCol))
            {
                continue;
            }
            //cerr<<"2";            
            if (!isObstacle(maze, newRow, newCol) && !closedList[newRow][newCol] && !isRobot(newRow, newCol,nowtime + Pathtime(nextNode),RobotId)) {
                int gValue = nextNode->g + 1;
                int hValue = calculateHValue(newRow, newCol, destRow, destCol);
                int fValue = gValue + hValue;
                Node* newNode = createNode(newRow, newCol, fValue, gValue, hValue, nextNode);
                // 将新节点加入开放列表
                newNode->next = openList;
                openList = newNode;
                //将新节点加入删除列表
                newNode->fordelete = deleteptr;
                deleteptr = newNode;
                // 将新节点标记为已访问
                closedList[newRow][newCol] = true;
            }
            //cerr<<"3";
        }
        //cerr<<"eeeeeeeeeeeeeee"<<endl;
    }
}

#endif
