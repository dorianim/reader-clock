#!/usr/bin/python3
import json
import re
import huffman


def truncate_if_to_long(string: str, max_length: int) -> str:
    if len(string) <= max_length:
        return string

    firstPoint = re.search(
        r'(?:(?<!A)(?<!P)(?<!No))\.', string)

    if firstPoint is not None and firstPoint.start() < len(string) - 10:
        return string[firstPoint.start() + 2:]

    textArr = string.split(" ")

    while len(' '.join(textArr)) > max_length:
        del textArr[0]

    return "..." + ' '.join(textArr)


def quote_as_string(quote: dict) -> str:

    combined_quote = []
    for key in ["textBefore", "timeText", "textAfterTime", "author", "title"]:
        combined_quote.append(quote[key])
    return ("\x02".join(combined_quote), quote["timeHour"], quote["timeMinute"])


def quote_lengths(quote: dict) -> dict:
    lengths = {}
    for key in ["textBefore", "timeText", "textAfterTime", "author", "title"]:
        lengths[key] = len(quote[key])
    return lengths


def quote_max_lengths(quotes: list) -> dict:
    max_lengths = {}
    for quote in quotes:
        lengths = quote_lengths(quote)
        for key in lengths:
            if key not in max_lengths or lengths[key] > max_lengths[key]:
                max_lengths[key] = lengths[key]
    return max_lengths


def dump_as_huffman(quotes: list) -> str:
    for quote in quotes:
        quote["textBefore"] = truncate_if_to_long(quote["textBefore"], 150)

    quote_lengths = quote_max_lengths(quotes)
    quotes = list(map(quote_as_string, quotes))
    combined_string = "".join(map(lambda x: x[0], quotes))
    huffman_tree = huffman.HuffmanTree.from_string(combined_string + "\x01")
    encoded_quotes = []

    for quote in quotes:
        encoded_quote = encode_quote(quote, huffman_tree)
        # make sure we don't have a zero at the beginning, as that would indicate EOF
        assert encoded_quote[0] != 0
        encoded_quotes += encoded_quote

    # add a null byte to indicate end of file
    encoded_quotes += [0]

    print(
        f"Without compression: {len(combined_string)}, with compression: {len(encoded_quotes)} gain: {int((1 - len(encoded_quotes) / len(combined_string)) * 100)}%")

    # dump lengths as defines
    lengths_header = ""
    for key in quote_lengths:
        lengths_header += f"#define QUOTES_{key.upper()}_LENGTH {quote_lengths[key]}\n"

    header = huffman.byte_list_to_c("quotes_huffman_tree",
                                    huffman_tree.as_byte_list()) + "\n"
    header += huffman.byte_list_to_c("quotes_data", encoded_quotes)

    return header, lengths_header


def encode_quote(quote: tuple, tree: huffman.HuffmanTree) -> list:
    quote_string, hour, minute = quote

    encoded, _ = huffman.encode(quote_string, tree)

    return encoded + [hour, minute]


f = open('AllQuotesWithoutDuplicates.json', encoding="utf-8")
d = json.load(f)

header, lengths_header = dump_as_huffman(d)

f = open("quotes.h", "w", encoding="utf-8")
f.write(header)
f.close()

f = open("quotes_lengths.h", "w", encoding="utf-8")
f.write(lengths_header)
f.close()
