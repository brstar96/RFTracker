import csv

csv.register_dialect(
    'mydialect',
    delimiter=',',
    quotechar='"',
    doublequote=True,
    skipinitialspace=True,
    lineterminator='\r\n',
    quoting=csv.QUOTE_MINIMAL)

# list 정의


arrayofdata = [['A', 'B', "C"],
               ['something', 'spam', 2.334],
               ['anything', 'spam', 0]]

with open('test4.csv', 'w', newline='') as mycsvfile:
    thedatawriter = csv.writer(mycsvfile, dialect='mydialect')
    for row in arrayofdata:
        thedatawriter.writerow(row)

with open('test4.csv', 'r') as mycsvfile:
    thedata = csv.reader(mycsvfile, dialect='mydialect')
    for row in thedata:
        print(row[0] + "\t \t" + row[1] + "\t \t" + row[2])