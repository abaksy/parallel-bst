import random
a, b, c = list(range(1, 101)), list(range(1, 1001)), list(range(1, 10001))
random.shuffle(a)
random.shuffle(b)
random.shuffle(c)
f = open("inp1.txt", "w")
for i in a:
    f.write("1 "+str(i)+"\n")
f.close()

f = open("inp2.txt", "w")
for i in b:
    f.write("1 "+str(i)+"\n")
f.close()

f = open("inp3.txt", "w")
for i in c:
    f.write("1 "+str(i)+"\n")
f.close()