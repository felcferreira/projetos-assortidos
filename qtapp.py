import os
import sys
import time
import datetime

import re
import json

import numpy as np
import face_recognition
import cv2

from openpyxl import Workbook
from PySide6.QtCore import *
from PySide6.QtGui import *
from PySide6.QtWidgets import *

import serial

# DB
class DB():
    data = {
        "Felipe": [ "db/felipe.png" ],
        "Goncalves": [ "db/goncalves.png" ], 
        "Bete": [ "db/bete.png" ], 
        "Diogo": [ "db/diogo.png" ], 
        "Eduardo": [ "db/eduardo.png" ]
        # "Antonio": [ "db/antonio.png" ]
    }
    
    encodings = []
    names     = []
    
    minutes = 0
    
    table = []
    
    def __init__(self):
        for k, v in self.data.items():
            print(v)
            image = face_recognition.load_image_file(v[0])
            v.append(face_recognition.face_encodings(image, num_jitters=5, model="cnn")[0])
            self.names.append(k)
            self.encodings.append(v[1])
            self.table.append([ k, "", "", "", "", "", "" ])
        
    def set_state(self, index, s):
        if index == -1: return
        self.table[index][self.minutes // 5 + 1] = s
    
    def add_minute(self):
        self.update()
        self.minutes = self.minutes + 1
        
    def update(self):
        if self.minutes >= 30:
            self.minutes = 0

enable = True

# Thread de atualização da câmera
class CameraThread(QThread):
    update_frame = Signal(QImage)
    db = DB()
    name = ""
    name_index = 0
    
    info = ""
    timeout = 0

    def __init__(self, parent=None):
        QThread.__init__(self, parent)
        self.status  = True
        self.capture = True
        # self.ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)

    def run(self):
        self.capture = cv2.VideoCapture(0)
        while True:
            if self.timeout >= 100:
                self.timeout = 0
            elif self.timeout >= 1:
                ++self.timeout
            
            if not self.status:
                continue
            
            ret, frame = self.capture.read()
            if not ret:
                continue
            
            # Reduz o tamanho da imagem e converte a mesma para formato RGB888
            small_frame = np.ascontiguousarray(cv2.resize(frame, (0, 0), fx=0.25, fy=0.25)[:, :, ::-1])
            
            # Escreve as informações na imagem
            face_locations, face_names = self.read_faces(small_frame)
            if face_names:
                self.name = face_names[0]
                if self.name == "?":
                    self.name_index = -1
                else:
                    self.name_index = self.db.names.index(self.name)
            
            self.plot_faces(frame, face_locations, face_names)

            # Lendo a imagem em formato RGB para poder mostrar
            color_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

            # Criando QImage
            h, w, ch = color_frame.shape
            img = QImage(color_frame.data, w, h, ch * w, QImage.Format.Format_RGB888)
            scaled_img = img.scaled(640, 480, Qt.AspectRatioMode.KeepAspectRatio)

            # Emite o sinal
            self.update_frame.emit(scaled_img)
    
    def read_faces(self, frame):
        face_locations = face_recognition.face_locations(frame)
        face_encodings = face_recognition.face_encodings(frame, face_locations, model="cnn")

        face_names = []
        for face_encoding in face_encodings:
            # Junta com as faces conhecidas e usa a que têm menor distância
            matches = face_recognition.compare_faces(self.db.encodings, face_encoding, tolerance=0.4)
            name = "?"

            face_distances = face_recognition.face_distance(self.db.encodings, face_encoding)
            best_match_index = np.argmin(face_distances)
            if matches[best_match_index]:
                name = self.db.names[best_match_index]

            face_names.append(name)
        
        if face_names and face_names[0] != "?":
            self.ser.write(b'R')
        
        return (face_locations, face_names)
    
    def plot_faces(self, frame, face_locations, face_names):
        # Mostra os resultados no vídeo
        for (top, right, bottom, left), name in zip(face_locations, face_names):
            # Aumenta as posições de novo
            top *= 4
            right *= 4
            bottom *= 4
            left *= 4

            cv2.rectangle(frame, (left, top), (right, bottom), (0, 0, 255), 2)

            cv2.rectangle(frame, (left, bottom - 35), (right, bottom), (0, 0, 255), cv2.FILLED)
            cv2.putText(frame, name, (left + 6, bottom - 6), cv2.FONT_HERSHEY_DUPLEX, 0.8, (255, 255, 255), 1)
        
        cv2.putText(frame, self.info, (1, 20), cv2.FONT_HERSHEY_DUPLEX, 0.6, (255, 255, 255), 1)

class Window(QMainWindow):
    def __init__(self):
        super().__init__()
        
        # Title and dimensions
        self.setWindowTitle("Detecção facial")
        self.setFixedSize(1380, 720)

        # Texto
        self.text = QLabel(self)
        self.text.setText("")

        # Cria a label para colocar a deteção do rosto
        self.label = QLabel(self)
        self.label.setFixedSize(640, 480)

        # Thread para atualizar a imagem
        self.thread = CameraThread(self)
        self.thread.finished.connect(self.close)
        self.thread.update_frame.connect(self.set_image)
        self.thread.start()
        
        # Tabela de presença
        self.table = QTableWidget()
        self.table.setRowCount(len(self.thread.db.names))
        self.table.setColumnCount(7)
        self.table.setHorizontalHeaderLabels(["Aluno", "1º", "2º", "3º", "4º", "5º", "6º"])
        self.table.setSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Expanding)
        self.update_table()

        # Buttons layout
        buttons_layout = QHBoxLayout()
        
        self.buttons = [ QPushButton("Parar"), QPushButton("Presença"), QPushButton("Ausência"), QPushButton("+Minuto") ]
        buttons_slot = [ self.change, self.add_state, self.remove_state, self.add_minute ]
        
        for (button, slot) in zip(self.buttons, buttons_slot):
            button.clicked.connect(slot)
            button.setSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Expanding)
            buttons_layout.addWidget(button)
        
        # Table layout
        table_layout = QVBoxLayout()
        table_layout.addWidget(self.table)
        table_layout.addWidget(self.text)
        
        # View layout
        view_layout = QHBoxLayout()
        view_layout.addWidget(self.label)
        view_layout.addLayout(table_layout)

        # Main layout
        main_layout = QVBoxLayout()
        main_layout.addLayout(view_layout)
        main_layout.addLayout(buttons_layout)

        # Central widget
        widget = QWidget(self)
        widget.setLayout(main_layout)
        self.setCentralWidget(widget)
    
    def update_table(self):
        self.text.setText(f"Minutos {self.thread.db.minutes}")
        
        for i, row in enumerate(self.thread.db.table):
            for j, v in enumerate(row):
                self.table.setItem(i, j, QTableWidgetItem(v))
    
    @Slot()
    def change(self):
        self.buttons[0].setText("Iniciar" if self.thread.status else "Parar")
        self.thread.status = not self.thread.status
    
    @Slot()
    def add_state(self):
        if self.thread.name_index == -1:
            self.timeout = 1
            self.info = "Pessoa desconhecida detectada"
            return
        self.thread.db.set_state(self.thread.name_index, "P")
        self.update_table()
    
    @Slot()
    def remove_state(self):
        if self.thread.name_index == -1:
            self.timeout = 1
            self.info = "Pessoa desconhecida detectada"
            return
        self.thread.db.set_state(self.thread.name_index, "F")
        self.update_table()
    
    @Slot()
    def add_minute(self):
        self.thread.db.add_minute()
        self.update_table()

    @Slot(QImage)
    def set_image(self, image):
        self.label.setPixmap(QPixmap.fromImage(image))

if __name__ == "__main__":
    app = QApplication()
    w = Window()
    w.show()
    sys.exit(app.exec())

