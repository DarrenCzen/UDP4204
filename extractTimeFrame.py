f = open("test.txt","r") #opens file with name of "test.txt"
myList = []

for line in f:
    if("Time(ms)" in line):
        newLine = line[10:]
        myList.append(float(newLine))

maxLen = len(myList)
print(maxLen)

confirmationList = []
confirmationList.append(myList[0])
print('{0:.3f}'.format(myList[0]))

for i in range (0, maxLen - 1):
    result = myList[i+1] - myList[i]
    confirmationList.append(result)
    print('{0:.3f}'.format(result))

print(len(confirmationList))
