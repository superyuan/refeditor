

def freadline(f):
    ''' read a line from file f, but skip comments lines '''
    line = f.readline()
    while len(line)>0 and (line[0]=='@' or line[0]=='#'):
        line = f.readline()
    return line    
