import sys
import os
import stat
import fnmatch

def printhelp():
    print "fixnewlines [rootfolder]"

def fixFile( fname ):
    try:
        fin = open(fname)
    except:
        fin = None

    if fin:
        alllines = fin.readlines()
        fin.close()

        try:
            fout = open(fname,"w")
        except:
            fout = None

        if fout:
            for f in alllines:
                fout.write(f.rstrip()+"\n")
        else:
            print "Could not open",fname,"to write"
    else:
        print "Could not open",fname,"to read"


def dirWalkCallBack( arg, folder, files ):
    d = os.getcwd()
    os.chdir(folder)
    for f in files:
        if not os.path.isfile(f):
            continue
        if fnmatch.fnmatch( f, "*.h" ) or fnmatch.fnmatch(f, "*.cpp"):
            print "Fixing file %s" % f
            fixFile( f )
    os.chdir( d )

if len(sys.argv)<2:
    printhelp()
else:
    fname=sys.argv[1]
    if os.path.isfile( fname ):    
        fixFile( fname )
    else:
        print "Fixing folder %s" % fname
        os.path.walk( fname, dirWalkCallBack, None )

