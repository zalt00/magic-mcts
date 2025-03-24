
from ctypes import cdll, c_char_p

lib = cdll.LoadLibrary("./libtruc.so")

BUFFER_SIZE = 30000

buffer = c_char_p(b"a" * (BUFFER_SIZE + 1))


import sys
from PyQt5 import uic
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                             QPushButton, QListWidget, QListWidgetItem, QDialog,
                             QFileDialog, QMessageBox, QTreeWidget, QTreeWidgetItem,
                             QTextEdit, QDockWidget, QMenu, QAction, QPlainTextEdit)
from PyQt5.QtCore import pyqtSlot, Qt, QPoint
from PyQt5.QtGui import QTextCursor, QFont
import os
import sys
import re
import json

def get_library(owner):
    lib.print_library(owner, buffer, BUFFER_SIZE)
    return buffer.value



dir_path = '/'.join(__file__.split('/')[:-1])



class Window(QMainWindow):
    def __init__(self, path=''):
        super().__init__()
        uic.loadUi(os.path.join(dir_path, 'main_window.ui'), self)
        self.show()
        self.showMaximized()
        self.item_lists = [self.library1, self.library2]
        
        for owner in range(2):
            string = get_library(owner)
            print(owner, string)
            for card_str in string.split(b";;"):
                if card_str:
                    card_str = card_str.decode()
                    card_str = " ".join(card_str.split("\n")[:-1]).replace("#", ": ")
                    self.item_lists[owner].insertItem(0, card_str)

        #self.library1.insertItem(0, arr.value.decode("utf-8"))

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

