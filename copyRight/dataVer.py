import os

DATA_FILE = 'res/received.bin'

dataFile = open(DATA_FILE, mode='r')

data = [chr(c) for c in range(ord('a'), ord('z') + 1) for i in range(1024)]
data = "".join(data)  # 'aaaa....zzzz'

readData = dataFile.read()

dataFile.close()

assert data == readData

os.remove(DATA_FILE)
