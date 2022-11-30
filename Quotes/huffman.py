class HuffmanTree:
    def __init__(self, char: str, count: int, left=None, right=None):
        if len(char) != 1:
            raise Exception("char can only have one char!")
        self.char = char
        self.count = count
        self.left: HuffmanTree | None = left
        self.rigth: HuffmanTree | None = right

    def __repr__(self) -> str:
        return f"Node('{self.char}'({ord(self.char)}), {self.count})"

    def as_byte_list(self):
        if self.char == "\x00":
            return [ord(self.char)] + self.left.as_byte_list() + self.rigth.as_byte_list()

        return [ord(self.char)]

    @staticmethod
    def __count_chars(string: str) -> dict:
        counts = {}
        for char in string:
            if char in counts:
                counts[char] += 1
            else:
                counts[char] = 1

        return counts

    @staticmethod
    def from_string(string: str):
        char_counts = HuffmanTree.__count_chars(string)

        nodes = []
        for char in char_counts:
            nodes.append(HuffmanTree(char, char_counts[char]))

        while len(nodes) > 1:
            nodes.sort(key=lambda node: node.count, reverse=True)

            right = nodes.pop()
            left = nodes.pop()

            parent_node = HuffmanTree("\x00",
                                      left.count + right.count, left, right)
            nodes.append(parent_node)

        return nodes[0]


def __calculate_lut(tree: HuffmanTree, dict={}, val=[]) -> dict:
    if tree.char != "\x00":
        dict[tree.char] = val
    else:
        dict = __calculate_lut(tree.left, dict, val + [0b1])
        dict = __calculate_lut(tree.rigth, dict, val + [0b0])

    return dict


def __group_bits_as_bytes(bits: list) -> list:
    byte_list = []
    while len(bits) > 0:
        byte = 0b0
        for _ in range(8):
            byte = (byte << 1)
            if len(bits) > 0:
                byte |= bits.pop(0)
        byte_list.append(byte)

    return byte_list


def encode(string, tree=None) -> tuple:
    if "\x00" in string or "\x01" in string:
        raise Exception("string must not conatain \\x00 or \\x01!")

    string += "\x01"
    if tree is None:
        tree = HuffmanTree.from_string(string)
    lut = __calculate_lut(tree)
    encoded = []
    for char in string:
        encoded += lut[char]

    encoded = __group_bits_as_bytes(encoded)
    return encoded, tree


def __find_char(tree: HuffmanTree, encoded: list, current_index: list) -> str:
    # the current index is a list with one element, so it acts like a pointer
    if tree.char != "\x00":
        return tree.char

    current_bit = (encoded[int(current_index[0] / 8)] >>
                   (7 - (current_index[0] % 8))) & 1
    current_index[0] += 1

    if current_bit == 1:
        tree = tree.left
    else:
        tree = tree.rigth

    return __find_char(tree, encoded, current_index)


def decode(encoded: list, tree: HuffmanTree) -> str:
    string = ""
    current_index = [0]
    while True:
        c = __find_char(tree, encoded, current_index)

        if c == "\x01":
            break

        string += c

    return string


def byte_list_to_c(name: str, bytes: list) -> str:

    # remove last comma
    headerString = "const uint8_t " + name + "[] PROGMEM = {\n  "
    counter = 0
    for byte in bytes:
        headerString += "0x{:02x}, ".format(byte)
        counter += 1
        if counter % 16 == 0:
            headerString += "\n  "

    # remove last comma
    headerString = headerString[:-2]
    headerString += "\n};"
    return headerString


if __name__ == "__main__":
    string = "Hello World!"

    encoded, tree = encode(string)
    print(byte_list_to_c("huffman_tree", tree.as_byte_list()))
    print(byte_list_to_c("encoded_string", encoded))

    decoded = decode(encoded, tree)
    assert decoded == string

    print(
        f"Without compression: {len(string)}, with compression: {len(encoded)}, gain: {int((1 - len(encoded) / len(string)) * 100)}%")
