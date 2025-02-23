#include "A_search.cpp"

#define JUDGE 85
#define COUNT_MAX 15

// 计算机器人 i 选择货物 j 去到港口 k 的收益
float calculateGoodsProfit(int i, int j, int k)
{
    int timeToGoods = abs(robot[i].x - goods.node[j].x) + abs(robot[i].y - goods.node[j].y); // 到达货物的时间,暂时用距离代替
    int goodsValue = goods.node[j].price;                                                    // 货物的价值
    int time_left = 1000 - nowtime + goods.node[j].time;                                     // 货物剩余停留时间
    int timeToBerth = abs(goods.node[j].x - berth[k].x) + abs(goods.node[j].y - berth[k].y); // 到达港口的时间
    float fit = time_left >= timeToGoods ? 0 : -100000000;
    float para[3] = {-2, 10.25, -2.5};                                                                                   // 参数列表
    float totalProfit = (float)para[0] * timeToGoods + (float)para[1] * goodsValue + (float)para[2] / timeToBerth + fit; // 总收益
    // cerr << "manhadun1:" << timeToGoods << "   goodsprice:" << goodsValue << "   time:" << time_left << "   manhadun2:" << timeToBerth << "   fit:" << fit << endl;
    // cerr << "totalProfit:" << totalProfit << endl;
    //   cerr<<timeToGoods<<goodsValue<<timeToBerth<<totalProfit<<endl
    if (nowtime >= 12000)
    {
        int para4 = -1;
        totalProfit += berth[k].time * para4;
    }
    return totalProfit + 5000;
}

// 计算机器人 i 选择港口 k 的收益
float calculateBerthProfit(int i, int k)
{

    int goods_num = berth[k].goodsprice.tail - berth[k].goodsprice.head; // 港口囤积物品

    // 定义各个评分因素的权重
    float weight_timeToBerth = -3; // 到达港口时间的权重
    float weight_time = -0.15;     // 港口到虚拟点时间的权重
    float weight_velocity = 12;    // 港口装卸速度的权重
    float weight_convenience = 1;  // 港口便利程度的权重
    float weight_goods_num = 3.5;  // 港口囤积物品的权重

    // 根据评分因素计算各个部分的分数
    float score_timeToBerth = (float)abs(robot[i].x - berth[k].x) + (float)abs(robot[i].y - berth[k].y); // 到达港口的时间
    float score_time = berth[k].time;                                                                    // 港口到虚拟点时间
    float score_velocity = berth[k].velocity;                                                            // 该港口装卸速度
    float score_convenience = berth[k].convenience;                                                      // 该港口的便利程度指数（0-1）
    int capacity = Ship::capacity;                                                                       // 轮船容量

    // 计算港口囤积货物数量与轮船容量的比例
    float ratio = (float)goods_num / capacity;
    // 根据比例调整评分
    float score_goods_num; // 假设港口货物数量越接近capacity的2/3得分越高

    if (ratio < 1)
    {
        score_goods_num = 8 - pow(ratio - 1, 4);
    }
    else
    {
        score_goods_num = -1;
    }
    if (nowtime >= 12000)
    {
        weight_time = -1;
    }

    // 根据权重计算综合评分
    float totalScore = weight_timeToBerth * score_timeToBerth + weight_time * score_time +
                       weight_velocity * score_velocity + weight_convenience * score_convenience +
                       weight_goods_num * score_goods_num;


    // 打印每个变量的值
    // cerr << "    score_timeToBerth: " << score_timeToBerth << "    score_time: " << score_time << "    score_velocity: " << score_velocity << "   score_convenience: " << score_convenience << "   score_goods_num: " << score_goods_num << endl;
    // cerr << "totalScore:" << totalScore << endl;
    return totalScore + 5000;
}

struct Element
{
    float value;
    int index_goods;
    int index_berth;
};

