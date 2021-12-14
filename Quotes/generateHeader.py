#!/usr/bin/python3
import json
import re

headerString = "Quote QuoteDatas[] = {\n"

with open('AllQuotesWithoutDuplicates.json', encoding="utf-8") as f:
    d = json.load(f)
    for e in d:
        headerString += 'Quote{' + json.dumps(e["textBefore"]) + ', ' + json.dumps(e["timeText"]) + ', ' + json.dumps(e["textAfterTime"]) + ', ' + json.dumps(e["author"]) + ', ' + json.dumps(e["title"]) + ', ' + e["timeHour"] + ', ' + e["timeMinute"] + ', '+ e["nsfw"] +'},\n'

headerString += "}"


f = open("completeQuoteObject.h", "w", encoding="utf-8")
f.write(headerString)
f.close()
