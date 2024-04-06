import subprocess
import random
import re
import os

# 设置离散参数值
discrete_values = {
    'JUDGE': [50, 70, 100, 120],
    'COUNT_MAX': [4, 6, 7, 17]
}

# 设置每个参数范围
para_range = {
    'para1': (-4, -1),
    'para2': (2, 8),
    'para3': (-4, -1),
    'weight_timeToBerth': (-16, -2),
    'weight_time': (-1, 0),
    'weight_velocity': (5, 15),
    'weight_convenience': (5, 15),
    'weight_goods_num': (5, 15)
}


# 编译项目代码
def compile_code():
    subprocess.run("g++ -c main.cpp", shell=True)
    subprocess.run("g++ main.o -o main", shell=True)


# 计算适应度函数
def fitness(individual):
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
    print("参数表:", individual)
    # 将参数传递给C++程序并运行
    cmd = f".\PreliminaryJudge.exe -l ERR -m .\map-3.11.txt .\main.exe "
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    output = result.stdout
    # 使用字符串格式化将individual的每个字段值打印出来，用空格隔开
    # print(
    #     f"{individual['JUDGE']} {individual['COUNT_MAX']} {individual['para1']} {individual['para2']} {individual['para3']} {individual['weight_timeToBerth']} {individual['weight_time']} {individual['weight_velocity']} {individual['weight_convenience']} {individual['weight_goods_num']}")
    # 提取 stderr 字符串并分割成列表
    stderr_part = result.stderr.split("\n")
    for line in stderr_part:
        print(line)

    # 提取 stdout 字符串
    stdout_part = result.stdout
    print(stdout_part)

    scores = re.findall(r"moneyget:(\d+)", output)
    total_score = sum(map(int, scores))
    return total_score


# 遗传算法主函数
def genetic_algorithm(pop_size, num_generations, mutation_rate):
    population = init_population(pop_size)
    for generation in range(num_generations):
        print("Generation:", generation + 1)
        selected = selection(population, pop_size // 2)
        offspring = []
        while len(offspring) < pop_size:
            parent1 = random.choice(selected)
            parent2 = random.choice(selected)
            child1, child2 = crossover(parent1, parent2)
            child1 = mutation(child1, mutation_rate)
            child2 = mutation(child2, mutation_rate)
            offspring.extend([child1, child2])
        population = offspring
        best_individual = max(population, key=lambda x: fitness(x))
        best_score = fitness(best_individual)
        print("Best Score:", best_score)
    return best_individual


# 初始化种群
def init_population(pop_size):
    population = []
    for _ in range(pop_size):
        individual = {}
        for param in discrete_values.keys():
            individual[param] = random.choice(discrete_values[param])
        for param, (min_val, max_val) in para_range.items():
            individual[param] = random.uniform(min_val, max_val)
        population.append(individual)
    return population


# 选择操作
def selection(population, num_selected):
    selected = sorted(population, key=lambda x: fitness(x), reverse=True)[:num_selected]
    return selected


# 交叉操作
def crossover(parent1, parent2):
    child1 = {}
    child2 = {}
    for param in para_range.keys():
        # 随机选择父代中的一个个体参数
        if random.random() < 0.5:
            child1[param] = parent1[param]
            child2[param] = parent2[param]
        else:
            child1[param] = parent2[param]
            child2[param] = parent1[param]
    return child1, child2


# 变异操作
def mutation(individual, mutation_rate):
    mutated_individual = {}
    for param, (min_val, max_val) in para_range.items():
        # 根据变异率决定是否对该参数进行变异
        if random.random() < mutation_rate:
            # 在参数的范围内随机生成新值
            mutated_individual[param] = random.uniform(min_val, max_val)
        else:
            mutated_individual[param] = individual[param]
    return mutated_individual


# 调用遗传算法进行参数优化
best_params = genetic_algorithm(pop_size=10, num_generations=10, mutation_rate=0.2)
print("Best Parameters:", best_params)
