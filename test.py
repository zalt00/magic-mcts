
from ctypes import cdll, c_char_p

lib = cdll.LoadLibrary("./libtruc.so")

BUFFER_SIZE = 30000

buffer = c_char_p(b"a" * (BUFFER_SIZE + 1))


import sys
from PyQt5 import uic
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                             QPushButton, QListWidget, QListWidgetItem, QDialog,
                             QFileDialog, QMessageBox, QTreeWidget, QTreeWidgetItem,
                             QTextEdit, QDockWidget, QMenu, QAction, QPlainTextEdit, QStyleOption, QStyle)
from PyQt5.QtCore import pyqtSlot, Qt, QPoint
from PyQt5.QtGui import QTextCursor, QFont, QPainter, QPalette, QBrush, QColor
from PyQt5 import QtGui
import os
import sys
import re
import json

import random

from dataclasses import dataclass

def get_library(owner):
    lib.print_library(owner, buffer, BUFFER_SIZE)
    return buffer.value

def get_battlefield(owner):
    lib.print_battlefield(owner, buffer, BUFFER_SIZE)
    return buffer.value

def get_hand(owner):
    lib.print_hand(owner, buffer, BUFFER_SIZE)
    return buffer.value

def action_pass():
    print("action requested afterwards: ", lib.action_pass())

@dataclass
class CardTemplate:
    quantity: int
    id: int

    name: str
    manacost: str
    type_stype: str
    abilities: str
    stats: str
    color: str

    def get_rgb(self):
        if len(self.color) >= 2:
            return (0xe9, 0xD9, 0x89, 0xff) #E9D989 
        elif len(self.color) == 0:
            return (0xcc, 0xd7, 0xdd, 0xff) #CCD7DD
        elif self.color == 'W':
            return (0xf6, 0xf7, 0xf2, 0xff) #F6F7F2
        elif self.color == 'U':
            return (0xb5, 0xd0, 0xed, 0xff) #B5D0ED
        elif self.color == 'B':
            return (0xaf, 0xa9, 0xa9, 0xff) #AFA9A9
        elif self.color == 'R':
            return (0xef, 0xc9, 0xb4, 0xff) #EFC9B4
        elif self.color == 'G':
            return (0xc5, 0xd4, 0xcd, 0xff) #C5D4CD
        
def get_cardtempl_data(s, quant=False, wid=False):
    if quant:
        (q, s) = s.split("#")
    else:
        q = 1

    if wid:
        (s, id_) = s.split("$")
    else:
        id_ = -1
    ct = CardTemplate(q, id_, *s.split("|"))
    while ct.abilities.endswith("\n"):
        ct.abilities = ct.abilities[:-1]
    return ct

dir_path = '/'.join(__file__.split('/')[:-1])

class Card(QWidget):
    def __init__(self, parent, data: CardTemplate, place="battlefield"):
        super().__init__(parent)
        uic.loadUi(os.path.join(dir_path, 'card.ui'), self)
        self.show()

        self.nameCostTypeLabel.setText(f"{data.name}  {data.manacost}\n{data.type_stype}")
        self.abilitiesLabel.setText(f"{data.abilities}")
        self.statsLabel.setText(f"{data.stats}")
        self.data = data
        self.being_dragged = False
        self.dragged_local_pos = [0, 0]

        palette = QPalette()
        brush = QBrush(QColor(255, 255, 255, 255))
        brush.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Base, brush)
        brush1 = QBrush(QColor(*data.get_rgb()))
        brush1.setStyle(Qt.SolidPattern)
        palette.setBrush(QPalette.Active, QPalette.Window, brush1)
        palette.setBrush(QPalette.Inactive, QPalette.Base, brush)
        palette.setBrush(QPalette.Inactive, QPalette.Window, brush1)
        palette.setBrush(QPalette.Disabled, QPalette.Base, brush1)
        palette.setBrush(QPalette.Disabled, QPalette.Window, brush1)
        self.frame.setPalette(palette)
        self.scrollAreaWidgetContents.setPalette(palette)

        self.place = place


    def mousePressEvent(self, a0: QtGui.QMouseEvent) -> None:
        super().mousePressEvent(a0)
        self.being_dragged = True
        self.dragged_local_pos = a0.localPos()

        self.frame.setLineWidth(2)

    def mouseReleaseEvent(self, a0):
        self.frame.setLineWidth(1)
        if abs(self.pos().y() - 9) > 60 and self.place == "hand":
            print(222)

        return super().mouseReleaseEvent(a0)

    def mouseMoveEvent(self, a0: QtGui.QMouseEvent) -> None:
        super().mouseMoveEvent(a0)
        if self.being_dragged:
            pos2 = a0.localPos()
            v = pos2 - self.dragged_local_pos
            self.move((self.pos() + v).toPoint())

    def mouseDoubleClickEvent(self, a0):
        print(111)
        return super().mouseDoubleClickEvent(a0)

class Window(QMainWindow):
    def __init__(self, path=''):
        super().__init__()
        uic.loadUi(os.path.join(dir_path, 'main_window.ui'), self)
        lib.init_board()

        
        self.show()
        self.showMaximized()
        self.item_lists = [self.library1, self.library2]
        
        for owner in range(2):
            string = get_library(owner)
            print(owner, string)
            for card_str in string.split(b";;"):
                if card_str:
                    card_str = card_str.decode()

                    card_templ = get_cardtempl_data(card_str, True)
                    label = f"{card_templ.quantity} {card_templ.name}  {card_templ.manacost}"


                    self.item_lists[owner].insertItem(0, label)

        self.battlefield_cards = [[], []]
        self.battlefields = [self.battlefield1, self.battlefield2]

        self.hand_layouts = [self.hand1layout, self.hand2layout]
        self.hands = [self.hand1, self.hand2]
        self.hand_cards = [[], []]

        for owner in range(2):
            string = get_battlefield(owner)
            print(owner, string)
            geometry = self.battlefields[owner].geometry()
            i = 0

            for card_str in string.split(b";;"):
                if card_str:
                    card_str = card_str.decode()
                    card_templ = get_cardtempl_data(card_str, False, True)
                    i = (i + 140) % 1200   
                    widget = Card(self.battlefields[owner], card_templ)
                    self.battlefield_cards[owner].append(widget)
                    x = i
                    y = random.randint(0, geometry.height() - 80)
                    widget.move(x, y)
        for owner in range(2):
            string = get_hand(owner)
            print("hand: ", owner, string)

            for card_str in string.split(b";;"):
                if card_str:
                    card_str = card_str.decode()
                    card_templ = get_cardtempl_data(card_str, False, True)
                    
                    widget = Card(self.hands[owner], card_templ, "hand")
                    self.hand_cards[owner].append(widget)
                    self.hand_layouts[owner].addWidget(widget)
        #self.library1.insertItem(0, arr.value.decode("utf-8"))

        self.passButton.clicked.connect(action_pass)

def main():
    if len(sys.argv) == 2:
        path = sys.argv[1]
    else:
        path = ''
    app = QApplication([])
    window = Window(path)
    app.exec_()

if __name__ == '__main__':
    main()

