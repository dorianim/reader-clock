#!/usr/bin/python3
import re
import json
import time
import difflib


def sanityze(string):
    string = re.sub(r'("|“|”)+', '\"', string)
    string = re.sub(r'(‘|’|′|´)+', "'", string)
    string = re.sub(r'(ë|é|ê|è)+', "e", string)
    string = re.sub(r'(—|–|−)+', "-", string)
    string = re.sub('ö', "oe", string)
    string = re.sub('Ö', "Oe", string)
    string = re.sub('ä', "ae", string)
    string = re.sub('ü', "ue", string)
    string = re.sub('š', "s", string)
    string = re.sub('í', "i", string)
    string = re.sub(r'(á|à|å)', "a", string)
    string = re.sub('É', "E", string)
    string = re.sub('…', "...", string)
    string = re.sub(r'(ø|ó)+', "o", string)
    string = re.sub('°', " degrees", string)
    string = re.sub('£', "$", string)
    string = re.sub('č', "c", string)
    string = re.sub('ñ', "n", string)
    return string


def add_line(line):
    quote = line.split("|")

    timeTextStart = quote[2].lower().find(quote[1].lower())
    timeTextEnd = timeTextStart + len(quote[1])

    textBeforeTime = sanityze(quote[2][:timeTextStart])
    timeText = sanityze(quote[2][timeTextStart:timeTextEnd])
    textAfterTime = sanityze(quote[2][timeTextEnd:])

    author = sanityze(quote[4])
    title = sanityze(quote[3])

    time = quote[0].split(":")
    return {"textBefore": textBeforeTime, "timeText": timeText, "textAfterTime": textAfterTime, "author": author, "title": title, "timeHour": str(time[0]), "timeMinute": str(int(time[1])), "nsfw": "false"}


def insertCorrectTime(arr, index):
    arr[index]["timeHour"] = str(int(arr[index]["timeHour"]))
    arr[index]["timeMinute"] = str(int(arr[index]["timeMinute"]))


def mergeSort(list1, list2):
    arr = []
    i = j = 0

    while i < len(list1) and j < len(list2):
        if(time.strptime(list1[i]["timeHour"] + list1[i]["timeMinute"], "%H%M") < time.strptime(list2[j]["timeHour"] + list2[j]["timeMinute"], "%H%M")):
            arr.append(list1[i])
            i += 1
        else:
            arr.append(list2[j])
            j += 1
        insertCorrectTime(arr, len(arr)-1)

    while i < len(list1):
        arr.append(list1[i])
        i += 1
        insertCorrectTime(arr, len(arr)-1)

    while j < len(list2):
        arr.append(list2[j])
        j += 1
        insertCorrectTime(arr, len(arr)-1)
    return arr


quoteList1 = []
quoteList2 = []

with open('jaap-meijers/quotes.csv', encoding="utf-8") as f:
    for line in f:
        line = line.rstrip()
        quoteList1.append(add_line(line))

with open('literaryclock/quotes.csv', encoding="utf-8") as f2:
    for line in f2:
        line = line.rstrip()
        quoteList2.append(add_line(line))

list = mergeSort(quoteList1, quoteList2)

seen = []
dupl = []
unique = []
while list:
    x = list.pop(0)
    if(len(seen) > 0):
        checkIndex = len(seen) - 1
        seq = []
        while checkIndex >= 0 and seen[checkIndex]["timeHour"] == x["timeHour"] and seen[checkIndex]["timeMinute"] == x["timeMinute"]:
            seq.append(difflib.SequenceMatcher(a=''.join(e for e in str(seen[checkIndex]) if e.isalnum(
            )), b=''.join(e for e in str(x) if e.isalnum())).quick_ratio())
            checkIndex -= 1
        if any(seqValue > 0.9 for seqValue in seq):
            dupl.append(x)
        else:
            unique.append(x)
    seen.append(x)

f = open("AllQuotesWithoutDuplicates.json", "w", encoding="utf-8")
f.write(json.dumps(unique, indent=4))
f.close()

f = open("duplicates.json", "w", encoding="utf-8")
f.write(json.dumps(dupl, indent=4))
f.close()
