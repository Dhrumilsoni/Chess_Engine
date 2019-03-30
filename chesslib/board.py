from itertools import groupby
from copy import deepcopy

import os
import pieces
import re
import copy
import time

class ChessError(Exception): pass
class InvalidCoord(ChessError): pass
class InvalidColor(ChessError): pass
class InvalidMove(ChessError): pass
class Check(ChessError): pass
class CheckMate(ChessError): pass
class Draw(ChessError): pass
class NotYourTurn(ChessError): pass

FEN_STARTING = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
RANK_REGEX = re.compile(r"^[A-Z][1-8]$")

class Board(dict):
    '''
       Board

       A simple chessboard class

       TODO:

        * PGN export
        * En passant
        * Castling
        * Promoting pawns
        * Fifty-move rule
    '''

    axis_y = ('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H')
    axis_x = tuple(range(1, 9)) # (1,2,3,...8)

    captured_pieces = { 'white': [], 'black': [] }
    player_turn = None
    castling = '-'
    en_passant = '-'
    halfmove_clock = 0
    fullmove_number = 1
    history = []

    def __init__(self, fen=None):
        if fen is None:
            self.load(FEN_STARTING)
        else:
            self.load(fen)

    def __getitem__(self, coord):
        if isinstance(coord, str):
            coord = coord.upper()
            if not re.match(RANK_REGEX, coord.upper()): raise KeyError
        elif isinstance(coord, tuple):
            coord = self.letter_notation(coord)
        try:
            return super(Board, self).__getitem__(coord)
        except KeyError:
            return None

    def save_to_file(self): pass

    def is_in_check_after_move(self, p1, p2):
        # Create a temporary board
        tmp = deepcopy(self)
        tmp._do_move(p1,p2)
        return tmp.is_in_check(self[p1].color)



    def get_enemy(self, color):
        if color == "white": return "black"
        else: return "white"

    def _do_move(self, p1, p2):
        '''
            Move a piece without validation
        '''
        piece = self[p1]
        dest  = self[p2]
        del self[p1]
        self[p2] = piece
        # print self
        # print self.keys()

    def _finish_move(self, piece, dest, p1, p2):
        '''
            Set next player turn, count moves, log moves, etc.
        '''
        enemy = self.get_enemy(piece.color)
        if piece.color == 'black':
            self.fullmove_number += 1
        self.halfmove_clock +=1
        self.player_turn = enemy
        abbr = piece.abbriviation
        if abbr == 'P':
            # Pawn has no letter
            abbr = ''
            # Pawn resets halfmove_clock
            self.halfmove_clock = 0
        if dest is None:
            # No capturing
            movetext = abbr +  p2.lower()
        else:
            # Capturing
            movetext = abbr + 'x' + p2.lower()
            # Capturing resets halfmove_clock
            self.halfmove_clock = 0

        self.history.append(movetext)

    def all_possible_moves(self, color):
        '''
            Return a list of `color`'s possible moves.
            Does not check for check.
        '''
        if(color not in ("black", "white")): raise InvalidColor

        result = []
        for coord in self.keys():
            if (self[coord] is not None) and self[coord].color == color:
                moves = self[coord].possible_moves(coord)
                if moves: result += moves
        return result

    def occupied(self, color):
        '''
            Return a list of coordinates occupied by `color`
        '''
        result = []
        if(color not in ("black", "white")): raise InvalidColor

        for coord in self:
            if self[coord].color == color:
                result.append(coord)
        return result

    def is_king(self, piece):
        return isinstance(piece, pieces.King)


    def get_king_position(self, color):
        for pos in self.keys():
            if self.is_king(self[pos]) and self[pos].color == color:
                return pos

    def get_king(self, color):
        if(color not in ("black", "white")): raise InvalidColor
        return self[self.get_king_position(color)]

    def is_in_check(self, color):
        if(color not in ("black", "white")): raise InvalidColor
        king = self.get_king(color)
        enemy = self.get_enemy(color)
        return king in map(self.__getitem__, self.all_possible_moves(enemy))

    def letter_notation(self, coord):
        # print coord
        if not self.is_in_bounds(coord): return
        try:
            # print self.axis_y[coord[1]] + str(self.axis_x[coord[0]])
            return self.axis_y[coord[1]] + str(self.axis_x[coord[0]])
        except IndexError:
            raise InvalidCoord

    def number_notation(self, coord):
        return int(coord[1])-1, self.axis_y.index(coord[0])

    def is_in_bounds(self, coord):
        if coord[1] < 0 or coord[1] > 7 or\
           coord[0] < 0 or coord[0] > 7:
            return False
        else: return True

    def load(self, fen):
        '''
            Import state from FEN notation
        '''
        self.clear()
        # Split data
        fen = fen.split(' ')
        # Expand blanks
        def expand(match): return ' ' * int(match.group(0))

        fen[0] = re.compile(r'\d').sub(expand, fen[0])

        for x, row in enumerate(fen[0].split('/')):
            for y, letter in enumerate(row):
                if letter == ' ': continue
                coord = self.letter_notation((7-x, y))
                self[coord] = pieces.piece(letter)
                self[coord].place(self)

        if fen[1] == 'w': self.player_turn = 'white'
        else: self.player_turn = 'black'

        self.castling = fen[2]
        self.en_passant = fen[3]
        self.halfmove_clock = int(fen[4])
        self.fullmove_number = int(fen[5])

    def export(self):
        '''
            Export state to FEN notation
        '''
        def join(k, g):
            if k == ' ': return str(len(g))
            else: return "".join(g)

        def replace_spaces(row):
            # replace spaces with their count
            result = [join(k, list(g)) for k, g in groupby(row)]
            return "".join(result)


        result = ''
        for number in self.axis_x[::-1]:
            for letter in self.axis_y:
                piece = self[letter+str(number)]
                if piece is not None:
                    result += piece.abbriviation
                else: result += ' '
            result += '/'

        result = result[:-1] # remove trailing "/"
        result = replace_spaces(result)
        result += " " + (" ".join([self.player_turn[0],
                         self.castling,
                         self.en_passant,
                         str(self.halfmove_clock),
                         str(self.fullmove_number)]))
        return result

    def legal_moves(self, color):
        '''
            Return a list of `color`'s legal moves.
        '''
        result = []
        if self.is_in_check(color):
            for coord in self.keys():
                if (self[coord] is not None) and self[coord].color == color:
                    moves = self[coord].possible_moves(coord)
                    for move in moves:
                        if not self.is_in_check_after_move(coord, move):
                            result.append([coord, move])
            return result
        for coord in self.keys():
            if (self[coord] is not None) and self[coord].color == color:
                # print coord
                moves = self[coord].possible_moves(coord)
                if moves:
                    for move in moves:
                        result.append([coord, move])
        return result

    def check_legal_moves(self, color):
        print self.legal_moves(color)

    def evaluate_state(self):
        white_value = 0
        black_value = 0
        for coord in self.keys():
            if(self[coord] is not None and self[coord].color == "white"):
                # print self[coord]
                # print coord
                # print self[coord].color
                # print self[coord].abbriviation

                if self[coord].abbriviation=='P':
                    white_value += 1
                elif self[coord].abbriviation=='N':
                    white_value += 3
                elif self[coord].abbriviation=='R':
                    white_value += 4
                elif self[coord].abbriviation=='B':
                    white_value += 5
                elif self[coord].abbriviation=='Q':
                    white_value += 9
                elif self[coord].abbriviation=='K':
                    white_value += 100
            elif (self[coord] is not None and self[coord].color == "black"):
                if self[coord].abbriviation=='p':
                    black_value += 1
                elif self[coord].abbriviation=='n':
                    black_value += 3
                elif self[coord].abbriviation=='r':
                    black_value += 4
                elif self[coord].abbriviation=='b':
                    black_value += 5
                elif self[coord].abbriviation=='q':
                    black_value += 9
                elif self[coord].abbriviation=='k':
                    black_value += 100
        # print black_value
        # print white_value
        # time.sleep(50)
        return black_value-white_value

    def dfs(self, local_board, maximizer, depth, to_move):
        if depth is 2:
            return local_board.evaluate_state()
        local_local_board = Board()
        local_local_board.load(local_board.export())
        local_local_board.move1(to_move[0], to_move[1])
        if maximizer is 1:
            min_value = 0
            for ele in local_local_board.legal_moves("white"):
                # print ele
                min_value = max(min_value, local_local_board.dfs(local_local_board, 0, depth+1, ele))
            return min_value
        else:
            max_value = -1000
            for ele in local_local_board.legal_moves("black"):
                max_value = min(max_value, local_local_board.dfs(local_local_board, 1, depth+1, ele))
            return max_value

    def make_move(self, color):
        # for coord in self.keys():
        #     print coord
        # print self.keys()
        # print self.export()
        var = open("C:\\Users\\dhrum\\Downloads\\Simple-Python-Chess-master-20190308T062157Z-001\\Simple-Python-Chess-master\\curr_board.in", "w")

        print self.export()
        var.write(self.export())
        time.sleep(5)
        var.close()
        os.system("g++ C:\\Users\\dhrum\\Downloads\\Simple-Python-Chess-master-20190308T062157Z-001\\Simple-Python-Chess-master\\bing.cpp")
        time.sleep(5)
        os.system("C:\\Users\\dhrum\\Downloads\\Simple-Python-Chess-master-20190308T062157Z-001\\Simple-Python-Chess-master\\a.exe")
        time.sleep(5)
        var = open("C:\\Users\\dhrum\\Downloads\\Simple-Python-Chess-master-20190308T062157Z-001\\Simple-Python-Chess-master\\inp.in","r")
        str1 = ""
        str2 = ""
        for xx in var:
            str1 = xx[:2]
            str2 = xx[2:]
        var.close()



        # local_black=[]
        # for ele in black_moves:
        #     local_black.append([ele[0], ele[1], type(self[ele[0]]).__name__])
        # local_white=[]
        # for ele in white_moves:
        #     local_white.append([ele[0], ele[1], type(ele[0]).__name__])
        # dfs(local_bla
        # ck, local_white, 1)
        # print black_moves
        # print white_moves
        # print "hi"

        # local_board = Board()
        # file = open("curr_board.in", "w")
        # file.write(self.export())
        # file.close()
        #
        #
        # local_board.load(self.export())
        # black_moves = local_board.legal_moves("black")
        # max_value = 0
        # final_move = black_moves[0]
        # for ele in black_moves:
        #
        #     temp_val = local_board.dfs(local_board, 1, 0, ele)
        #     if max_value <= temp_val:
        #         max_value = temp_val
        #         final_move = ele
        # # print max_value
        print str1
        print str2
        self.move(str1, str2)


        # print local_board.legal_moves("black")
        # print local_board.legal_moves("white")
        # local_board.keys()
        # local_board.move((local_board.legal_moves("white"))[0][0], (local_board.legal_moves("white"))[0][1])
        # print local_board

        # if color == "black":
        #     self.move(black_moves[0][0], black_moves[0][1])
        # else :
        #     self.move(white_moves[0][0], white_moves[0][1])
        # curr_state = [[]]
        # for i in range(1, 9):
        #     for j in range(1, 9):
        #         if self[(char(i)+'A')+j] is not None:
        #             curr_state[i][j].append([self[coord].color, type(self[coord]).__name__])
        # print curr_state

        # for coord in self.keys():
        #     print coord + " " + self[coord].color + " " + type(self[coord]).__name__
        # self.move(moves[0][0], moves[0][1])

    def move(self, p1, p2):
        p1, p2 = p1.upper(), p2.upper()
        piece = self[p1]
        dest  = self[p2]
        if self.player_turn != piece.color:
            raise NotYourTurn("Not " + piece.color + "'s turn!")

        enemy = self.get_enemy(piece.color)
        possible_moves = piece.possible_moves(p1)
        # 0. Check if p2 is in the possible moves
        if p2 not in possible_moves:
            raise InvalidMove

        # If enemy has any moves look for check
        if self.all_possible_moves(enemy):
            if self.is_in_check_after_move(p1, p2):
                raise Check

        if not possible_moves and self.is_in_check(piece.color):
            raise CheckMate
        elif not possible_moves:
            raise Draw
        else:
            self._do_move(p1, p2)
            self._finish_move(piece, dest, p1, p2)
        # print piece.color
        # if piece.color == "white":
        #     color = "black"
        # else:
        #     color = "white"

        # if color == "black":
        #     # print "go black"
        #     self.make_move(color)

    def move1(self, p1, p2):
        p1, p2 = p1.upper(), p2.upper()
        piece = self[p1]
        dest  = self[p2]
        if self.player_turn != piece.color:
            raise NotYourTurn("Not " + piece.color + "'s turn!")

        # 0. Check if p2 is in the possible moves

        # If enemy has any moves look for check

        self._do_move(p1, p2)
        self._finish_move(piece, dest, p1, p2)
        # print piece.color
        if piece.color == "white":
            color = "black"
        else:
            color = "white"

        # if color == "black":
        #     # print "go black"
        #     self.make_move(color)
