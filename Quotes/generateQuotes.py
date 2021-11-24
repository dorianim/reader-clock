#!/usr/bin/python3
import re

def sanityze(string):
    string = re.sub(r'("|“|”)+', '\\\"', string)
    string = re.sub(r'(‘)+', "'", string)
    return string

quoteArray = "Quote QuoteDatas[] = {\n"

with open('quotes.csv', encoding="utf-8") as f: 
    for line in f:
        line = line.rstrip()
        quote = line.split("|")

        timeTextStart = quote[2].lower().find(quote[1].lower())
        timeTextEnd = timeTextStart + len(quote[1])

        textBeforeTime = sanityze(quote[2][:timeTextStart])
        timeText = sanityze(quote[2][timeTextStart:timeTextEnd])
        textAfterTime = sanityze(quote[2][timeTextEnd:])

        author = sanityze(quote[4])
        title = sanityze(quote[3])

        time = quote[0].split(":")
        quoteArray += 'Quote{"' + textBeforeTime + '", "' + timeText + '", "' + textAfterTime + '", "' + author + '", "' + title + '", ' + str(int(time[0])) + ', ' + str(int(time[1])) + '},\n'

quoteArray += "};"

f = open("quoteObject.h", "w", encoding="utf-8")
f.write(quoteArray)
f.close()
        


