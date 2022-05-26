#!/usr/bin/env python3.10
from __future__ import annotations
import sys

TOKEN_TYPES_NAMES = ['NEWLINE', 'OPERATOR', 'IO_NUMBER', 'TOKEN', 'WORD', 'ASSIGNMENT_WORD', 'NAME']
TOKEN_TYPES = dict(enumerate(TOKEN_TYPES_NAMES)) | {-1: 'UNKNOWN'}

# general: 0
# here_io: 1
STATE: int = 0
TOKENS = list()

class Token:
    def __init__(self, s: str, t: int = -1):
        self.s = s
        self.t = t
    def __str__(self):
        s = r'\n' if self.s == '\n' else self.s
        t = TOKEN_TYPES[self.t]
        return f"T: '{s}' ({t})"
    def __repr__(self):
        return str(self)

class Tokenizer(object):
    def tokenize(self, line: str) -> list[str]:
        print(line)

class GeneralTokenizer(Tokenizer):
    def tokenize(self, line: str) -> list[str]:
        print(f"general: {line = }")
        index: int = 0
        line_len: int = len(line)
        c: str = line[index]
        current_token: str = str()
        current_token_type: int = -1 # unknown
        while True:
            # Rule 1
            if index == line_len:
                # finalize current token
                TOKENS.append(Token(current_token))
                return 0
            if 






class HereDocumentTokenizer(Tokenizer):
    def tokenize(self, line: str) -> list[str]:
        print(f"here_io: {line = }")








general_tokenizer = GeneralTokenizer()
here_doc_tokenizer = HereDocumentTokenizer()

def parse_line(line: str, mode: int) -> tuple[int, list[str]]:
    if mode == 0:
        mode = general_tokenizer.tokenize(line)
    else:
        mode = here_doc_tokenizer.tokenize(line)
    return mode

def main():
    mode = 0
    while line := sys.stdin.readline():
        mode = parse_line(line, mode)



if __name__ == "__main__":
    main()
