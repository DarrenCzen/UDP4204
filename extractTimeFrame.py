f = open("test.txt", "r")  # opens file with name of "test.txt"
myList = []

method = input("[Option 1: 1 ACK | Option 2: 1 ACK 2 ACK]\n")

for line in f:
    if "Time(ms)" in line:
        newLine = line[10:]
        myList.append(float(newLine))

maxLen = len(myList)
print("Number of sets of data: ", maxLen)

confirmationList = []
confirmationList.append(myList[0])

for i in range(0, maxLen - 1):
    result = myList[i + 1] - myList[i]
    confirmationList.append(result)

print("Number of gaps: ", len(confirmationList))

print()

if method == '1':
    for line in confirmationList:
        print('{0:.3f}'.format(line))
elif method == '2':
    # split array into two
    oneAckList = []
    twoAckList = []

    for i in range(0, int(maxLen/2)):
        oneAckList.append(confirmationList[i*2])
        twoAckList.append(confirmationList[i*2 + 1])

    print('---------- 1 DU:', len(oneAckList), '----------\n')

    for line in oneAckList:
        print('{0:.3f}'.format(line))

    print()
    print('---------- 2 DU:', len(twoAckList), '----------\n')
    
    for line in twoAckList:
        print('{0:.3f}'.format(line))