import random


def gen():
        num = 10000000
    	ch_list = [chr(i) for i in range(ord('a'), ord('z') + 1)] 
	length = 1
        k = random.sample(range(700000,99999999),10000000)
	with open("data/small-data.txt", "w") as F:
		for i in range(num):
			#key = random.randint(3000001, 99999999)
                        key = k[i]
			F.write(str(key))
			F.write(" " + "".join([random.choice(ch_list) for i in range(length)]) + "\n")

def main():
        gen()

main()
