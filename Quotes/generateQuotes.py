import re

quoteArray = "Quote quotes[] = {\n"

with open('quotes.csv', encoding="utf-8") as f: 
    for line in f:
        l = line.rstrip()
        l = re.sub(r'(\"|\'|‘|“)+', "\\\"", l)
        quote = l.split("|")
        start = quote[2].find(quote[1])
        end = start + len(quote[1])
        string = quote[2][:start] + "|" + quote[2][start:end] + "|"+ quote[2][end:]
        time = quote[0].split(":")
        quoteArray += "Quote{\"" + string + "\", \"" + quote[4] + "\", \"" + quote[3] + "\", " + str(int(time[0])) + ", " + str(int(time[1])) + "},\n"

quoteArray += "};"

f = open("quoteObject.h", "w", encoding="utf-8")
f.write(quoteArray)
f.close()
        