// 返回从高到低，第count个大的score数组的第一个索引
Element getmax(vector<Element> &score, int count)
{
    if (count - 1 >= score.size())
    {
        Element elem;
        elem.value = 0;
        elem.index_berth = 0;
        elem.index_goods = 0;
        return elem;
    }
    // 初始化为前 count 个元素
    Element maxElements[count];
    for (int i = 0; i < count; ++i)
    {
        maxElements[i] = score[i];
    }

    // 遍历剩余元素，更新前 count 个最大的元素
    for (int i = count; i < score.size(); ++i)
    {
        for (int j = 0; j < count; ++j)
        {
            if (score[i].value > maxElements[j].value)
            {
                for (int k = count - 1; k > j; --k)
                {
                    maxElements[k] = maxElements[k - 1];
                }
                maxElements[j] = score[i];
                break;
            }
        }
    }

    // 返回第 count 个大的元素
    return maxElements[count - 1];
}

// 中断控制
void DispatchInterrupt()
{
    if (Goods::symbol == 1)
    {
        for (int i = 0; i < Robot::num; i++)
        {
            if (robot[i].iftake == 0 && robot[i].ifwait == 0)
            {
                robot[i].ifwait = 1;
                goods.node[robot[i].takegoods].validSymbol = 1;
            }
        }
        for (int i = 0; i < Robot::num; i++)
        {
            if (robot[i].status == 0)
            {
                // 如果机器人状态不正常，跳过
                continue;
            }

            // 如果机器人没有装货，寻找目标货物坐标
            if (robot[i].ifwait == 1 && robot[i].iftake == 0)
            {
                // 该机器人，物品，港口得分
                vector<Element> score;
                // 遍历货物
                for (int j = goods.head; j < goods.tail; ++j)
                {
                    if (goods.node[j].validSymbol == 0 || segmentedMap[goods.node[j].x][goods.node[j].y] != segmentedMap[robot[i].x][robot[i].y])
                    {
                        // 货物已经被安排运输或者不在一个区块,跳过
                        continue;
                    }
                    // 遍历码头
                    for (int k = 0; k < 10; ++k)
                    {
                        if (segmentedMap[berth[k].x][berth[k].y] != segmentedMap[robot[i].x][robot[i].y])
                        {
                            // 港口不在一个区块,跳过
                            continue;
                        }
                        // 计算路线评分
                        Element nowElement;
                        nowElement.value = calculateGoodsProfit(i, j, k);
                        nowElement.index_goods = j;
                        nowElement.index_berth = k;
                        score.push_back(nowElement);
                    }
                }
                int count = 1;
                int tmp; // 暂存比较结果
                Element currentElement;
                do
                {
                    if (count > COUNT_MAX)
                    {
                        break;
                    }
                    // 查找第count大的元素(count取值范围1-5)
                    currentElement = getmax(score, count);
                    // 如果目前最大的评分已经为0或者查询次数过多，退出，直接采用当前目标
                    if (currentElement.value <= -5000)
                    {
                        break;
                    }
                    // 调用A星搜索路径
                    // cerr << goods.node[currentElemnt.index_goods].x<<endl;
                    int astar_dist = aStarDispatch(robot[i].x, robot[i].y, goods.node[currentElement.index_goods].x, goods.node[currentElement.index_goods].y);
                    // 计算哈密尔顿路径长度
                    int Hamilton_lenth = abs(goods.node[currentElement.index_goods].x - robot[i].x) + abs(goods.node[currentElement.index_goods].y - robot[i].y);
                    tmp = astar_dist - Hamilton_lenth;
                    count++;
                } while (tmp > JUDGE);
                if (currentElement.value == 0)
                {
                    continue;
                }
                if (robot[i].takegoods != currentElement.index_goods)
                {
                    robot[i].takegoods = currentElement.index_goods;
                    robot[i].ifwait = 0; // 机器人置为非等待状态
                    robot[i].route->x = goods.node[currentElement.index_goods].x;
                    robot[i].route->y = goods.node[currentElement.index_goods].y;
                    goods.node[currentElement.index_goods].validSymbol = 0;
                    robot[i].route->validsymbol = 0;
                }
            }
        }
    }
}

