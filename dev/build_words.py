import urllib.request
import megadrive_resource_tools
from megadrive_resource_tools.restool import ResTool


# restool = ResTool()
# restool.use_palette('test.png')
# exit(1)


guesses_url = 'https://raw.githubusercontent.com/Kinkelin/WordleCompetition/main/data/official/official_allowed_guesses.txt'
wordles_url = 'https://raw.githubusercontent.com/Kinkelin/WordleCompetition/main/data/official/shuffled_real_wordles.txt'


def read_lines_from_url(url):
    lines = []
    for line in urllib.request.urlopen(url):
        lines.append(line.decode('utf-8').strip().upper())

    return lines


words = read_lines_from_url(guesses_url)[1:]

wordle_words = []

for line in read_lines_from_url(wordles_url)[1:]:

    wordle_words.append(line)

    if line not in words:
        words.append(line)

list.sort(words)
list.sort(wordle_words)


print(words)
print(wordle_words)
print(len(wordle_words))

offsets = [-1] * 26
for i in range(0, len(words)):
    word = words[i]

    o = ord(word[0]) - ord('A')

    if offsets[o] == -1:
        offsets[o] = i

print(offsets)

with open('words.bin', 'w+') as words_data:
    for word in words:
        words_data.write(word)

with open("../src/offset.s", 'w+') as offsets_file:
    offsets_file.writelines([
        ".align 2\n",
        ".globl WORD_OFFSETS\n",
        "WORD_OFFSETS:\n"
    ])

    for o in offsets:
        offsets_file.write("dc.w {}\n".format(o))

    offsets_file.writelines([
        ".globl WORDLE_WORDS\n"
        "WORDLE_WORDS:\n"
    ])

    for word in wordle_words:
        offsets_file.write("dc.w {}\n".format(words.index(word)))
