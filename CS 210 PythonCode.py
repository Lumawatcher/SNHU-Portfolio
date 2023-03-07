import re
import string

def getAllItems():
    with open('itemlist.txt', 'r') as myFile:
        itemString = myFile.read();
    itemList = itemString.split('\n')
    itemList.sort()
    itemDict = {}
    for item in itemList:
        if item in itemDict:
            itemDict[item] += 1
        else:
            itemDict.update({item: 1})
    for item in itemDict:
        print(item, end='')
        print(': ', end='')
        print(itemDict[item])

def getSingleItem(v):
    with open('itemlist.txt', 'r') as myFile:
        itemString = myFile.read();
    itemList = itemString.split('\n')
    itemDict = {}
    v = v.lower()
    v = v.capitalize()
    for item in itemList:
        if item in itemDict:
            itemDict[item] += 1
        else:
            itemDict.update({item: 1})
    if v in itemDict:
        return itemDict[v]
    else:
        return 0

def writeFrequencyFile():
    with open('itemlist.txt', 'r') as myFile:
        itemString = myFile.read();
    itemList = itemString.split('\n')
    itemList.sort()
    itemDict = {}
    for item in itemList:
        if item in itemDict:
            itemDict[item] += 1
        else:
            itemDict.update({item: 1})
    with open('frequency.dat', 'w') as myFile:
        for key in itemDict:
            myFile.write(key)
            myFile.write(' ')
            myFile.write(str(itemDict[key]))
            myFile.write('\n')
