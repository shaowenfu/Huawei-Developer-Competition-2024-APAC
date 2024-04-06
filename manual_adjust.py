import subprocess
import random
import re
import os
individual =     {'JUDGE': 40, 'COUNT_MAX': 9, 'para1': -7.45804344086446, 'para2': 6.926483532385132, 'para3': -3.59209838157526, 'weight_timeToBerth': -17.834272655829352, 'weight_time': -0.31666172070769427, 'weight_velocity': 11.36829703541864, 'weight_convenience': 14.47806325748173, 'weight_goods_num': 3.981075784035733, 'w1': 16.99149236649886, 'w2': -7.527114663014457, 'w3': 5.852971807391061, 'ideal_ratio': 0.9068232027781495}
# 编译项目代码
def compile_code():
    subprocess.run("g++ -c main.cpp", shell=True)
    subprocess.run("g++ main.o -o main", shell=True)


def main():
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

    # 将参数传递给C++程序并运行
    cmd = f".\PreliminaryJudge.exe -l ERR -m .\map-3.9.txt .\main.exe "
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    output = result.stdout
    # 使用字符串格式化将individual的每个字段值打印出来，用空格隔开
    print(
        f"{individual['JUDGE']} {individual['COUNT_MAX']} {individual['para1']} {individual['para2']} {individual['para3']} {individual['weight_timeToBerth']} {individual['weight_time']} {individual['weight_velocity']} {individual['weight_convenience']} {individual['weight_goods_num']}")
    # 提取 stderr 字符串并分割成列表
    stderr_part = result.stderr.split("\n")
    for line in stderr_part:
        print(line)

    # 提取 stdout 字符串
    stdout_part = result.stdout
    print(stdout_part)

if __name__ == '__main__':
    main()