#!/usr/bin/python

# this script reads the DAQkit dump
# and dumps the output to the screen

from sys import argv,exit

# open the input file

if( len(argv) == 1 or len(argv) > 2 ):
    print "USAGE: ./dumpevent <filepath>" 
    exit()

try:
    DataFile = open(argv[1],'r')
except:
    print('Can not open ' + argv[1])
    exit()

# read the data
foundStartMarker = False
data = []
line = DataFile.readline()
while (line and line != '================ END OF DUMP ===================\n'):
    cells = line.strip().split()
    if( cells[0] == '00000000'):
        foundStartMarker = True
    if(foundStartMarker):
        data.append([int(i,16) for i in cells[2:4]]) 
        data.append([int(i,16) for i in cells[4:6]]) 
    line = DataFile.readline()


print len(data)
print data

#now translate the data into human readable output

for cWord in data:

    fullWord = ( cWord[1] << 32 ) + cWord[0]
#    print(hex(cWord[0]),hex(cWord[1]))
#    print hex(fullWord)

    if (fullWord >> 60) == 0x5 :
#        print "Header"
        print "Evt. ty" + str( (fullWord >> 56) & 0xF ) + " L1A Id " + str( (fullWord >> 32) & 0xFFFFFF) + " BX Id " + str( (fullWord >> 20) & 0xFFF ) + " Source Id " + str( (fullWord >> 8) & 0xFFF) + " FOV " + str( (fullWord >> 4) & 0xF) 
    elif (fullWord >> 60) == 0xa :
#        print "Trailer"
        print "Evt. Length " + str( (fullWord >> 32) & 0xFFFFFF ) + " CRC " + str( (fullWord >> 16) & 0xFFFF)
    else:
        print "Channel " + str( (cWord[0] >> 26) & 0x3F) + " ROC " +  str( (cWord[0] >> 21) & 0x1F) + " DC " + str( (cWord[0] >> 16) & 0x1F) + " Pxl " + str( (cWord[0] >> 8) & 0xFF) + " PH " + str(cWord[0] & 0xFF)
        print "Channel " + str( (cWord[1] >> 26) & 0x3F) + " ROC " +  str( (cWord[1] >> 21) & 0x1F) + " DC " + str( (cWord[1] >> 16) & 0x1F) + " Pxl " + str( (cWord[1] >> 8) & 0xFF) + " PH " + str(cWord[1] & 0xFF)
