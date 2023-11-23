import os
import sys
if(len(sys.argv)!=4):
    print("Usage:")
    print("python .\run.py host1 host2 datasize")
    print("Enter -1 as datasize for all required sizes")
    exit()
if(int(sys.argv[3])==-1):
    sizes=[1000,100000,1000000,8000000,32000000,64000000,128000000]
else:
    sizes=[int(sys.argv[3])]
hostpairs=[[int(sys.argv[1]),int(sys.argv[2])]]
for i in hostpairs:
    print("Hosts: csews",i[0]," csews",i[1])
    for j in sizes:
        print("DataSize: ",j)
        for k in range(0,10):
            cmd = "mpirun -np 2 -hosts csews"+str(i[0])+",csews"+str(i[1])+" ./code.x "+str(j)
            os.system(cmd)