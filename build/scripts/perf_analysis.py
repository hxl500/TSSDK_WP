#! /usr/bin/env python3
# SPDX-License-Identifier: MIT
# -*- python -*-
# -*- coding: utf-8 -*-

import sys, getopt
import matplotlib.pyplot as plt
#import string

processName=''
systemFree=[]
systemCpuUtilization=[]
processPss=[]
processCputilization=[]
timeStamp=[]

def parseLogFile(file):
    localFile = open(file,"r")
    lineList = localFile.readlines()
    for line in lineList:
        tmp = line.split()
        timeStampRecord(tmp)
        systemFreeMemCollect(tmp)
        systemCpuUtilizationCollect(tmp)
        processMemPssCollect(tmp)
        processCpuUtilizationCollect(tmp)

def plotShow():
    print('plotShow')
    global systemFree
    global systemCpuUtilization
    global processPss
    global processCputilization
    global timeStamp
    print('timeStamp: ' + str(timeStamp))
    print('systemFree: ' + str(systemFree))
    print('systemCpuUtilization: ' + str(systemCpuUtilization))
    print('processPss: ' + str(processPss))
    print('processCputilization: ' + str(processCputilization))
    plotNumber=0
    if len(systemFree) >= 1:
        plotNumber = plotNumber + 1;
    if len(systemCpuUtilization) >= 1:
        plotNumber = plotNumber + 1;
    if len(processPss) >= 1:
        plotNumber = plotNumber + 1;
    if len(processCputilization) >= 1:
        plotNumber = plotNumber + 1;

    print('plotNumber=' + str(plotNumber))

    plt.grid(True)
    plt.subplots_adjust(wspace=1, hspace=1)
    for number in range(1, plotNumber+1):
        print('plot ' + str(number) + ' of ' + str(plotNumber))
        plt.subplot(plotNumber, 1, number)
        if len(systemFree) >= 1:
            print('System Memory Free')
            plt.plot(timeStamp, systemFree, color='b')
            plt.xlabel("TimeStamp")
            plt.ylabel("Memory KB")
            plt.title("System Memory Free")
            systemFree=[]
        elif len(systemCpuUtilization) >= 1:
            print('systemCpuUtilization')
            plt.plot(timeStamp, systemCpuUtilization, color='r')
            plt.ylabel("cpu %")
            plt.title("System CPU Utilization")
            plt.xlabel("TimeStamp")
            systemCpuUtilization=[]
        elif len(processPss) >= 1:
            print('processPss')
            plt.plot(timeStamp, processPss, color='c')
            plt.ylabel("Memory KB")
            plt.title('Process ' + processName + ' Memory Pss')
            plt.xlabel("TimeStamp")
            processPss=[]
        elif len(processCputilization) >= 1:
            print('processCputilization')
            plt.plot(timeStamp, processCputilization, color='g')
            plt.ylabel("cpu %")
            plt.title('Process ' + processName + ' CPU Utilization')
            plt.xlabel("TimeStamp")
            processCputilization=[]
    plt.show()
    #plt.xlim(0,1000)
    #plt.ylim(0,10000)
    #plt.yscale('linear')

# vmstat -s
def systemFreeMemCollect(line):
    if "Mem:" in line: # Mem: 203816K used, 2450080K free, 175980K shrd, 0K buff, 175980K cached
        global systemFree
        systemFree.append(line[3].split("K")[0]) # memory free in column 3

# top -n 1
def systemCpuUtilizationCollect(line):
    if "CPU:" in line: # CPU: 8% usr 17% sys 0% nic 73% idle 0% io 0% irq 0% sirq
        global systemCpuUtilization
        systemCpuUtilization.append(100-int(float(line[7].split("%")[0]))) # 100% - cpu idle

# procrank -p
def processMemPssCollect(line):
    global processName
    global processPss
    if processName in line: # 1 2804K 1560K 339K 108K init
        if len(line) == 6: # procrank have 6 columns, column 3 is pss value
            processPss.append(line[3].split("K")[0])

# pidstat
def processCpuUtilizationCollect(line):
    global processName
    global processCputilization
    if processName in line: # 11:51:45 0 740 0.10 0.09 0.00 0.00 0.18 0 init
        if len(line) == 11: # pidstat have 11 columns, column 8 is cpu utilization
            processCputilization.append(line[8])

# date "+timestamp: %H:%M:%S"
def timeStampRecord(line):
    if "timestamp:" in line: # timestamp: 09:54:51
        global timeStamp
        timeStamp.append(line[1])

def main(argv):
    inputFile=""
    try:
        opts, args = getopt.getopt(argv, "hn:i:",["ifile=", "name="])
    except getopt.GetoptError:
        print('trace.py -i <inputfile>');
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('perf_analysis -i <inputFile> -n <processName>')
            sys.exit(0)
        elif opt in ("-i", "--ifile"):
            inputFile = arg
            print('input=' + inputFile)
        elif opt in ("-n", "--name"):
            global processName
            processName = arg
            print('processName=' + processName)
    parseLogFile(inputFile)
    plotShow()

if __name__ == '__main__':
    main(sys.argv[1:])
