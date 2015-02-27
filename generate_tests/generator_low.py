#!/usr/bin/python

import random
import os, subprocess
import sys

dir_name = "tests_low"
number_of_tests = 100
if len(sys.argv) == 2:
    number_of_tests = int(sys.argv[1])
number_of_msgs = 2000
msgSizeMin = 0
msgSizeMax = 1000
periodMin = 1000
periodMax = 1000000
tMaxMin = 100
tMaxMax = 100000
destNumMin = 1
destNumMax = 3

numberOfGroups = 8
partGroups = [[9, 10], [11,12], [13,14,15], [16], [17,18], [19, 20], [21,22], [23,24]]

def generate_one_df(fromPartition, toPartitions, jMax, msgSize, period, tMax, id):
    dest = ""
    for part in toPartitions:
        dest += str(part) + ','
    dest = dest[:-1]
    ans = '\t\t<dataFlow dest="' + dest + '" id="Data Flow ' + str(id) + '" jMax="' + str(jMax) + '" msgSize="' + str(msgSize) + '" period="' + str(period)
    ans += '" source="' + str(fromPartition) + '" tMax="' + str(tMax) + '" vl="None"/>'

    return ans

def generateSource():
    group = random.randint(0, numberOfGroups - 1)
    id = random.randint(0, len(partGroups[group])- 1)
    return {'id': partGroups[group][id], 'group': group}

def generateDests(sourceGroup, destNumber):
    dests = []
    for i in range(destNumber):
        id = -1
        while id == -1 or (id in dests):
            group = sourceGroup

            while group == sourceGroup:
                group = random.randint(0, numberOfGroups - 1)
            index = random.randint(0, len(partGroups[group])- 1)
            id = partGroups[group][index]

        dests.append(id)
    return dests

def generate_random_df(id):
    msgSize = random.randint(msgSizeMin, msgSizeMax)
    period = random.randint(periodMin, periodMax)
    tMax = random.randint(tMaxMin, tMaxMax)
    destNumber = random.randint(destNumMin, destNumMax)
    sourceInfo = generateSource()
    dests = generateDests(sourceInfo['group'], destNumber)
    return generate_one_df(sourceInfo['id'], dests, 0, msgSize, period, tMax, id)

def generateOneTest():
    f = open('test_arch.afdxxml', 'r')
    textArr = f.readlines()
    text = "".join(textArr[:-2])
    for id in range(1, number_of_msgs + 1):
        text += generate_random_df(id) + os.linesep
    text += "\t</dataFlows>" + os.linesep
    text += "</afdxxml>" + os.linesep
    f.close()
    return text

num_of_requests = number_of_msgs
num_of_assigned = {'lim_1': 0, 'lim_2': 0, 'lim_3': 0, 'without_source': 0, 'without_resp': 0, 'without_lim': 0, 'without_redesign': 0}
num_of_vls = {'lim_1': 0, 'lim_2': 0, 'lim_3': 0, 'without_source': 0, 'without_resp': 0, 'without_lim': 0, 'without_redesign': 0}
av_time = {'lim_1': 0, 'lim_2': 0, 'lim_3': 0, 'without_source': 0, 'without_resp': 0, 'without_lim': 0, 'without_redesign': 0}

configs = {'lim_1': '--limited-search-depth=1', 'lim_2': '--limited-search-depth=2', 'lim_3': '--limited-search-depth=3', 'without_source': '--disable-aggr-on-source', 'without_resp': '--disable-aggr-on-resp-time', 'without_lim': '--disable-limited-search', 'without_redesign': '--disable-redesign'}

def calc_num_of_assigned():
    process = subprocess.Popen(['grep', 'vl="None"', "out.afdxxml", "-c"], stdout=subprocess.PIPE)
    output = int(process.communicate()[0])
    return num_of_requests - output

def calc_num_of_vls():
    process = subprocess.Popen(['grep', '<virtualLink ', "out.afdxxml", "-c"], stdout=subprocess.PIPE)
    output = int(process.communicate()[0])
    print output
    return output

def generate_and_run():
    if not os.path.exists(dir_name):
        os.makedirs(dir_name)

    for i in range(1, number_of_tests+1):
        fileName = dir_name + '/' + 'test_'+ str(i) +'.afdxxml'
        f = open(fileName, 'w')
        f.write(generateOneTest())
        f.close()

        if sys.platform.startswith("win"):
            name = "../algo/AFDX_DESIGN.exe"
        else:
            name = "../algo/AFDX_DESIGN"

        for config in configs.keys():
            process = subprocess.Popen('time ' + name + ' ' + fileName + ' out.afdxxml ' + " a " + configs[config],  stdout=subprocess.PIPE,  stderr=subprocess.PIPE, shell=True)
            print config + " for test " + str(i) + " done"
            output = process.communicate()
            time = output[1].split()[3]

            try:
                minutes = float(time.split('m')[0])
                secs = float(time.split('m')[1][:-1])
            except:
                print "exception!"
                print time
                return;
            time = minutes*60 + secs
            av_time[config] += time

            num_of_assigned[config] += calc_num_of_assigned()
            num_of_vls[config] += calc_num_of_vls()

    for config in configs.keys():
        ans = config
        ans += '\t' + str(num_of_requests)
        ans += '\t' + str(int(float(num_of_assigned[config]) / number_of_tests))
        ans += '\t' + str(int(float(num_of_vls[config]) / number_of_tests))
        ans += '\t' + str(av_time[config]/number_of_tests)
        print ans

generate_and_run()
