import sqlite3
from http.server import BaseHTTPRequestHandler
from urllib import parse
import threading
from threading import Semaphore
import time

sem = Semaphore(1)
total = 0

def reserva(sol):
    time.sleep(1)
    sem.acquire()
    res = dbconnection("SELECT id FROM Tickets WHERE state = 0;")
    if res >= 0:
        dbconnection(
            "UPDATE Tickets set state = 1 WHERE id = " + str(res) + ";")
        response = "Solicitud: " + str(sol) + " Ticket nro " + str(res) + " Reservado"
    else:
        response = "Solicitud: " + str(sol) + " Error de reserva"
    time.sleep(1)
    sem.release()
    print(response)
    return response


def compra(id, sol):
    time.sleep(1)
    sem.acquire()
    res = dbconnection("SELECT state FROM Tickets WHERE id = " + str(id) + ";")
    if res == 1:
        dbconnection(
            "UPDATE Tickets set state = 2 WHERE id = " + str(id) + ";")
        response = "Solicitud: " + str(sol) + " Ticket nro " + str(id) + " Comprado"
    else:
        response = "Solicitud: " + str(sol) + " Error de compra"
    time.sleep(1)
    sem.release()
    print(response)
    return response


def initdb():
    sqliteConnection = sqlite3.connect('Tickets.db')
    cursor = sqliteConnection.cursor()
    sqliteConnection.commit()
    cursor.execute('DROP TABLE IF EXISTS Tickets;')
    cursor.execute('''  CREATE TABLE IF NOT EXISTS Tickets(
                        id INTEGER PRIMARY KEY,
                        state INTEGER);''')
    sqliteConnection.commit()
    cursor.execute('SELECT COUNT(*) FROM Tickets;')
    sqliteConnection.commit()
    record = cursor.fetchone()
    if record[0] == 0:
        for x in range(15):
            cursor.execute('INSERT INTO Tickets(state) VALUES(0);')
    sqliteConnection.commit()
    cursor.close()


def dbconnection(sql):
    sqliteConnection = sqlite3.connect('Tickets.db')
    cursor = sqliteConnection.cursor()
    cursor.execute(sql)
    record = cursor.fetchone()
    sqliteConnection.commit()
    cursor.close()
    if record != None:
        return record[0]
    else:
        return -1


def dblist():
    sqliteConnection = sqlite3.connect('Tickets.db')
    cursor = sqliteConnection.cursor()
    sqlite_select_query = "SELECT * from Tickets"
    cursor.execute(sqlite_select_query)
    records = cursor.fetchall()
    for row in records:
        print("(", row[0], ",", row[1], ")")
    cursor.close()


class GetHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        global total
        parsed_path = parse.urlparse(self.path)
        query = parse.parse_qs(parsed_path.query)
        res = query.get("reserva")
        com = query.get("compra")
        id = total
        total += 1
        if res:
            t = threading.Thread(target=reserva, args=[id])
            t.start()
        if com:
            t = threading.Thread(target=compra, args=[int(com[0]), id])
            t.start()
        response = "Respuesta random"
        self.send_response(200)
        self.send_header('Content-Type', 'text/plain; charset=utf-8')
        self.end_headers()
        self.wfile.write(response.encode('utf-8'))

# dblist()


if __name__ == '__main__':
    initdb()
    from http.server import HTTPServer
    server = HTTPServer(('localhost', 8080), GetHandler)
    print('Serving, <Ctrl-C> to stop')
    server.serve_forever()
