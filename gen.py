import random


def gen():
        num = 10
    	ch_list = [chr(i) for i in range(ord('a'), ord('z') + 1)] 
	length = 3
	with open("data/small-data.txt", "w") as F:
		for i in range(num):
			key = random.randint(1, 100)
			F.write(str(key))
			F.write(" " + "".join([random.choice(ch_list) for i in range(length)]) + "\n")

def main():
        gen()

main()
