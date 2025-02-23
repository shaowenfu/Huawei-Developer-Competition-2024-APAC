import json
import subprocess
import random
import os

# 设置离散参数值
discrete_values = {
    'JUDGE': [10,20, 30, 40, 50, 55, 60, 65, 70, 75, 80, 90, 100, 120],
    'COUNT_MAX': [2, 3, 4, 6, 7, 8,9,10, 13,15, 17,19,30]
}

# 设置每个参数范围
para_range = {
    'para1': (-8, 0),
    'para2': (0, 10),
    'para3': (-6, 0),
    'weight_timeToBerth': (-20, 0),
    'weight_time': (-5, 0),
    'weight_velocity': (0, 15),
    'weight_convenience': (0, 15),
    'weight_goods_num': (0, 15),
    'w1': (0, 20),
    'w2': (-10, 0),
    'w3': (0, 50),
    'ideal_ratio': (0, 1)
}


# 编译项目代码
def compile_code():
    subprocess.run("g++ -c main.cpp", shell=True)
    subprocess.run("g++ main.o -o main", shell=True)


# 计算适应度函数
def fitness(individual, i):
    print("\n第" + str(i) + "次:")
    # 将个体参数传递给C++程序并运行，计算得到适应度
    compile_code()
    # 通过环境变量传递参数
    os.environ['JUDGE'] = str(individual['JUDGE'])
    os.environ['COUNT_MAX'] = str(individual['COUNT_MAX'])
    os.environ['para1'] = str(individual['para1'])
    os.environ['para2'] = str(individual['para2'])
    os.environ['para3'] = str(individual['para3'])
    os.environ['weight_timeToBerth'] = str(individual['weight_timeToBerth'])
    os.environ['weight_time'] = str(individual['weight_time'])
    os.environ['weight_velocity'] = str(individual['weight_velocity'])
    os.environ['weight_convenience'] = str(individual['weight_convenience'])
    os.environ['weight_goods_num'] = str(individual['weight_goods_num'])
    os.environ['w1'] = str(individual['w1'])
    os.environ['w2'] = str(individual['w2'])
    os.environ['w3'] = str(individual['w3'])
    os.environ['ideal_ratio'] = str(individual['ideal_ratio'])
    # 使用字符串格式化将individual的每个字段值打印出来
    print("本次参数：", individual)
    # 将参数传递给C++程序并运行
    cmd = f".\PreliminaryJudge.exe -l ERR -m .\map-3.7.txt .\main.exe "
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    output = result.stdout
    # 提取 stderr 字符串并分割成列表
    stderr_part = result.stderr.strip().split("\n")
    for line in stderr_part:
        print(line)

    # 提取 stdout 字符串
    stdout_part = result.stdout.strip()
    print(stdout_part)

    # 从 stdout 中提取 JSON 字符串
    json_str = result.stdout.strip()

    # 将 JSON 字符串解析为字典
    result_dict = json.loads(json_str)

    # 获取 "score" 字段的值
    score = result_dict.get("score")

    # 打印提取的分数
    print("Score:", score)
    return score


# 随机搜索主函数
def random_search(num_iterations):
    best_score = float('-inf')
    best_params = None
    i = 0

    for _ in range(num_iterations):
        i = i + 1
        individual = {}
        for param, values in discrete_values.items():
            individual[param] = random.choice(values)

        for param, (min_val, max_val) in para_range.items():
            individual[param] = random.uniform(min_val, max_val)

        score = fitness(individual, i)

        if score > best_score:
            best_score = score
            best_params = individual
        print("目前最大分数：", best_score)
        print(f"目前最大分数的参数：", best_params)

    return best_params, best_score


# 调用随机搜索进行参数优化
best_params, best_score = random_search(num_iterations=10000)
print("Best Parameters:", best_params)
print("Best Score:", best_score)