// 机器人调度(总体上使用贪心算法实现）
void RobotDispatch()
{
    // 遍历机器人
    for (int i = 0; i < Robot::num; i++)
    {
        if (robot[i].status == 0)
        {
            // 如果机器人状态不正常，跳过
            continue;
        }
        // 如果机器人没有装货，寻找目标货物坐标
        if (robot[i].ifwait == 1 && robot[i].iftake == 0)
        {
            // 该机器人，物品，港口得分
            vector<Element> score;
            // 遍历货物
            for (int j = goods.head; j < goods.tail; ++j)
            {
                if (goods.node[j].validSymbol == 0 || segmentedMap[goods.node[j].x][goods.node[j].y] != segmentedMap[robot[i].x][robot[i].y])
                {
                    // 货物已经被安排运输或者不在一个区块,跳过
                    continue;
                }
                // 遍历码头
                for (int k = 0; k < 10; ++k)
                {
                    if (segmentedMap[berth[k].x][berth[k].y] != segmentedMap[robot[i].x][robot[i].y])
                    {
                        // 港口不在一个区块,跳过
                        continue;
                    }
                    // 计算路线评分
                    Element nowElement;
                    nowElement.value = calculateGoodsProfit(i, j, k);
                    nowElement.index_goods = j;
                    nowElement.index_berth = k;
                    score.push_back(nowElement);
                }
            }
            int count = 1;
            int tmp; // 暂存比较结果
            Element currentElement;
            do
            {
                if (count > COUNT_MAX)
                {
                    break;
                }
                // 查找第count大的元素(count取值范围1-5)
                currentElement = getmax(score, count);
                // 如果目前最大的评分已经为0或者查询次数过多，退出，直接采用当前目标
                if (currentElement.value <= -5000)
                {
                    break;
                }
                // 调用A星搜索路径
                // cerr << goods.node[currentElemnt.index_goods].x<<endl;
                int astar_dist = aStarDispatch(robot[i].x, robot[i].y, goods.node[currentElement.index_goods].x, goods.node[currentElement.index_goods].y);
                // 计算哈密尔顿路径长度
                int Hamilton_lenth = abs(goods.node[currentElement.index_goods].x - robot[i].x) + abs(goods.node[currentElement.index_goods].y - robot[i].y);
                tmp = astar_dist - Hamilton_lenth;
                count++;
            } while (tmp > JUDGE);
            if (currentElement.value <= -5000)
            {
                continue;
            }
            robot[i].takegoods = currentElement.index_goods;
            robot[i].ifwait = 0; // 机器人置为非等待状态
            robot[i].route->x = goods.node[currentElement.index_goods].x;
            robot[i].route->y = goods.node[currentElement.index_goods].y;
            goods.node[currentElement.index_goods].validSymbol = 0;
        }
        else if (robot[i].ifwait == 1 && robot[i].iftake == 1)
        {
            float score_index[10] = {};
            // 港口调度写在这里
            for (int k = 0; k < Berth::num; ++k)
            {
                score_index[k] = 0;
                if (segmentedMap[berth[k].x][berth[k].y] != segmentedMap[robot[i].x][robot[i].y])
                {
                    // 港口不在一个区块,跳过
                    continue;
                }
                // 计算路线评分
                score_index[k] = calculateBerthProfit(i, k);
            }

            // 初始化最大值和其索引
            float maxValue = score_index[0];
            int maxIndex = 0;
            // 遍历数组，找到最大值及其索引
            for (int j = 0; j < Berth::num; ++j)
            {
                if (score_index[j] > maxValue)
                {
                    maxValue = score_index[j];
                    maxIndex = j;
                }
            }
            if (maxValue == 0)
            {
                continue;
            }
            robot[i].route->x = detailposition(i, maxIndex).x;
            robot[i].route->y = detailposition(i, maxIndex).y;
            robot[i].ifwait = 0; // 机器人置为非等待状态
            robot[i].forBerth = maxIndex;
        }
    }
}
