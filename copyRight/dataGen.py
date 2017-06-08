dataFile = open('res/data.bin', mode='w')

data = [chr(c) for c in range(ord('a'), ord('z') + 1) for i in range(1024)]
data = "".join(data)  # 'aaaa....zzzz'

dataFile.write(data)

dataFile.close()
