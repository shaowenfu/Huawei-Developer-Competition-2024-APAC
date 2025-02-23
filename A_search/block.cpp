#ifndef MY_UNIQUE_HEADER_H_O
#define MY_UNIQUE_HEADER_H_O

#include "define.cpp"

const int MAP_SIZE = 200; // 地图大小
int regionCount = 0; // 全局变量，用于跟踪分块数量

vector<vector<int> > segmentedMap(MAP_SIZE, vector<int>(MAP_SIZE, 0)); // 初始化结果地图
vector<vector<char> > blockedmap(MAP_SIZE, vector<char>(MAP_SIZE, '0')); // '0' 代表海洋或障碍
// 四个方向的行列偏移量
const int dx[] = {-1, 1, 0, 0};
const int dy[] = {0, 0, -1, 1};

//将数组转化为vectro<vector<char>>类型
void arrayToVector(char map[200][200],vector<vector<char> >& segmentedMap){
    for (int i = 0; i < 200; i++)
    {
        for (int j = 0; j < 200; j++)
        {
            segmentedMap[i][j] = map[i][j];
        }
    }
}

// DFS函数，用于标记相互连接的陆地
void dfs(vector<vector<char> >& map, int x, int y, int regionId) {
    // 检查边界条件和是否已经是其他区域或障碍物/海洋
    if (x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE || segmentedMap[x][y] != 0 || (map[x][y] != '.' && map[x][y] != 'A' && map[x][y] != 'B')) {
        return;
    }

    // 标记当前位置为当前区域
    segmentedMap[x][y] = regionId;
    blockedmap[x][y] = char (segmentedMap[x][y]);
    // 递归搜索四个方向
    for (int i = 0; i < 4; ++i) {
        dfs(map, x + dx[i], y + dy[i], regionId);
    }
}

// 分割地图的函数
void segmentMap(vector<vector<char> >& map) {
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            if (map[i][j] == '.' && segmentedMap[i][j] == 0) {
                ++regionCount; // 增加区域编号，以标记新的陆地部分
                dfs(map, i, j, regionCount);
            }
        }
    }
}

#endif