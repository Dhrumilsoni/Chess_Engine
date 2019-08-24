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


FEN_STARTING = 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1' # Starting fen notation
RANK_REGEX = re.compile(r"^[A-Z][1-8]$")
fl = 0

class Board(dict):
    '''
       Board

       TODO:

        * En passant
        * Castling
        * Promoting pawns
        * Fifty-move rule
    '''

    # Initializing variables for orientation

    axis_y = ('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H')
    axis_x = tuple(range(1, 9)) # (1,2,3,...8)

    # Initializing variables for working

    captured_pieces = { 'white': [], 'black': [] }
    player_turn = None
    castling = '-'
    en_passant = '-'
    halfmove_clock = 0
    fullmove_number = 1
    history = []

    # Try to load a fen

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

    # Functions for checking and validating a move

    # Names of the functions are analogous to its meaning

    def is_in_check_after_move(self, p1, p2):
        # Create a temporary board
        tmp = deepcopy(self)
        tmp._do_move(p1, p2)
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
            movetext = abbr + p2.lower()
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

    # Load the fen

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

    # Converting a chess board (dictionary) into fen

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

    # Check if move is legal

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

    # Evaluate state by getting the exact score of the pieces
    # Following functions are not called as of now as c++ code for the same thing works faster

    def evaluate_state(self):
        white_value = 0
        black_value = 0
        for coord in self.keys():
            if(self[coord] is not None and self[coord].color == "white"):
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

    # This function is called when AI has to make a move

    def make_move(self, color):
        global fl
        # for coord in self.keys():
        #     print coord
        # print self.keys()
        # print self.export()


        # fl defines if it is a starting move or not. If yes, then starting procedure is followed

        if fl is 0:
            fl = 1
            str1="D7"
            str2="D5"
            self.move(str1, str2)
            return
        if fl is 1:
            fl = 2
            str1="G8"
            str2="F6"
            self.move(str1, str2)
            return

        # Export fen state to a file named curr_board.in
        var = open("C:\\Users\\dhrum\\Downloads\\Simple-Python-Chess-master-20190308T062157Z-001\\Simple-Python-Chess-master\\curr_board.in", "w")
        print self.export()
        var.write(self.export())
        var.close()

        # Calling a.exe which will run the executable file which is made from c++ code in the versions folder

        print(os.system("C:\\Users\\dhrum\\Downloads\\Simple-Python-Chess-master-20190308T062157Z-001\\Simple-Python-Chess-master\\a.exe"))
        # time.sleep(5)

        # Output of the executable is imported from inp.in

        var = open("C:\\Users\\dhrum\\Downloads\\Simple-Python-Chess-master-20190308T062157Z-001\\Simple-Python-Chess-master\\inp.in", "r")

        # Processing of output
        str1 = ""
        str2 = ""
        for xx in var:
            str1 = xx[:2]
            str2 = xx[2:]
        var.close()

        # Calling the move function which finally completes the move with validation for legality and checks.

        print str1
        print str2
        self.move(str1, str2)

    # This function finally validates and makes the move.

    def move(self, p1, p2):
        # Getting piece's values
        p1, p2 = p1.upper(), p2.upper()
        piece = self[p1]
        dest  = self[p2]

        # Checking for the turn correctly

        if self.player_turn != piece.color:
            raise NotYourTurn("Not " + piece.color + "'s turn!")

        # Checking if the move is valid or not

        enemy = self.get_enemy(piece.color)
        possible_moves = piece.possible_moves(p1)
        # 0. Check if p2 is in the possible moves
        if p2 not in possible_moves:
            raise InvalidMove

        # If enemy has any moves look for check
        if self.all_possible_moves(enemy):
            if self.is_in_check_after_move(p1, p2):
                raise Check

        # Raise checkmate or draw if such an event occurs or complete the move finally

        if not possible_moves and self.is_in_check(piece.color):
            raise CheckMate
        elif not possible_moves:
            raise Draw
        else:
            self._do_move(p1, p2)
            self._finish_move(piece, dest, p1, p2)

# Todo
# Raise stalemate error
# Queen from pawn
