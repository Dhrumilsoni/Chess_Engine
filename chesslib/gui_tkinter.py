import board
import pieces
import os
from multiprocessing import Process, Queue
import Tkinter as tk
from PIL import Image, ImageTk
import time
turn = 0
flag = 0
class BoardGuiTk(tk.Frame):
    pieces = {}
    selected = None
    selected_piece = None
    hilighted = None
    icons = {}
    color1 = "white"
    color2 = "grey"

    rows = 8
    columns = 8
    @property
    def canvas_size(self):
        return (self.columns * self.square_size,
                self.rows * self.square_size)

    def __init__(self, parent, chessboard, square_size=64):
        # print "Init"
        self.chessboard = chessboard
        self.square_size = square_size
        self.parent = parent

        canvas_width = self.columns * square_size
        canvas_height = self.rows * square_size

        tk.Frame.__init__(self, parent)

        self.canvas = tk.Canvas(self, width=canvas_width, height=canvas_height, background="grey")
        self.canvas.pack(side="top", fill="both", anchor="c", expand=True)

        self.canvas.bind("<Configure>", self.refresh)
        self.canvas.bind("<Button-1>", self.click)
        # self.canvas.bind("<Button-1>", callback)

        self.statusbar = tk.Frame(self, height=64)
        self.button_quit = tk.Button(self.statusbar, text="New", fg="black", command=self.reset)
        self.button_quit.pack(side=tk.LEFT, in_=self.statusbar)

        self.button_save = tk.Button(self.statusbar, text="Save", fg="black", command=self.chessboard.save_to_file)
        self.button_save.pack(side=tk.LEFT, in_=self.statusbar)

        self.button_move = tk.Button(self.statusbar, text="Engine Move", fg="black", command=self.move_engine)
        self.button_move.pack(side=tk.LEFT, in_=self.statusbar)

        self.label_status = tk.Label(self.statusbar, text="   White's turn  ", fg="black")
        self.label_status.pack(side=tk.LEFT, expand=0, in_=self.statusbar)

        self.button_quit = tk.Button(self.statusbar, text="Quit", fg="black", command=self.parent.destroy)
        self.button_quit.pack(side=tk.RIGHT, in_=self.statusbar)
        self.statusbar.pack(expand=False, fill="x", side='bottom')


    def click(self, event):
        # self.refresh()
        # print "Click"
        # Figure out which square we've clicked
        global flag
        global turn
        col_size = row_size = event.widget.master.square_size

        current_column = event.x / col_size
        current_row = 7 - (event.y / row_size)

        position = self.chessboard.letter_notation((current_row, current_column))
        # print position
        # print self.selected_piece
        # piece = self.chessboard[position]

        if self.selected_piece:
            self.move(self.selected_piece[1], position)
            # print "moved"
            turn = 1
            self.selected_piece = None
            self.hilighted = None
            self.pieces = {}
            self.refresh()
            self.draw_pieces()
            flag=1
            # Process(target=self.engine_move).start()
            # self.engine_move()

        if position is not None:
            self.hilight(position)
        self.refresh()

    def engine_move(self):
        # time.sleep(2)
        var = open("C:\\Users\\dhrum\\Downloads\\Simple-Python-Chess-master-20190308T062157Z-001\\Simple-Python-Chess-master\\curr_board.in","w")
        print self.chessboard.export()
        var.write(self.chessboard.export())
        var.close()
        # os.system("g++ C:\\Users\\dhrum\\Downloads\\Simple-Python-Chess-master-20190308T062157Z-001\\Simple-Python-Chess-master\\bing.cpp")
        # time.sleep(5)
        print(os.system("C:\\Users\\dhrum\\Downloads\\Simple-Python-Chess-master-20190308T062157Z-001\\Simple-Python-Chess-master\\a.exe"))
        # time.sleep(5)
        var = open("C:\\Users\\dhrum\\Downloads\\Simple-Python-Chess-master-20190308T062157Z-001\\Simple-Python-Chess-master\\inp.in","r")
        str1 = ""
        str2 = ""
        for xx in var:
            str1 = xx[:2]
            str2 = xx[2:]
        var.close()
        self.chessboard.move(str1, str2)
        self.selected_piece = None
        self.hilighted = None
        self.pieces = {}
        self.refresh()
        self.draw_pieces()

    def move(self, p1, p2):
        # print "move"
        global turn
        piece = self.chessboard[p1]
        dest_piece = self.chessboard[p2]
        if dest_piece is None or dest_piece.color != piece.color:
            try:
                self.chessboard.move(p1, p2)
            except board.ChessError as error:
                self.label_status["text"] = error.__class__.__name__
            else:
                self.label_status["text"] = \
                    " " + piece.color.capitalize() +": "+ p1 + p2
        # self.refresh()
        # self.draw_pieces()
        # self.chessboard.make_move("black")
        # self.selected_piece = None
        # self.hilighted = None
        # self.pieces = {}
        # self.refresh()
        # self.draw_pieces()

    def hilight(self, pos):
        # print "hilight"
        piece = self.chessboard[pos]
        # print piece
        if piece is not None and (piece.color == self.chessboard.player_turn):
            self.selected_piece = (self.chessboard[pos], pos)
            self.hilighted = map(self.chessboard.number_notation, (self.chessboard[pos].possible_moves(pos)))

    def addpiece(self, name, image, row=0, column=0):
        # print "addpiece"
        '''Add a piece to the playing board'''
        self.canvas.create_image(0, 0, image=image, tags=(name, "piece"), anchor="c")
        self.placepiece(name, row, column)

    def placepiece(self, name, row, column):
        # print "placepiece"
        '''Place a piece at the given row/column'''
        # print name
        # print row
        # print column
        # print "--"
        self.pieces[name] = (row, column)
        x0 = (column * self.square_size) + int(self.square_size/2)
        y0 = ((7-row) * self.square_size) + int(self.square_size/2)
        self.canvas.coords(name, x0, y0)

    def refresh(self, event={}):
        global flag
        '''Redraw the board'''
        # print "refresh"

        #  #     self.engine_move()
        if event:
            xsize = int((event.width-1) / self.columns)
            ysize = int((event.height-1) / self.rows)
            self.square_size = min(xsize, ysize)

        self.canvas.delete("square")
        color = self.color2
        for row in range(self.rows):
            color = self.color1 if color == self.color2 else self.color2
            for col in range(self.columns):
                x1 = (col * self.square_size)
                y1 = ((7-row) * self.square_size)
                x2 = x1 + self.square_size
                y2 = y1 + self.square_size
                if (self.selected is not None) and (row, col) == self.selected:
                    self.canvas.create_rectangle(x1, y1, x2, y2, outline="black", fill="orange", tags="square")
                elif (self.hilighted is not None and (row, col) in self.hilighted):
                    self.canvas.create_rectangle(x1, y1, x2, y2, outline="black", fill="spring green", tags="square")
                else:
                    self.canvas.create_rectangle(x1, y1, x2, y2, outline="black", fill=color, tags="square")
                color = self.color1 if color == self.color2 else self.color2
        for name in self.pieces:
            self.placepiece(name, self.pieces[name][0], self.pieces[name][1])
        self.canvas.tag_raise("piece")
        self.canvas.tag_lower("square")

    def draw_pieces(self):
        global flag
        # print "draw pieces"
        self.canvas.delete("piece")
        for coord, piece in self.chessboard.iteritems():
            x,y = self.chessboard.number_notation(coord)
            if piece is not None:
                filename = "img/%s%s.png" % (piece.color, piece.abbriviation.lower())
                piecename = "%s%s%s" % (piece.abbriviation, x, y)

                if(filename not in self.icons):
                    self.icons[filename] = ImageTk.PhotoImage(file=filename, width=32, height=32)

                self.addpiece(piecename, self.icons[filename], x, y)
                self.placepiece(piecename, x, y)

    def move_engine(self):
        # print "in move_engine"
        global turn
        if turn is 1:
            self.chessboard.make_move("black")
            self.pieces = {}
            self.refresh()
            self.draw_pieces()
            turn = 0

    def reset(self):
        # print "reset"
        self.chessboard.load(board.FEN_STARTING)
        self.refresh()
        self.draw_pieces()
        self.refresh()

def display(chessboard):
    # print "display"
    root = tk.Tk()
    root.title("Chess Engine")

    gui = BoardGuiTk(root, chessboard)
    gui.pack(side="top", fill="both", expand="true", padx=4, pady=4)
    gui.draw_pieces()
    # print "drawn"
    # time.sleep(1)
    # print "slept"

    # gui.move_engine()

    # print "called"
    # gui.pack(side="top", fill="both", expand="true", padx=4, pady=4)
    # gui.draw_pieces()

    root.mainloop()

if __name__ == "__main__":
    display()
