#!/usr/bin/python3
import json
import re
from types import NoneType
from typing import Match

headerString = "Quote QuoteDatas[] = {\n"

with open('AllQuotesWithoutDuplicates.json', encoding="utf-8") as f:
    d = json.load(f)
    for e in d:
        filterList = ["A.M.", "P.M.", "No."]
        if len(e["textBefore"]) > 150:
            firstPoint = re.search(r'(?:(?<!A)(?<!P)(?<!No))\.', e["textBefore"])
            if type(firstPoint) is not NoneType and firstPoint.start() < len(e["textBefore"]) - 10:
                e["textBefore"] = e["textBefore"][firstPoint.start() + 2:]
            else:
                textArr = e["textBefore"].split(" ")
                while len(' '.join(textArr)) > 150:
                    del textArr[0]
                e["textBefore"] = "..." + ' '.join(textArr)
        headerString += 'Quote{' + json.dumps(e["textBefore"]) + ', ' + json.dumps(e["timeText"]) + ', ' + json.dumps(e["textAfterTime"]) + ', ' + json.dumps(e["author"]) + ', ' + json.dumps(e["title"]) + ', ' + e["timeHour"] + ', ' + e["timeMinute"] + ', '+ e["nsfw"] +'},\n'

headerString += "};"


f = open("completeQuoteObject.h", "w", encoding="utf-8")
f.write(headerString)
f.close()
